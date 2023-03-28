/** MegatAnalyzer module: __pkgname__
 *
 * \file __name__.h
 *
 * Description:
 *   [...]
 */

#ifndef __pkgname_____name___h
#define __pkgname_____name___h

#include "ROOT/RVec.hxx"
#include "edm4hep/TrackerHitData.h"

namespace __pkgname__ {
  namespace rv = ROOT::VecOps;

  void            dummy_analysis();
  rv::RVec<float> dummy_collection( const rv::RVec<edm4hep::TrackerHitData>& );
} // namespace __pkgname__

#endif
