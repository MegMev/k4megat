#ifndef MEGAT_UTILITY_H
#define MEGAT_UTILITY_H

#include "utility.hxx"

namespace megat {
  namespace utility {
    using ROOT::RVec;

    template <typename T>
    inline auto getsize( T& vec ) {
      return vec.size();
    };
    template <typename T>
    inline RVec<RVec<T>> as_vector( const RVec<T>& in ) {
      return RVec<RVec<T>>( 1, in );
    };

    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_x, position.x )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_y, position.y )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_z, position.z )
  }; // namespace utility
} // namespace megat
#endif
