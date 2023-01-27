#ifndef SIMG4FULL_G4FULLSIMACTIONS_H
#define SIMG4FULL_G4FULLSIMACTIONS_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/SystemOfUnits.h"


// FCCSW
#include "SimInterface/ISimActionTool.h"

namespace megat {

/** @class SimFullSimActions SimFull/src/components/SimFullSimActions.h SimFullSimActions.h
 *
 *  Tool for loading full simulation user action initialization (list of user actions)
 *
 *  @author Anna Zaborowska
 */

class SimFullSimActions : public AlgTool, virtual public ISimActionTool {
public:
  explicit SimFullSimActions(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimFullSimActions();

  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the user action initialization.
   *  @return pointer to G4VUserActionInitialization (ownership is transferred to the caller)
   */
  virtual G4VUserActionInitialization* userActionInitialization() final;

private:
  /// Set to true to save secondary particle info
  Gaudi::Property<bool> m_enableHistory{this, "enableHistory", false, "Set to true to save secondary particle info"};
  Gaudi::Property<double> m_energyCut{this, "energyCut", 0.0 * Gaudi::Units::GeV, "minimum energy for secondaries to be saved"};
};

} // namespace megat

#endif /* SIMG4FULL_G4FULLSIMACTIONS_H */
