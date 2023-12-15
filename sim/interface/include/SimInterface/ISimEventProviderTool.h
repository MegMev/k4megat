#pragma once

#include "GaudiKernel/IAlgTool.h"

class G4Event;

class ISimEventProviderTool : virtual public IAlgTool {
public:
  DeclareInterfaceID( ISimEventProviderTool, 1, 0 );

  /** get initilization hook for the geometry
   *  @return pointer to the G4Event containing primary particles
   */
  virtual G4Event* g4Event() = 0;
};
