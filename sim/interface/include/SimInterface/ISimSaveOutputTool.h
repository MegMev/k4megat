#pragma once

#include "GaudiKernel/IAlgTool.h"

class G4Event;

class ISimSaveOutputTool : virtual public IAlgTool {
public:
  DeclareInterfaceID( ISimSaveOutputTool, 1, 0 );

  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput( const G4Event& aEvent ) = 0;
};
