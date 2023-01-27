#ifndef SIMG4FAST_G4USERLIMITPHYSICSLIST_H
#define SIMG4FAST_G4USERLIMITPHYSICSLIST_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimInterface/ISimPhysicsList.h"

namespace megat {

  /** @class SimUserLimitPhysicsList SimFast/src/components/SimUserLimitPhysicsList.h SimUserLimitPhysicsList.h
   *
   *  User limits physics list tool.
   *  Attaches G4StepLimiterPhysics process to the full simulation physics list.
   *
   *  @author Anna Zaborowska
   */

  class SimUserLimitPhysicsList : public AlgTool, virtual public ISimPhysicsList {
  public:
    explicit SimUserLimitPhysicsList( const std::string& aType, const std::string& aName, const IInterface* aParent );
    virtual ~SimUserLimitPhysicsList();

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

#endif /* SIMG4FAST_G4USERLIMITPHYSICSLIST_H */
