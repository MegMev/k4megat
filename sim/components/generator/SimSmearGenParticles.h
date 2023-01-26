#ifndef SIMG4COMPONENTS_G4SMEARGENPARTICLES_H
#define SIMG4COMPONENTS_G4SMEARGENPARTICLES_H

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "k4FWCore/DataHandle.h"
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimInterface/ISimParticleSmearTool.h"

// datamodel
namespace edm4hep {
class MCParticleCollection;
}

/** @class SimSmearGenParticles SimComponents/src/SimSmearGenParticles.h SimSmearGenParticles.h
 *
 *  Smear 'generated' (smeared) particles.
 *
 *  @author Coralie Neubüser
 */

class SimSmearGenParticles : public GaudiAlgorithm {
 public:
  SimSmearGenParticles(const std::string& aName, ISvcLocator* svcLoc);
  /**  Initialize.
   *   @return status code
   */
  StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  StatusCode finalize();
  /**  Save the data output.
   *   Saves the 'reconstructed' (smeared) particles and associates them with MC particles.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  StatusCode execute();

 private:
  /// Handle for the particles to be written                                                                                                                              
  DataHandle<edm4hep::MCParticleCollection> m_inParticles{"GenParticles", Gaudi::DataHandle::Reader, this};
  /// Handle for the particles to be written
  DataHandle<edm4hep::MCParticleCollection> m_particles{"SimParticlesSmeared", Gaudi::DataHandle::Writer, this};
  /// Handle for the calorimeter cells noise tool 
  ToolHandle<ISimParticleSmearTool> m_smearTool{"SimParticleSmearRootFile", this};
  /// Flag to decide on wether to only smear and write out charged particles
  Gaudi::Property<bool> m_simTracker{this, "simulateTracker", true};
};

#endif /* SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H */
