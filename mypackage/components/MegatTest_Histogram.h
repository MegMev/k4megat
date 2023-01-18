#ifndef MEGAT_TEST_HISTOGRAM_H
#define MEGAT_TEST_HISTOGRAM_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"
class ITHistSvc;

// datamodel
namespace edm4hep {
class SimCalorimeterHitCollection;
}

class TH1F;

/** @class MegatTest_Histogram SimG4Components/src/MegatTest_Histogram.h MegatTest_Histogram.h
 *
 *  Fast simulation histograms algorithm.
 *  Fills validation histograms.
 *  It takes ParticleCollection (\b'smearedParticles') as the input.
 *
 *  @author Anna Zaborowska
 */

class MegatTest_Histogram : public GaudiAlgorithm {
public:
  explicit MegatTest_Histogram(const std::string&, ISvcLocator*);
  virtual ~MegatTest_Histogram();
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
  DataHandle<edm4hep::SimCalorimeterHitCollection> m_calorHits{"CztHits", Gaudi::DataHandle::Reader, this};
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  // Histogram of the smeared particle's momentum
  TH1F* m_edep{nullptr};
};
#endif /* MEGAT_TEST_HISTOGRAM_H */
