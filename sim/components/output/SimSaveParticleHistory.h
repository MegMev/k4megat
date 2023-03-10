#ifndef SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H
#define SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimKernel/EventInformation.h"
#include "k4FWCore/DataHandle.h"

class IGeoSvc;

// datamodel
namespace edm4hep {
  class MCParticleCollection;
}

namespace megat {

  namespace sim {
    class EventInformation;
  }

  /** @class SimSaveParticleHistory SimComponents/src/SimSaveParticleHistory.h SimSaveParticleHistory.h
   *
   *  This tool allows to save the particle history of particles decaying during the simulation
   *
   *  @author J. Lingemann
   *  @author V. Volkl
   */

  class SimSaveParticleHistory : public GaudiTool, virtual public ISimSaveOutputTool {
  public:
    explicit SimSaveParticleHistory( const std::string& aType, const std::string& aName, const IInterface* aParent );
    virtual ~SimSaveParticleHistory() = default;

    /**  Save the history
     *   Creates particles and gen vertices that allow association of parents and children
     *   @param[in] aEvent The Geant Event conatining data to save.
     *   @return status code
     */
    StatusCode saveOutput( const G4Event& aEvent ) override final;

  private:
    /// Handle for collection of MC particles to create
    DataHandle<edm4hep::MCParticleCollection> m_mcParticles{ "SimParticleSecondaries", Gaudi::DataHandle::Writer,
                                                             this };
    /// Pointer to the particle collection, ownership should be handled in a algorithm / tool
    edm4hep::MCParticleCollection* m_mcParticleColl;
  };
} // namespace megat

#endif /* SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H */
