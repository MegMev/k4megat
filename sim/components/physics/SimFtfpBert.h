#ifndef SIMG4COMPONENTS_G4FTFPBERT_H
#define SIMG4COMPONENTS_G4FTFPBERT_H
// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimInterface/ISimPhysicsList.h"

/** @class SimFtfpBert SimComponents/src/SimFtfpBert.h SimFtfpBert.h
 *
 *  FTFP_BERT physics list tool.
 *
 *  @author Anna Zaborowska
 */

class SimFtfpBert : public AlgTool, virtual public ISimPhysicsList {
public:
  explicit SimFtfpBert(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimFtfpBert();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList (ownership is transferred to the caller)
   */
  virtual G4VModularPhysicsList* physicsList();
private:
  Gaudi::Property<int> m_G4PhysicsVerbosityLevel{this, "G4PhysicsVerbosityLevel", 0, "Verbosity of G4 Physics printouts. 0: no printouts. 1: Verbose Printouts"};
};

#endif /* SIMG4COMPONENTS_G4FTFPBERT_H */
