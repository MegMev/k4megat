#ifndef MEGAT_Analysis_SimTracker_H
#define MEGAT_Analysis_SimTracker_H

#include "GeoUtils.h"
#include "ROOT/RVec.hxx"
#include "Utils.h"
#include "edm4hep/SimTrackerHitData.h"

namespace megat {
  // explicit template instantiate of utilities is dispatched here (personal preference)
  namespace utility {
    // single value
    INST_GET_FUNC( get_x, edm4hep::SimTrackerHitData );
    INST_GET_FUNC( get_y, edm4hep::SimTrackerHitData );
    INST_GET_FUNC( get_z, edm4hep::SimTrackerHitData );

    INST_GET_FUNC( get_EDep, edm4hep::SimTrackerHitData );
    INST_GET_FUNC( get_time, edm4hep::SimTrackerHitData );

    INST_GET_FUNC( get_cellid, edm4hep::SimTrackerHitData );

    // vector value
    INST_GET_FUNC( get_pos, edm4hep::SimTrackerHitData );
    INST_GET_FUNC( get_mom, edm4hep::SimTrackerHitData );

  } // namespace utility

  //
  namespace SimTracker {
    template <typename T>
    using VecImp = ROOT::VecOps::RVec<T>;

    // some useful aliases to utility
    RENAME_GET_FUNC_NS( megat::utility, get_x, hit_x, edm4hep::SimTrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_y, hit_y, edm4hep::SimTrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_z, hit_z, edm4hep::SimTrackerHitData );

    RENAME_GET_FUNC_NS( megat::utility, get_EDep, hit_e, edm4hep::SimTrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_time, hit_t, edm4hep::SimTrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_cellid, hit_id, edm4hep::SimTrackerHitData );

    //
    RENAME_GET_FUNC_NS( megat::utility, get_pos, hit_pos, edm4hep::SimTrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_mom, hit_mom, edm4hep::SimTrackerHitData );

    // functors
    RENAME_FUNCTOR_NS( megat::utility, CellPosition, cell_pos, edm4hep::SimTrackerHitData );

  } // namespace SimTracker
} // namespace megat

#endif
