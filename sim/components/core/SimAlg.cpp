#include "SimInterface/ISimEventProviderTool.h"
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimInterface/ISimSvc.h"
#include "SimInterface/ISimVertexSmearTool.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "k4FWCore/DataHandle.h"

#include "G4Event.hh"
#include <CLHEP/Vector/ThreeVector.h>

/** @class SimAlg SimComponents/src/SimAlg.h SimAlg.h
 *
 *  Geant simulation algorithm.
 *  Controls the event flow:
 *  - translates the EDM event to G4Event
 *  - smear the primary vertex
 *  - passes it to ISimSvc for simulation
 *  - retrieves it after the finished simulation, and stores the output as specified in tools.
 *
 *  @author Anna Zaborowska
 *
 *  - add the vertex smearing tool
 *
 *  @modified Yong Zhou
 */

namespace megat {

  class SimAlg : public GaudiAlgorithm {
  public:
    explicit SimAlg( const std::string&, ISvcLocator* );

    virtual StatusCode initialize() final;
    virtual StatusCode execute() final;

  private:
    void smearVertex( G4Event* );

  private:
    /// Pointer to the interface of Geant simulation service
    ServiceHandle<ISimSvc> m_geantSvc;

    /// Handle to the tools saving the output
    /// to be replaced with the ToolHandleArray<ISimSaveOutputTool> m_saveTools
    std::vector<ISimSaveOutputTool*> m_saveTools;
    /// Names for the saving tools
    /// to be deleted once the ToolHandleArray<ISimSaveOutputTool> m_saveTools is in place
    Gaudi::Property<std::vector<std::string>> m_saveToolNames{ this, "outputs", {}, "Names for the saving tools" };

    /// Handle for tool that creates the G4Event, default is from edm persistency
    ToolHandle<ISimEventProviderTool> m_eventTool{ "SimPrimariesFromEdmTool", this };

    /// Handle for tool that smears the generated vertex from event provider, default it volume-based
    ToolHandle<ISimVertexSmearTool> m_vxSmearTool{ "SimVertexSmearVolumeTool", this };
    bool                            m_vxSmearFlag{ true };
  };

  SimAlg::SimAlg( const std::string& aName, ISvcLocator* aSvcLoc )
      : GaudiAlgorithm( aName, aSvcLoc ), m_geantSvc( "SimSvc", aName ) {
    declareProperty( "eventProvider", m_eventTool, "Tool that creates the G4Event" );
    declareProperty( "vertexSmearer", m_vxSmearTool, "Tool that smears the primary vertex" );
  }

  StatusCode SimAlg::initialize() {
    if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

    //
    if ( !m_geantSvc ) {
      error() << "Unable to locate Geant Simulation Service" << endmsg;
      return StatusCode::FAILURE;
    }

    //
    for ( auto& toolname : m_saveToolNames ) {
      m_saveTools.push_back( tool<ISimSaveOutputTool>( toolname ) );
      // FIXME: check StatusCode once the m_saveTools is a ToolHandleArray
      // if (!) {
      //   error() << "Unable to retrieve the output saving tool." << endmsg;
      //   return StatusCode::FAILURE;
      // }
    }

    //
    if ( !m_eventTool.retrieve() ) {
      error() << "Unable to retrieve the G4Event provider " << m_eventTool << endmsg;
      return StatusCode::FAILURE;
    }

    //
    if ( !m_vxSmearTool.retrieve() ) {
      m_vxSmearFlag = false;
      debug() << "No vertex smearing performed" << endmsg;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode SimAlg::execute() {
    // first translate the event
    G4Event* event = m_eventTool->g4Event();
    if ( !event ) {
      error() << "Unable to retrieve G4Event from " << m_eventTool << endmsg;
      return StatusCode::FAILURE;
    }

    // smear the primary vertex
    if ( m_vxSmearFlag ) { smearVertex( event ); }

    // pass to g4 engine for simulation
    m_geantSvc->processEvent( *event ).ignore();

    // save the results
    G4Event* constevent;
    m_geantSvc->retrieveEvent( constevent ).ignore();
    for ( auto& tool : m_saveTools ) { tool->saveOutput( *constevent ).ignore(); }

    // terminate current event simulation
    m_geantSvc->terminateEvent().ignore();

    return StatusCode::SUCCESS;
  }

  void SimAlg::smearVertex( G4Event* aEvent ) {
    // get the smear offset
    auto offset = m_vxSmearTool->smearVertex();

    // adding to the original vertex
    auto cur = aEvent->GetPrimaryVertex();
    while ( cur ) {
      auto vertex = offset + static_cast<CLHEP::Hep3Vector>( cur->GetPosition() );
      cur->SetPosition( vertex.x(), vertex.y(), vertex.z() );

      cur = cur->GetNext();
    }
  }

  DECLARE_COMPONENT_WITH_ID( SimAlg, "SimAlg" )
} // namespace megat
