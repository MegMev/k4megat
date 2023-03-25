#ifndef MEGAT_Analysis_SimCalo_H
#define MEGAT_Analysis_SimCalo_H

#include "GeoUtils.h"
#include "ROOT/RVec.hxx"
#include "Utils.h"
#include "edm4hep/SimCalorimeterHitData.h"

namespace megat {
  // explicit template instantiate of utilities is dispatched here (personal preference)
  namespace utility {
    INST_GET_FUNC( get_x, edm4hep::SimCalorimeterHitData );
    INST_GET_FUNC( get_y, edm4hep::SimCalorimeterHitData );
    INST_GET_FUNC( get_z, edm4hep::SimCalorimeterHitData );

    INST_GET_FUNC( get_energy, edm4hep::SimCalorimeterHitData );
    INST_GET_FUNC( get_cellid, edm4hep::SimCalorimeterHitData );
  } // namespace utility

  //
  namespace SimCalo {
    template <typename T>
    using VecImp = ROOT::VecOps::RVec<T>;

    // some useful aliases to utility
    RENAME_GET_FUNC_NS( megat::utility, get_x, hit_x, edm4hep::SimCalorimeterHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_y, hit_y, edm4hep::SimCalorimeterHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_z, hit_z, edm4hep::SimCalorimeterHitData );

    RENAME_GET_FUNC_NS( megat::utility, get_energy, hit_e, edm4hep::SimCalorimeterHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_cellid, hit_id, edm4hep::SimCalorimeterHitData );

    // functors
    RENAME_FUNCTOR_NS( megat::utility, CellPosition, cell_pos, edm4hep::SimCalorimeterHitData );

    // calo hits (single cells)
    // ROOT::VecOps::RVec<int>      getHit_layer( const ROOT::VecOps::RVec<edm4hep::SimCalorimeterHitData>& in );
    // ROOT::VecOps::RVec<float>    getHit_energy( const ROOT::VecOps::RVec<edm4hep::SimCalorimeterHitData>& in );
    // );
  } // namespace SimCalo
} // namespace megat

#endif
