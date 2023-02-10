#ifndef MEGAT_MgTut_EDMIO_H
#define MEGAT_MgTut_EDMIO_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

/** @class MgTut_EDMIO
 *
 * This algorithm demo shows how to fetch a existing collection object from TES, add an custom offset
 * to the energy value, create and write to a new collection object in TES.
 *
 * See tut_edmio.py for usage.
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

  /// Handle to EDM collection
  DataHandle<edm4hep::SimCalorimeterHitCollection> m_caloHits{"CztHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::SimCalorimeterHitCollection> m_offsetCaloHits{"OffsetCztHits", Gaudi::DataHandle::Writer, this};

  /// Properties
  Gaudi::Property<float> m_offset{this, "Offset", 5.0, "An arbitrary offest added to energy value"};
};
#endif /* MEGAT_MgTut_EDMIO_H */
