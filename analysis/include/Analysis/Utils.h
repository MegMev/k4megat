#ifndef MEGAT_UTILITY_H
#define MEGAT_UTILITY_H

#include "utility.hxx"
#include <ROOT/RVec.hxx>

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

    // common accessors
    GET_UINT64_ITEM_FUNC_TEMPLATE( get_cellid, cellID )

    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_x, position.x )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_y, position.y )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_z, position.z )

    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_mom_x, momentum.x )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_mom_y, momentum.y )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_mom_z, momentum.z )

    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_time, time )

    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_EDep, EDep )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_eDep, eDep )
    GET_FLOAT_ITEM_FUNC_TEMPLATE( get_energy, energy )
  }; // namespace utility
} // namespace megat
#endif
