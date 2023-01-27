#ifndef SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H
#define SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimInterface/ISimSaveOutputTool.h"
#include "k4FWCore/DataHandle.h"

// datamodel
namespace edm4hep {
  class ReconstructedParticleCollection;
  class MCRecoParticleAssociationCollection;
} // namespace edm4hep

namespace megat {

  /** @class SimSaveSmearedParticles SimComponents/src/SimSaveSmearedParticles.h SimSaveSmearedParticles.h
   *
   *  Save 'reconstructed' (smeared) particles.
   *
   *  @author Anna Zaborowska
   */

  class SimSaveSmearedParticles : public GaudiTool, virtual public ISimSaveOutputTool {
  public:
    explicit SimSaveSmearedParticles( const std::string& aType, const std::string& aName, const IInterface* aParent );
    virtual ~SimSaveSmearedParticles();
    /**  Initialize.
     *   @return status code
     */
    virtual StatusCode initialize();
    /**  Finalize.
     *   @return status code
     */
    virtual StatusCode finalize();
    /**  Save the data output.
     *   Saves the 'reconstructed' (smeared) particles and associates them with MC particles.
     *   @param[in] aEvent Event with data to save.
     *   @return status code
     */
    virtual StatusCode saveOutput( const G4Event& aEvent ) final;

  private:
    /// Handle for the particles to be written
    DataHandle<edm4hep::ReconstructedParticleCollection> m_particles{ "RecParticlesSmeared", Gaudi::DataHandle::Writer,
                                                                      this };
    /// Handle for the associations between particles and MC particles to be written
    DataHandle<edm4hep::MCRecoParticleAssociationCollection> m_particlesMCparticles{ "SmearedParticlesToParticles",
                                                                                     Gaudi::DataHandle::Writer, this };
  };
} // namespace megat

#endif /* SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H */
