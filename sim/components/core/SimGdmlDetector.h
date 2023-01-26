#ifndef SIMG4COMPONENTS_G4GDMLDETECTOR_H
#define SIMG4COMPONENTS_G4GDMLDETECTOR_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimInterface/ISimDetectorConstruction.h"

// Geant4
#include "G4GDMLParser.hh"

/** @class SimGdmlDetector SimComponents/src/SimGdmlDetector.h SimGdmlDetector.h
 *
 *  Detector construction tool using the GDML file.
 *  GDML file name needs to be specified in job options file (\b'gdml').
 *
 *  @author Anna Zaborowska
 */

class SimGdmlDetector : public AlgTool, virtual public ISimDetectorConstruction {
public:
  explicit SimGdmlDetector(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimGdmlDetector();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the initilization of the geometry.
   *  @return pointer to G4VUserDetectorConstruction (ownership is transferred to the caller)
   */
  virtual G4VUserDetectorConstruction* detectorConstruction();

private:
  /// name of the GDML file
  Gaudi::Property<std::string> m_gdmlFile{this, "gdml", "", "name of the GDML file"};
  // validate gdml schema
  Gaudi::Property<bool> m_validate{this, "validateGDMLSchema", false, "try to validate the GDML schema"};
};

#endif /* SIMG4COMPONENTS_G4GDMLDETECTOR_H */
