/** MegatAnalyzer module: __pkgname__
 *
 * \file __name__.cc
 */

#include "__name__.h"
#include <fmt/core.h>

namespace __pkgname__ {
  void dummy_analysis() { fmt::print( "Dummy analysis initialised." ); }

  rv::RVec<float> dummy_collection( const rv::RVec<edm4hep::TrackerHitData>& hits ) {
    rv::RVec<float> output;
    for ( size_t i = 0; i < hits.size(); ++i ) output.emplace_back( hits.at( i ).position.x );
    return output;
  }
} // namespace __pkgname__
