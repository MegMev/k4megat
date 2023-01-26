#ifndef SIMG4COMPONENTS_G4GEANTINODEPOSITS_H
#define SIMG4COMPONENTS_G4GEANTINODEPOSITS_H
// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimInterface/ISimPhysicsList.h"


class SimGeantinoDeposits: public AlgTool, virtual public ISimPhysicsList {
public:
  explicit SimGeantinoDeposits(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~SimGeantinoDeposits() = default;

  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() override final;

  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() override final;

  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList (ownership is transferred to the caller)
   */
  virtual G4VModularPhysicsList* physicsList() override final;

};

#endif /* SIMG4COMPONENTS_G4GEANTINODEPOSITS_H */
