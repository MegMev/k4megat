#pragma once

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

//
#include "SimKernel/Units.h"
#include <cmath>

/** @class TpcSimpleSmearAlg
 *
 *  Energy and timestamp smearing with Gaussian:
 *  - fixed sigma is used for timestamp (param: m_tsSigma)
 *  - two options for energy
 *    1. fixed sigma (param: m_eSigma)
 *    2. fix the fano factor and variant energy sigma based on edep (param: m_fano)
 *
 *  Input: TrackerHit from TpcSegmentAlg
 *
 *  Output: Clone of the input except smeared energy or timestamp
 *
 *  @author Yong Zhou
 */

class IGeoSvc;

namespace edm4hep {
  class TrackerHitCollection;
} // namespace edm4hep

class TpcSimpleSmearAlg : public GaudiAlgorithm {
public:
  explicit TpcSimpleSmearAlg( const std::string&, ISvcLocator* );
  virtual ~TpcSimpleSmearAlg();
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
  /// Random number generators
  Rndm::Numbers m_sigmaGenT;
  Rndm::Numbers m_sigmaGenE;

  /// Handle to EDM collection
  DataHandle<edm4hep::TrackerHitCollection> m_inHits{ "TpcSegHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::TrackerHitCollection> m_outHits{ "TpcSmearHits", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<bool>   use_Fano{ this, "use_fano", true, "Use a fixed Fano factor to deduce energy resolution." };
  Gaudi::Property<double> m_fano{ this, "fano_factor", 0.2, "Fano factor for energy measurement (aka. sigma^2/mean)" };
  Gaudi::Property<double> m_sigmaE{ this, "energy_sigma", 50,
                                    "[eV] Sigma of the energy distribution of a single channel" };
  Gaudi::Property<double> m_sigmaT{ this, "time_sigma", 100,
                                    "[ps] Sigma of the time distribution of a single channel" };
};
