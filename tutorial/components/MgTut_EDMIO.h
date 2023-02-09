#ifndef MEGAT_MgTut_EDMIO_H
#define MEGAT_MgTut_EDMIO_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

/** @class MgTut_EDMIO
 *
 * A template for Gaudi algorithm. Copy the source and header file and replace "MgTut_EDMIO" with
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

class MgTut_EDMIO : public GaudiAlgorithm {
public:
  explicit MgTut_EDMIO(const std::string&, ISvcLocator*);
  virtual ~MgTut_EDMIO();
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
#endif /* MEGAT_MgTut_EDMIO_H */
