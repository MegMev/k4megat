#ifndef SIMG4COMPONENTS_G4SIMALG_H
#define SIMG4COMPONENTS_G4SIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "k4FWCore/DataHandle.h"
#include "SimInterface/ISimEventProviderTool.h"
#include "SimInterface/ISimSaveOutputTool.h"

// Forward declarations:
// Interfaces
class ISimSvc;

// Geant
class G4Event;

/** @class SimAlg SimComponents/src/SimAlg.h SimAlg.h
 *
 *  Geant simulation algorithm.
 *  Controls the event flow: translates the EDM event to G4Event, passes it to SimSvc,
 *  retrieves it after the finished simulation, and stores the output as specified in tools.
 *  It takes MCParticleCollection (\b'genParticles') as the input
 *  as well as a list of names of tools that define the EDM output (\b'outputs').
 *  [For more information please see](@ref md_sim_doc_geant4fullsim).
 *
 *  @author Anna Zaborowska
 */

class SimAlg : public GaudiAlgorithm {
public:
  explicit SimAlg(const std::string&, ISvcLocator*);
  virtual ~SimAlg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute the simulation.
   *   Translation of MCParticleCollection to G4Event is done using EDM2G4() method.
   *   Then, G4Event is passed to SimSvc for the simulation and retrieved afterwards.
   *   The tools m_saveTools are used to save the output from the retrieved events.
   *   Finally, the event is terminated.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Pointer to the interface of Geant simulation service
  ServiceHandle<ISimSvc> m_geantSvc;
  /// Handle to the tools saving the output
  /// to be replaced with the ToolHandleArray<ISimSaveOutputTool> m_saveTools
  std::vector<ISimSaveOutputTool*> m_saveTools;
  /// Names for the saving tools
  /// to be deleted once the ToolHandleArray<ISimSaveOutputTool> m_saveTools is in place
  Gaudi::Property<std::vector<std::string>> m_saveToolNames{this, "outputs", {}, "Names for the saving tools"};
  /// Handle for tool that creates the G4Event
  ToolHandle<ISimEventProviderTool> m_eventTool{"SimPrimariesFromEdmTool", this};
};
#endif /* SIMG4COMPONENTS_G4SIMALG_H */
