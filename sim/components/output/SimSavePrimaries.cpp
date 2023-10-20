#include "SimInterface/ISimSaveOutputTool.h"
#include "SimKernel/EventInformation.h"
#include "SimKernel/Units.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "edm4hep/MCParticleCollection.h"
#include "k4FWCore/DataHandle.h"

namespace megat {

  class SimSavePriamries : public GaudiTool, virtual public ISimSaveOutputTool {
  public:
    explicit SimSavePriamries( const std::string& aType, const std::string& aName, const IInterface* aParent )
        : GaudiTool( aType, aName, aParent ) {
      declareInterface<ISimSaveOutputTool>( this );
      declareProperty( "Particles", m_mcParticles, "Handle to the primary particles" );
    }

    StatusCode saveOutput( const G4Event& aEvent ) final {
      auto out_hits = m_mcParticles.createAndPut();

      auto cur_vtx = aEvent.GetPrimaryVertex();
      while ( cur_vtx ) {
        auto cur_pos = cur_vtx->GetPosition();
        auto cur_t0  = cur_vtx->GetT0();

        //
        auto cur_par = cur_vtx->GetPrimary();
        while ( cur_par ) {
          auto new_hit = out_hits->create();

          //
          new_hit.setVertex( {
              cur_pos.x() * g42edm::length,
              cur_pos.y() * g42edm::length,
              cur_pos.z() * g42edm::length,
          } );
          new_hit.setTime( cur_t0 * Gaudi::Units::c_light * g42edm::length );

          new_hit.setPDG( cur_par->GetPDGcode() );
          new_hit.setCharge( cur_par->GetCharge() );
          new_hit.setGeneratorStatus( 1 );
          new_hit.setMomentum( {
              (float)( cur_par->GetPx() * g42edm::energy ),
              (float)( cur_par->GetPy() * g42edm::energy ),
              (float)( cur_par->GetPz() * g42edm::energy ),
          } );
          new_hit.setMass( cur_par->GetMass() * g42edm::energy );

          //
          cur_par = cur_par->GetNext();
        }

        //
        cur_vtx = cur_vtx->GetNext();
      }

      return StatusCode::SUCCESS;
    }

  private:
    DataHandle<edm4hep::MCParticleCollection> m_mcParticles{ "GenParticles", Gaudi::DataHandle::Writer, this };
  };

  DECLARE_COMPONENT_WITH_ID( SimSavePriamries, "SimSavePriamries" )
} // namespace megat
