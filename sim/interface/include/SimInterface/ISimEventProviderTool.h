#ifndef SIMG4INTERFACE_ISIMG4EVENTPROVIDERTOOL_H
#define SIMG4INTERFACE_ISIMG4EVENTPROVIDERTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4Event;

/** @class ISimEventProviderTool SimInterface/ISimEventProviderTool.h ISimEventProviderTool.h
 *
 *  Abstract interface to Geant4 Primary Generator classes
 *
 *  @author Andrea Dell'Acqua, J. Lingemann
 *  @date   2014-10-02
 */

class ISimEventProviderTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimEventProviderTool, 1, 0);

  /** get initilization hook for the geometry
   *  @return pointer to the G4Event containing primary particles
   */
  virtual G4Event* g4Event() = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4EVENTPROVIDERTOOL_H */
