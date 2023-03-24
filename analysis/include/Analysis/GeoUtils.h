#ifndef MEGAT_GeoUitls_H
#define MEGAT_GeoUitls_H

#include "DD4hep/Detector.h"
#include <string>
#include <vector>

namespace megat {
  namespace utility {
    // geometry loader
    dd4hep::Detector& loadGeometry( std::vector<std::string> xmlList, std::string tpc_readout_name = "",
                                    std::string tag = "", std::string tpc_name = "TPC" );

  } // namespace utility
} // namespace megat
#endif
