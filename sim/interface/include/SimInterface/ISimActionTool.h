#pragma once

#include "GaudiKernel/IAlgTool.h"

class G4VUserActionInitialization;

class ISimActionTool : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID( ISimActionTool, 1, 0 );

  /** get initilization hook for the user action
   *  @return  pointer to G4VUserActionInitialization
   */
  virtual G4VUserActionInitialization* userActionInitialization() = 0;
};
