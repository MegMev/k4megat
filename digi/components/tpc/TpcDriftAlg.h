#pragma once

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

// DD4hep
#include "DD4hep/Segmentations.h"
#include <DD4hep/VolumeManager.h>
#include <DD4hep/Volumes.h>
#include <podio/GenericParameters.h>

/** @class TpcDriftAlg
 *
 *  Simulate the whole process:
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

class IGeoSvc;

namespace edm4hep {
  class SimTrackerHitCollection;
} // namespace edm4hep

class TpcDriftAlg : public GaudiAlgorithm {
public:
  explicit TpcDriftAlg( const std::string&, ISvcLocator* );
  virtual ~TpcDriftAlg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Data service: needed to register objects and get collection IDs. Just an observing pointer.
  PodioDataSvc* m_podioDataSvc;

  /// volume manager
  dd4hep::VolumeManager m_volMgr;

  /// Handle to EDM collection
  DataHandle<edm4hep::SimTrackerHitCollection> m_inHits{ "TpcHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::SimTrackerHitCollection> m_outHits{ "TpcDriftHits", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<int>  m_maxHits{ this, "maxHits", 1000, "Maxium number electron hits allowed" };
  Gaudi::Property<bool> use_Poisson{ this, "use_poisson", true,
                                     "Use Poisson process to generate primary electrons; otherwise simple mean." };
  // default Gas properties from Ar/CO2 90/10 200 V/cm.atm
  Gaudi::Property<float> m_wValue{ this, "wvalue", 25, "Mean activation energy in eV during primary ionization" };
  Gaudi::Property<float> m_transDiffCnst{ this, "trans_diffusion_const", 200., "Transverse diffusion constant" };
  Gaudi::Property<float> m_longDiffCnst{ this, "long_diffusion_const", 250.,
                                         "Longitudinal diffusion const in um/sqrt(cm)" };
  Gaudi::Property<float> m_driftVelocity{ this, "drift_velocity", 6.,
                                          "Mean activation energy in um/us during primary ionization" };
  Gaudi::Property<float> m_attachFactor{ this, "attach_factor", 0.0,
                                         "Electron attachement coefficient (dimensionless)" };

  // [todo: gas service]
  // Gaudi::Property<std::string> m_gas{ this, "gas", "DemoGas",
  //                                             "Name of the gas mix to be served by gas service" };
};
