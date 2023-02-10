#ifndef MEGAT_TUTURIAL_RDMNUMBER_H
#define MEGAT_TUTURIAL_RDMNUMBER_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/ITHistSvc.h"

/** @class MgTut_RdmNumber
 *
 *  This algorithm demo shows how to use the Gaudi random service and histogram service.
 *  It uses a gaussian random generator to fill a histogram.
 *  The dist parameters and hist entries are configured using Gaudi property.
 *
 *  See tut_rndm.py for usage.
 *
 *  @author Yong Zhou
 */

class MgTut_RdmNumber : public GaudiAlgorithm {
public:
  explicit MgTut_RdmNumber(const std::string&, ISvcLocator*);
  virtual ~MgTut_RdmNumber();
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
  SmartIF<IRndmGenSvc> m_randSvc;

  /// random generator parameters
  Gaudi::Property<float> m_mean{this, "Mean", 0.0, "Mean of Gaussian distribution"};
  Gaudi::Property<float> m_sigma{this, "Sigma", 1.0, "Sigma of Gaussian distribution"};
  Gaudi::Property<std::string> m_name{this, "Name", "hRndGauss", "Name of the histogram"};
  Gaudi::Property<std::string> m_dir{this, "Directory", "/", "Directory of the histogram in TFile"};
};
#endif /* MEGAT_TUTURIAL_RDMNUMBER_H */
