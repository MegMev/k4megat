#ifndef MEGAT_Analysis_McParticle_H
#define MEGAT_Analysis_McParticle_H

#include "GeoUtils.h"
#include "ROOT/RVec.hxx"
#include "Utils.h"
#include "edm4hep/MCParticleData.h"

namespace megat {
  // explicit template instantiate of utilities is dispatched here (personal preference)
  namespace utility {
    INST_GET_FUNC( get_vertex_x, edm4hep::MCParticleData );
    INST_GET_FUNC( get_vertex_y, edm4hep::MCParticleData );
    INST_GET_FUNC( get_vertex_z, edm4hep::MCParticleData );

    INST_GET_FUNC( get_mom_x, edm4hep::MCParticleData );
    INST_GET_FUNC( get_mom_y, edm4hep::MCParticleData );
    INST_GET_FUNC( get_mom_z, edm4hep::MCParticleData );

    INST_GET_FUNC( get_charge, edm4hep::MCParticleData );
    INST_GET_FUNC( get_pdg, edm4hep::MCParticleData );
    INST_GET_FUNC( get_mass, edm4hep::MCParticleData );
  } // namespace utility

  //
  namespace MCParticle {
    template <typename T>
    using VecImp = ROOT::VecOps::RVec<T>;

    // some useful aliases to utility
    RENAME_GET_FUNC_NS( megat::utility, get_vertex_x, vertex_x, edm4hep::MCParticleData );
    RENAME_GET_FUNC_NS( megat::utility, get_vertex_y, vertex_y, edm4hep::MCParticleData );
    RENAME_GET_FUNC_NS( megat::utility, get_vertex_z, vertex_z, edm4hep::MCParticleData );

    RENAME_GET_FUNC_NS( megat::utility, get_mom_x, mom_x, edm4hep::MCParticleData );
    RENAME_GET_FUNC_NS( megat::utility, get_mom_y, mom_y, edm4hep::MCParticleData );
    RENAME_GET_FUNC_NS( megat::utility, get_mom_z, mom_z, edm4hep::MCParticleData );

    RENAME_GET_FUNC_NS( megat::utility, get_charge, charge, edm4hep::MCParticleData );
    RENAME_GET_FUNC_NS( megat::utility, get_pdg, pdg, edm4hep::MCParticleData );
    RENAME_GET_FUNC_NS( megat::utility, get_mass, mass, edm4hep::MCParticleData );
  } // namespace MCParticle
} // namespace megat

#endif
