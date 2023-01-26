#ifndef SIMG4COMPONENTS_G4DD4HEPDETECTOR_H
#define SIMG4COMPONENTS_G4DD4HEPDETECTOR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimInterface/ISimDetectorConstruction.h"
class IGeoSvc;

/** @class SimDD4hepDetector SimComponents/src/SimDD4hepDetector.h SimDD4hepDetector.h
 *
 *  Detector construction tool using the DD4hep.
 *  Retrieves the geometry build by the implementation of IGeoSvc.
 *
 *  @author Anna Zaborowska
 */

class SimDD4hepDetector : public GaudiTool, virtual public ISimDetectorConstruction {
public:
  explicit SimDD4hepDetector(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimDD4hepDetector();
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
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
};

#endif /* SIMG4COMPONENTS_G4DD4HEPDETECTOR_H */
