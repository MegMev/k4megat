#include "GaudiAlg/GaudiTool.h"
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimKernel/EventInformation.h"
#include "SimKernel/Units.h"
#include "k4FWCore/DataHandle.h"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "edm4hep/MCParticleCollection.h"

namespace megat {

  class SimSaveParticleHistory : public GaudiTool, virtual public ISimSaveOutputTool {
  public:
    explicit SimSaveParticleHistory( const std::string& aType, const std::string& aName, const IInterface* aParent )
        : GaudiTool( aType, aName, aParent ) {
      declareInterface<ISimSaveOutputTool>( this );
      declareProperty( "Hits", m_mcParticles, "Handle to the secondary particles" );
    }

    ~SimSaveParticleHistory() = default;

    StatusCode saveOutput( const G4Event& aEvent ) final {
      auto evtinfo = dynamic_cast<sim::EventInformation*>( aEvent.GetUserInformation() );
      // take over ownership of particle and vertex collections
      evtinfo->setCollections( m_mcParticleColl );
      debug() << "Saved " << m_mcParticleColl->size() << " particles from Geant4 history." << endmsg;
      m_mcParticles.put( m_mcParticleColl );

      return StatusCode::SUCCESS;
    }

  private:
    DataHandle<edm4hep::MCParticleCollection> m_mcParticles{ "SimParticleSecondaries", Gaudi::DataHandle::Writer,
                                                             this };
    edm4hep::MCParticleCollection*            m_mcParticleColl; // transient pointer
  };

  DECLARE_COMPONENT_WITH_ID( SimSaveParticleHistory, "SimSaveParticleHistory" )

} // namespace megat
