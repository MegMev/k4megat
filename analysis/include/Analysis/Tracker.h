#ifndef MEGAT_Analysis_Tracker_H
#define MEGAT_Analysis_Tracker_H

#include "ROOT/RVec.hxx"
#include "Utils.h"
#include "edm4hep/TrackerHitData.h"

namespace megat {
  // explicit template instantiate of utilities is dispatched here (personal preference)
  namespace utility {
    // single value
    INST_GET_FUNC( get_x, edm4hep::TrackerHitData );
    INST_GET_FUNC( get_y, edm4hep::TrackerHitData );
    INST_GET_FUNC( get_z, edm4hep::TrackerHitData );

    INST_GET_FUNC( get_eDep, edm4hep::TrackerHitData );
    INST_GET_FUNC( get_time, edm4hep::TrackerHitData );

    INST_GET_FUNC( get_cellid, edm4hep::TrackerHitData );

    // vector value
    INST_GET_FUNC( get_pos, edm4hep::TrackerHitData );

  } // namespace utility

  //
  namespace Tracker {
    template <typename T>
    using VecImp = ROOT::VecOps::RVec<T>;

    // some useful aliases to utility
    RENAME_GET_FUNC_NS( megat::utility, get_x, hit_x, edm4hep::TrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_y, hit_y, edm4hep::TrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_z, hit_z, edm4hep::TrackerHitData );

    RENAME_GET_FUNC_NS( megat::utility, get_eDep, hit_e, edm4hep::TrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_time, hit_t, edm4hep::TrackerHitData );
    RENAME_GET_FUNC_NS( megat::utility, get_cellid, hit_id, edm4hep::TrackerHitData );

    //
    RENAME_GET_FUNC_NS( megat::utility, get_pos, hit_pos, edm4hep::TrackerHitData );

    // functors

  } // namespace Tracker
} // namespace megat

#endif
