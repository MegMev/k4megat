#include "GaudiAlg/GaudiTool.h"
#include "SimInterface/ISimEventProviderTool.h"
#include "details/io.h"

#include "G4GeneralParticleSource.hh"
#include "G4UImanager.hh"

#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <vector>

/**
 *  Tool that wraps G4's GPS generator.
 *  Use G4 macro commands for its configuration.
 *
 *  @author Yong Zhou
 *
 */

namespace fs = std::filesystem;

namespace megat {

  class SimG4GpsGenerator : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    SimG4GpsGenerator( const std::string& type, const std::string& name, const IInterface* parent )
        : GaudiTool( type, name, parent ) {
      declareInterface<ISimEventProviderTool>( this );
    }

    virtual StatusCode initialize() final;
    virtual G4Event*   g4Event() final;

  private:
    /// macro filename with GPS commands (must start with /gps)
    Gaudi::Property<std::string> m_macroFile{ this, "MacroFile", "", "Geant4 commands for GPS configuration" };

    std::vector<std::string>                 m_cmds;
    std::unique_ptr<G4GeneralParticleSource> m_gps{ nullptr };
  };

  StatusCode SimG4GpsGenerator::initialize() {
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

    // 1. read in gps commands
    if ( m_macroFile.empty() || !fs::exists( m_macroFile.value() ) ) {
      error() << "Can't find macro file: " << m_macroFile << " (for GPS specification)" << endmsg;
      return StatusCode::FAILURE;
    }
    m_cmds = getLinesWithPrefix( m_macroFile, "/gps" );

    // 2. create the generator
    m_gps = std::make_unique<G4GeneralParticleSource>();

    // 3. apply the commands
    auto* _ui_mgr = G4UImanager::GetUIpointer();
    for ( auto command : m_cmds ) { _ui_mgr->ApplyCommand( command ); }

    return StatusCode::SUCCESS;
  }

  G4Event* SimG4GpsGenerator::g4Event() {
    auto* theEvent = new G4Event();
    m_gps->GeneratePrimaryVertex( theEvent );
    debug() << "thread id: " << std::this_thread::get_id() << endmsg;

    return theEvent;
  }

  // component declaration
  DECLARE_COMPONENT_WITH_ID( SimG4GpsGenerator, "SimG4GpsGenerator" )

} // namespace megat
