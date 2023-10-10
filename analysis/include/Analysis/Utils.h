#ifndef MEGAT_UTILITY_H
#define MEGAT_UTILITY_H

#include "utility.hxx"
#include <ROOT/RVec.hxx>
#include <string>
#include <tuple>

namespace megat {
  namespace utility {
    // [to be clarified] cellid is long long int in dd4hep, but uint64 in edm4hep
    typedef std::uint64_t CellId;
    using ROOT::RVec;

    template <typename T>
    inline auto getsize( T& vec ) {
      return vec.size();
    };
    template <typename T>
    inline RVec<RVec<T>> as_vector( const RVec<T>& in ) {
      return RVec<RVec<T>>( 1, in );
    };

    template <typename T>
    class VecAt {
    public:
      VecAt( size_t pos ) : m_pos( pos ) {}

      auto operator()( const RVec<RVec<T>>& in ) {
        auto mapper = [this]( const RVec<T>& hit ) { return hit.at( m_pos ); };
        return Map( in, mapper );
      }

    private:
      size_t m_pos;
    };

    // common accessors
    GET_ITEM_FUNC_TEMPLATE( get_cellid, cellID )

    GET_ITEM_FUNC_TEMPLATE( get_x, position.x )
    GET_ITEM_FUNC_TEMPLATE( get_y, position.y )
    GET_ITEM_FUNC_TEMPLATE( get_z, position.z )

    GET_ITEM_FUNC_TEMPLATE( get_mom_x, momentum.x )
    GET_ITEM_FUNC_TEMPLATE( get_mom_y, momentum.y )
    GET_ITEM_FUNC_TEMPLATE( get_mom_z, momentum.z )

    GET_ITEM_FUNC_TEMPLATE( get_time, time )

    GET_ITEM_FUNC_TEMPLATE( get_EDep, EDep )
    GET_ITEM_FUNC_TEMPLATE( get_eDep, eDep )
    GET_ITEM_FUNC_TEMPLATE( get_energy, energy )

    GET_ITEM_FUNC_TEMPLATE( get_pdg, PDG )
    GET_ITEM_FUNC_TEMPLATE( get_charge, charge )
    GET_ITEM_FUNC_TEMPLATE( get_mass, mass )
    GET_ITEM_FUNC_TEMPLATE( get_vertex_x, vertex.x )
    GET_ITEM_FUNC_TEMPLATE( get_vertex_y, vertex.y )
    GET_ITEM_FUNC_TEMPLATE( get_vertex_z, vertex.z )

    // vector accessors
    GET_RVEC_FUNC_TEMPLATE( get_pos, position )
    GET_RVEC_FUNC_TEMPLATE( get_mom, momentum )

    GET_RVEC_FUNC_TEMPLATE( get_vertex, vertex )

  }; // namespace utility
} // namespace megat
#endif
