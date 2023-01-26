#ifndef SIMG4FAST_G4FASTSIMHISTOGRAMS_H
#define SIMG4FAST_G4FASTSIMHISTOGRAMS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "k4FWCore/DataHandle.h"
class ITHistSvc;

// datamodel
namespace edm4hep {
class ReconstructedParticleCollection;
class MCRecoParticleAssociationCollection;
}

class TH1F;

/** @class SimFastSimHistograms SimComponents/src/SimFastSimHistograms.h SimFastSimHistograms.h
 *
 *  Fast simulation histograms algorithm.
 *  Fills validation histograms.
 *  It takes ParticleCollection (\b'smearedParticles') as the input.
 *
 *  @author Anna Zaborowska
 */

class SimFastSimHistograms : public GaudiAlgorithm {
public:
  explicit SimFastSimHistograms(const std::string&, ISvcLocator*);
  virtual ~SimFastSimHistograms();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Handle for the EDM particles and MC particles associations to be read
  DataHandle<edm4hep::MCRecoParticleAssociationCollection> m_particlesMCparticles{"ParticlesMCparticles",
                                                                                  Gaudi::DataHandle::Reader, this};
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  // Histogram of the smeared particle's momentum
  TH1F* m_p{nullptr};
  // Histogram of the smeared particle's pseudorapidity
  TH1F* m_eta{nullptr};
  // Histogram of the difference between MC particle's and smeared particle's momentum
  TH1F* m_diffP{nullptr};
  // Histogram of the smeared particle's PDG code
  TH1F* m_pdg{nullptr};
};
#endif /* SIMG4FAST_G4FASTSIMHISTOGRAMS_H */
