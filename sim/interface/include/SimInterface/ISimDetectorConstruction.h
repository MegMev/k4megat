#ifndef SIMG4INTERFACE_ISIMG4DETECTORCONSTRUCTION_H
#define SIMG4INTERFACE_ISIMG4DETECTORCONSTRUCTION_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
class G4VUserDetectorConstruction;

/** @class ISimDetectorConstruction SimInterface/SimInterface/DetectorConstruction.h ISimDetectorConstruction.h
 *
 *  abstract interface to load G4VUserDetectorConstruction
 *
 *  @author Benedikt HEGNER
 */

class ISimDetectorConstruction : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(ISimDetectorConstruction, 1, 0);

  /** get initilization hook for the geometry
   *  @return pointer to G4VUserDetectorConstruction
   */
  virtual G4VUserDetectorConstruction* detectorConstruction() = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4DETECTORCONSTRUCTION_H */
