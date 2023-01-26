#ifndef SIMG4INTERFACE_ISIMG4ACTIONTOOL_H
#define SIMG4INTERFACE_ISIMG4ACTIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
class G4VUserActionInitialization;

/** @class ISimActionTool SimInterface/SimInterface/ISimActionTool.h ISimActionTool.h
 *
 *  abstract interface to load initialization list of user actions
 *
 *  @author Benedikt HEGNER
 */

class ISimActionTool : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(ISimActionTool, 1, 0);

  /** get initilization hook for the user action
   *  @return  pointer to G4VUserActionInitialization
   */
  virtual G4VUserActionInitialization* userActionInitialization() = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4ACTIONTOOL_H */
