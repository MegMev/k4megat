#ifndef SIMG4FAST_G4FASTSIMACTIONS_H
#define SIMG4FAST_G4FASTSIMACTIONS_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimInterface/ISimActionTool.h"

/** @class SimFastSimActions SimFast/src/components/SimFastSimActions.h SimFastSimActions.h
 *
 *  Tool for loading fast simulation user action initialization (list of user actions)
 *
 *  @author Anna Zaborowska
*/

class SimFastSimActions : public AlgTool, virtual public ISimActionTool {
public:
  explicit SimFastSimActions(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimFastSimActions();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /** Get the user action initialization.
   *  @return pointer to G4VUserActionInitialization (ownership is transferred to the caller)
   */
  virtual G4VUserActionInitialization* userActionInitialization() final;
};

#endif /* SIMG4FAST_G4FASTSIMACTIONS_H */
