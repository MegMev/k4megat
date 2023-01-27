#ifndef SIMG4COMPONENTS_G4SIMSVC_H
#define SIMG4COMPONENTS_G4SIMSVC_H

// FCCSW
#include "SimInterface/ISimActionTool.h"
#include "SimInterface/ISimDetectorConstruction.h"
#include "SimInterface/ISimMagneticFieldTool.h"
#include "SimInterface/ISimPhysicsList.h"
#include "SimInterface/ISimRegionTool.h"
#include "SimInterface/ISimSvc.h"
#include "SimKernel/RunManager.h"

// Gaudi
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"

#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

/** @class SimSvc SimComponents/SimComponents/SimSvc.h SimSvc.h
 *
 *  Main Geant simulation service.
 *  It handles Geant initialization (via tools) and communication with the G4RunManager.
 *  [For more information please see](@ref md_sim_doc_geant4fullsim).
 *
 *  @author Anna Zaborowska
 */
namespace megat {

  class SimSvc : public extends1<Service, ISimSvc> {
  public:
    /// Standard constructor
    explicit SimSvc( const std::string& aName, ISvcLocator* aSL );
    /// Standard destructor
    virtual ~SimSvc();
    /**  Initialize the Geant simulation service.
     *   Tools set in the configuration file need to specify the detector construction,
     *   physics list and user action initialization to initialize G4RunManager.
     *   @return status code
     */
    virtual StatusCode initialize() final;
    /**  Finalize the Geant simulation service.
     *   @return status code
     */
    virtual StatusCode finalize() final;
    /**  Simulate the event with Geant.
     *   @param[in] aEvent An event to be processed.
     *   @return status code
     */
    StatusCode processEvent( G4Event& aEvent );
    /**  Retrieve the processed event.
     *   @param[out] aEvent The processed event.
     *   @return status code
     */
    StatusCode retrieveEvent( G4Event*& aEvent );
    /**  Terminate the event simulation.
     *   @return status code
     */
    StatusCode terminateEvent();

  private:
    /// Pointer to the tool service
    SmartIF<IToolSvc> m_toolSvc;
    /// Pointer to the random numbers service
    SmartIF<IRndmGenSvc> m_randSvc;
    /// Handle for the detector construction tool
    ToolHandle<ISimDetectorConstruction> m_detectorTool{ "SimDD4hepDetector", this, true };
    /// Handle for the Geant physics list tool
    ToolHandle<ISimPhysicsList> m_physicsListTool{ "SimFtfpBert", this, true };
    /// Handle for the user action initialization
    ToolHandle<ISimActionTool> m_actionsTool{ "SimFullSimActions", this, true };
    /// Handle for the magnetic field initialization
    ToolHandle<ISimMagneticFieldTool> m_magneticFieldTool{ "SimConstantMagneticFieldTool", this, true };
    /// Geant4 commands to be executed before user initialization
    Gaudi::Property<std::vector<std::string>> m_g4PreInitCommands{
        this, "g4PreInitCommands", {}, "Geant4 commands to be executed before user initialization" };
    /// Geant4 commands to be executed after user initialization
    Gaudi::Property<std::vector<std::string>> m_g4PostInitCommands{
        this, "g4PostInitCommands", {}, "Geant4 commands to be executed after user initialization" };
    /// Handles to the tools creating regions and fast simulation models
    /// to be replaced with the ToolHandleArray<ISimRegionTool> m_regionTools
    std::vector<ISimRegionTool*> m_regionTools;
    /// Names of the tools that create regions and fast simulation models
    /// to be deleted once the ToolHandleArray<ISimRegionTool> m_regionTools is in place
    Gaudi::Property<std::vector<std::string>> m_regionToolNames{
        this, "regions", {}, "Names of the tools that create regions and fast simulation models" };
    /// Flag whether random numbers seeds should be taken from Gaudi (default: true)
    Gaudi::Property<bool> m_rndmFromGaudi{ this, "randomNumbersFromGaudi", true,
                                           "Whether random numbers should be taken from Gaudi" };
    Gaudi::Property<long> m_seedValue{
        this, "seedValue", 1234567,
        "Seed to be used in RndmGenSvc engine (randomNumbersFromGaudi must be set to false)" };

    Gaudi::Property<bool> m_interactiveMode{ this, "InteractiveMode", false, "Enter the interactive mode" };

    /// Run Manager
    sim::RunManager m_runManager;

    std::unique_ptr<G4VisManager> m_visManager{ nullptr };
    // Define UI terminal for interactive mode
    std::unique_ptr<G4UIsession> m_session{ nullptr };
  };

} // namespace megat

#endif /* SIMG4COMPONENTS_G4SIMSVC_H */
