#ifndef MEGAT_AlgTemplate_H
#define MEGAT_AlgTemplate_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

/** @class AlgTemplate
 *
 * A template for Gaudi algorithm. Copy the source and header file and replace "AlgTemplate" with
 * real algorithm name.
 *
 *  @author Yong Zhou
 */

class ITHistSvc;
class TH1F;

// uncomment the edm collection you need
namespace edm4hep {
  class SimCalorimeterHitCollection;
  // class SimTrackerHitCollection;
  // class MCParticleCollection;
  // class ReconstructedParticleCollection;
  // class MCRecoParticleAssociationCollection;
}

class AlgTemplate : public GaudiAlgorithm {
public:
  explicit AlgTemplate(const std::string&, ISvcLocator*);
  virtual ~AlgTemplate();
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
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the random numbers service
  // SmartIF<IRndmGenSvc> m_randSvc;

  /// Handle to EDM collection
  DataHandle<edm4hep::SimCalorimeterHitCollection> m_caloHits{"CztHits", Gaudi::DataHandle::Reader, this};

  /// Properties
  // Gaudi::Property<float> m_sigma{this, "Sigma", 1.0, "Sigma of Gaussian distribution"};
  // Gaudi::Property<std::string> m_name{this, "Name", "hRndGauss", "Name of the histogram"};
};
#endif /* MEGAT_AlgTemplate_H */
