#pragma once

#include "DD4hep/Factories.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include <cmath>

namespace {
  template <typename T>
  dd4hep::SegmentationObject* create_segmentation( const dd4hep::BitFieldCoder* decoder ) {
    return new dd4hep::SegmentationWrapper<T>( decoder );
  }

  // Compare the parity between two number, same or not
  inline bool sameParity( int x, int y ) { return ( x - y ) % 2 == 0 ? true : false; }
} // namespace
