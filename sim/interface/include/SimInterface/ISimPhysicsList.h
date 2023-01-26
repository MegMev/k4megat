#ifndef SIMG4INTERFACE_ISIMG4PHYSICSLIST_H
#define SIMG4INTERFACE_ISIMG4PHYSICSLIST_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
class G4VModularPhysicsList;

/** @class ISimPhysicsList SimInterface/SimInterface/ISimPhysicsList.h ISimPhysicsList.h
 *
 *  abstract interface to load physics lists
 *
 *  @author Benedikt HEGNER
 */

class ISimPhysicsList : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(ISimPhysicsList, 1, 0);

  /** get initilization hook for the physics list
   *  @return  pointer to G4VModularPhysicsList
   */
  virtual G4VModularPhysicsList* physicsList() = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4PHYSICSLIST_H */
