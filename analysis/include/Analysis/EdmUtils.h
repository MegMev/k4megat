#pragma once

#include "ROOT/RVec.hxx"
#include "Utils.h"
#include "edm4hep/Vector3d.h"

namespace megat {
  namespace utility {
    INST_GET_FUNC( get_x, edm4hep::Vector3d );
    INST_GET_FUNC( get_y, edm4hep::Vector3d );
    INST_GET_FUNC( get_z, edm4hep::Vector3d );
  } // namespace utility
} // namespace megat
