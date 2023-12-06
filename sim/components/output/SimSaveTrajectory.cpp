#include "GaudiAlg/GaudiTool.h"
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimKernel/Units.h"
#include "k4FWCore/DataHandle.h"

// Geant4
#include "G4Event.hh"
#include "G4Trajectory.hh"
#include "G4VTrajectory.hh"

// datamodel
#include "edm4hep/TrackerHitCollection.h"

namespace megat {

  class SimSaveTrajectory : public GaudiTool, virtual public ISimSaveOutputTool {

  public:
    explicit SimSaveTrajectory( const std::string& aType, const std::string& aName, const IInterface* aParent )
        : GaudiTool( aType, aName, aParent ) {
      declareInterface<ISimSaveOutputTool>( this );
      declareProperty( "Hits", m_trackHits, "Collection Name of Trajectory Points" );
    }

    ~SimSaveTrajectory() = default;

    StatusCode saveOutput( const G4Event& aEvent ) final {
      auto                   edmPositions        = m_trackHits.createAndPut();
      G4TrajectoryContainer* trajectoryContainer = aEvent.GetTrajectoryContainer();
      for ( size_t trajectoryIndex = 0; trajectoryIndex < trajectoryContainer->size(); ++trajectoryIndex ) {
        G4VTrajectory* theTrajectory = ( *trajectoryContainer )[trajectoryIndex];
        for ( int pointIndex = 0; pointIndex < theTrajectory->GetPointEntries(); ++pointIndex ) {
          auto trajectoryPoint = theTrajectory->GetPoint( pointIndex )->GetPosition();
          auto edmHit          = edmPositions->create();
          edmHit.setCellID( 0 );
          edmHit.setEDep( 0 );
          edmHit.setTime( 0 );
          edmHit.setPosition( {
              trajectoryPoint.x() * g42edm::length,
              trajectoryPoint.y() * g42edm::length,
              trajectoryPoint.z() * g42edm::length,
          } );
        }
      }

      return StatusCode::SUCCESS;
    }

  private:
    DataHandle<edm4hep::TrackerHitCollection> m_trackHits{ "Hits/Trajectory", Gaudi::DataHandle::Writer, this };
  };

  DECLARE_COMPONENT_WITH_ID( SimSaveTrajectory, "SimSaveTrajectory" )

} // namespace megat
