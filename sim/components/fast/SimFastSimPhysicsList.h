#ifndef SIMG4FAST_G4FASTSIMPHYSICSLIST_H
#define SIMG4FAST_G4FASTSIMPHYSICSLIST_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimInterface/ISimPhysicsList.h"

namespace megat {

  /** @class SimFastSimPhysicsList SimFast/src/components/SimFastSimPhysicsList.h SimFastSimPhysicsList.h
   *
   *  Fast simulation physics list tool.
   *  Attaches parametrisation process to the full simulation physics list.
   *
   *  @author Anna Zaborowska
   */

  class SimFastSimPhysicsList : public AlgTool, virtual public ISimPhysicsList {
  public:
    explicit SimFastSimPhysicsList( const std::string& aType, const std::string& aName, const IInterface* aParent );
    virtual ~SimFastSimPhysicsList();

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
    /// Handle for the full physics list tool
    ToolHandle<ISimPhysicsList> m_physicsListTool{ "SimFtfpBert", this, true };
  };
} // namespace megat

#endif /* SIMG4FAST_G4FASTSIMPHYSICSLIST_H */
