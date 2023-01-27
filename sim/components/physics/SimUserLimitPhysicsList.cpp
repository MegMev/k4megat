#include "SimUserLimitPhysicsList.h"

// FCCSW
#include "SimInterface/ISimPhysicsList.h"

// Geant4
#include "G4StepLimiterPhysics.hh"
#include "G4VModularPhysicsList.hh"

namespace megat {

  DECLARE_COMPONENT_WITH_ID( SimUserLimitPhysicsList, "SimUserLimitPhysicsList" )

  SimUserLimitPhysicsList::SimUserLimitPhysicsList( const std::string& aType, const std::string& aName,
                                                    const IInterface* aParent )
      : AlgTool( aType, aName, aParent ) {
    declareInterface<ISimPhysicsList>( this );
    declareProperty( "fullphysics", m_physicsListTool, "Handle for the full physics list tool" );
  }

  SimUserLimitPhysicsList::~SimUserLimitPhysicsList() {}

  StatusCode SimUserLimitPhysicsList::initialize() {
    if ( AlgTool::initialize().isFailure() ) { return StatusCode::FAILURE; }
    m_physicsListTool.retrieve().ignore();
    return StatusCode::SUCCESS;
  }

  StatusCode SimUserLimitPhysicsList::finalize() { return AlgTool::finalize(); }

  G4VModularPhysicsList* SimUserLimitPhysicsList::physicsList() {
    // ownership passed to SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
    G4VModularPhysicsList* physicsList = m_physicsListTool->physicsList();
    // Attach step limiter process
    physicsList->RegisterPhysics( new G4StepLimiterPhysics() );
    return physicsList;
  }
} // namespace megat
