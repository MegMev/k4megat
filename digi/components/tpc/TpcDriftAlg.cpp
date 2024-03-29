#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "SimInterface/IGeoSvc.h"
#include "SimKernel/Units.h"

#include "edm4hep/SimTrackerHitCollection.h"
#include "k4FWCore/DataHandle.h"

#include "DD4hep/BitFieldCoder.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DDRec/Surface.h"
#include <CLHEP/Units/SystemOfUnits.h>
#include <DDRec/Vector3D.h>
#include <boost/iterator/iterator_categories.hpp>
#include <cfloat>
#include <cmath>
#include <numeric>

/** @class TpcDriftAlg
 *
 *  Simulate the whole process of electron drifting to anode plane:
 *  1. primary ionization (param m_wvalue: average ionization energy)
 *  2. diffusion (param m_lDiffusionK m_tDiffusionK: long and trans diffusion constant)
 *  3. drift to anode surface and transformed to time (param m_driftVelocity: drift velocity)
 *
 *  Input: G4 output, sensitive gas without segmentation
 *
 *  Output: collection of electrons drifted to anode surface
 *         - cellID: no change (i.e. system id of subdetector)
 *         - electron generated from same hit have same energy: etotal / nr_electrons
 *         - local x, y: hit position + diffusion (gauss randmom)
 *         - z: on anode surface
 *         - time: hit_time + drift_time (d_to_anode - l_diffusion)
 *
 *  Note: drift and diffusion coordinates are in local systeme of drift volume
 *
 *  @author Yong Zhou
 */

using namespace megat;
using dd4hep::rec::Surface;
using dd4hep::rec::Vector3D;

static constexpr double max_boundary = std::numeric_limits<double>::max();

class TpcDriftAlg : public GaudiAlgorithm {
public:
  explicit TpcDriftAlg( const std::string& aName, ISvcLocator* aSvcLoc )
      : GaudiAlgorithm( aName, aSvcLoc ), m_geoSvc( "GeoSvc", aName ) {
    declareProperty( "inHits", m_inHits, "Simulated Hit Collection of Tpc" );
    declareProperty( "outHits", m_outHits, "Drifted Electron Hit Collection" );
  }

  virtual StatusCode initialize() final {
    if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

    /// services
    if ( !m_geoSvc ) {
      error() << "Unable to locate Geometry Service. "
              << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
      return StatusCode::FAILURE;
    }

    /// flat rndm
    auto sc = m_flatGen.initialize( randSvc(), Rndm::Flat( 0.0, 1.0 ) );
    if ( !sc.isSuccess() ) { return StatusCode::FAILURE; }

    /// unit conversion
    static const double unit_diffusion_coeff = CLHEP::um / std::sqrt( CLHEP::cm );
    static const double unit_drift_velocity  = CLHEP::cm / CLHEP::us;

    m_wValue        = m_wValue * CLHEP::eV;
    m_driftVelocity = m_driftVelocity * unit_drift_velocity;
    m_transDiffCnst = m_transDiffCnst * unit_diffusion_coeff;
    m_longDiffCnst  = m_longDiffCnst * unit_diffusion_coeff;

    return StatusCode::SUCCESS;
  }

  virtual StatusCode execute() final {
    /// input
    const auto in_hits = m_inHits.get();

    // 1 -> eff_wvalue
    auto  e_vec         = in_hits->EDep();
    auto  tot_edep      = std::accumulate( e_vec.begin(), e_vec.end(), 0. ) * edmdefault::energy;
    int   tot_eno       = tot_edep / m_wValue;
    float eno_cor_ratio = ( tot_eno > m_maxHits ) ? ( tot_edep / m_maxHits / m_wValue ) : 1.0;

    /// output
    auto out_hits = m_outHits.createAndPut();
    int  nr_in_e{ 0 }, nr_tot_e{ 0 };

    // 2. loop input hits
    for ( const auto& hit : *in_hits ) {
      auto cellid        = hit.getCellID();
      auto sL            = m_geoSvc->getHelperSurfList( cellid );
      auto anode_surf    = static_cast<Surface*>( sL.front() );
      auto anode_volSurf = anode_surf->volSurface();

      auto     _gpos = hit.getPosition();
      Vector3D gpos{ _gpos.x, _gpos.y, _gpos.z };

      // 3.1 -> drift distance & diffusion width [edm unit]
      auto          drift_d = std::abs( anode_surf->distance( edm2dd::length * gpos ) ) * dd2edm::length;
      Rndm::Numbers trans_diff_gen( randSvc(), Rndm::Gauss( 0, std::sqrt( drift_d ) * m_transDiffCnst ) );
      Rndm::Numbers long_diff_gen( randSvc(), Rndm::Gauss( 0, std::sqrt( drift_d ) * m_longDiffCnst ) );

      // 3.2 -> local position in drift volume [dd unit] (since diffusion in local)
      auto lpos = anode_surf->globalToLocal( edm2dd::length * gpos );

      // 3.3 attach model [todo: what's the basics of this formula?]
      auto is_attached = [&]() -> bool {
        if ( !m_attachFactor ) return false;
        return m_flatGen() > std::pow( 1. - m_attachFactor, drift_d / CLHEP::cm );
      };

      // 4. -> electron number & kinetic energy
      int cur_eno = hit.getEDep() * edmdefault::energy / m_wValue;
      if ( m_usePoisson ) {
        Rndm::Numbers poisson( randSvc(), Rndm::Poisson( cur_eno ) );
        cur_eno = std::round( poisson() / eno_cor_ratio );
      } else {
        cur_eno = std::round( cur_eno / eno_cor_ratio );
      }
      if ( cur_eno == 0 && hit.getEDep() > 0 ) cur_eno = 1;

      // kinetic energy of each electron for output
      auto e_kin = hit.getEDep() / cur_eno;
      nr_tot_e += cur_eno;

      // 5. loop electrons
      while ( cur_eno > 0 ) {
        cur_eno--;

        // ? attached
        if ( !is_attached() ) {
          // 6.1 -> diffusion distance [dd unit]
          auto xdiff = lpos.u() + trans_diff_gen() * edm2dd::length;
          auto ydiff = lpos.v() + trans_diff_gen() * edm2dd::length;

          // ? still inside drift volume
          if ( m_discardOutBound && !anode_volSurf.insideBounds( { xdiff, ydiff, 0 }, max_boundary ) ) { continue; }
          nr_in_e++;

          // 6.2 -> new global pos on anode surface [edm unit]
          auto new_gpos = dd2edm::length * anode_surf->localToGlobal( { xdiff, ydiff } );

          // 6.3 -> drift time [edm unit]
          double drift_t = ( drift_d + long_diff_gen() ) / m_driftVelocity / edmdefault::time;

          // 6.4 -> new hit
          auto new_hit = out_hits->create();
          new_hit.setCellID( cellid );
          new_hit.setEDep( e_kin );
          new_hit.setTime( drift_t );
          new_hit.setPosition( { new_gpos.x(), new_gpos.y(), new_gpos.z() } );
        }
      }
    }
    m_nrElectronGen.put( new int{ nr_tot_e } );
    m_nrElectronIn.put( new int{ nr_in_e } );
    m_nrElectronEsc.put( new int{ nr_tot_e - nr_in_e } );

    return StatusCode::SUCCESS;
  }

private:
  ServiceHandle<IGeoSvc> m_geoSvc;
  Rndm::Numbers          m_flatGen;

  /// Handle to EDM collection
  DataHandle<edm4hep::SimTrackerHitCollection> m_inHits{ "TpcHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::SimTrackerHitCollection> m_outHits{ "TpcDriftHits", Gaudi::DataHandle::Writer, this };

  /// Handle to other data types
  DataHandle<int> m_nrElectronGen{ "NrElctronTotal", Gaudi::DataHandle::Writer, this };
  DataHandle<int> m_nrElectronIn{ "NrElctronInside", Gaudi::DataHandle::Writer, this };
  DataHandle<int> m_nrElectronEsc{ "NrElctronEscape", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<int>  m_maxHits{ this, "maxHits", 1000, "Maxium number electron hits allowed" };
  Gaudi::Property<bool> m_usePoisson{ this, "usePoisson", true,
                                      "Use Poisson process to generate primary electrons; otherwise simple mean." };
  Gaudi::Property<bool> m_discardOutBound{ this, "discardOutBound", true,
                                           "Don't keep electrons outside of drift volume." };

  // default Gas properties from Ar/CO2 90/10 200 V/cm.atm
  Gaudi::Property<float> m_wValue{ this, "wvalue", 25, "[eV] Mean activation energy during primary ionization" };
  Gaudi::Property<float> m_transDiffCnst{ this, "trans_diffusion_const", 200.,
                                          "[um/sqrt(cm)] Transverse diffusion constant" };
  Gaudi::Property<float> m_longDiffCnst{ this, "long_diffusion_const", 250.,
                                         "[um/sqrt(cm)] Longitudinal diffusion constant" };
  Gaudi::Property<float> m_driftVelocity{ this, "drift_velocity", 6.,
                                          "[cm/us] Mean activation energy in um/us during primary ionization" };
  Gaudi::Property<float> m_attachFactor{ this, "attach_factor", 0.0,
                                         "Electron attachement coefficient (dimensionless)" };

  // [todo: gas service]
  // Gaudi::Property<std::string> m_gas{ this, "gas", "DemoGas",
  //                                             "Name of the gas mix to be served by gas service" };
};

DECLARE_COMPONENT( TpcDriftAlg )
