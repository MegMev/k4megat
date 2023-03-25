#ifndef MEGAT_analysis_utility_H
#define MEGAT_analysis_utility_H

#include "edm4hep/utils/vector_utils.h"

/**** get RVec<> ****/
#define GET_ITEM_FUNC_TEMPLATE( FUNC, ITEM )                                                                           \
  template <typename HitType>                                                                                          \
  constexpr auto FUNC( const ROOT::RVec<HitType>& in ) {                                                               \
    auto mapper = []( const HitType& data ) { return data.ITEM; };                                                     \
    return ROOT::VecOps::Map( in, mapper );                                                                            \
  }

#if !__cpp_concepts
#  include "utility_legacy.hxx"
#else // use C++20 concept

/**** get RVec<RVec<>> ****/
#  define GET_RVEC_FUNC_TEMPLATE( FUNC, ITEM )                                                                         \
    template <typename HitType>                                                                                        \
    requires edm4hep::Vector3D<decltype( HitType::ITEM )>                                                              \
    constexpr auto FUNC( const ROOT::RVec<HitType>& in ) {                                                             \
      using namespace edm4hep::utils;                                                                                  \
      using VecType = decltype( HitType::ITEM );                                                                       \
      using RetType = utils::ValueType<VecType>;                                                                       \
      auto mapper   = []( const HitType& data ) {                                                                      \
        ROOT::RVec<RetType>{ vector_x( data.ITEM ), vector_y( data.ITEM ), vector_z( data.ITEM ) };                  \
      };                                                                                                               \
      return ROOT::VecOps::Map( in, mapper );                                                                          \
    }
#endif

#define INST_GET_FUNC( FUNC, DATATYPE ) template auto FUNC( const ROOT::RVec<DATATYPE>& )
#define INST_GET_FUNC_NS( NS, FUNC, DATATYPE ) template auto NS::FUNC( const ROOT::RVec<DATATYPE>& )
#define INST_FUNCTOR( CLASS, DATATYPE ) template class CLASS<DATATYPE>
#define INST_FUNCTOR_NS( NS, CLASS, DATATYPE ) template class NS::CLASS<DATATYPE>

#define RENAME_GET_FUNC( OLD, NEW, DATATYPE ) constexpr auto NEW = OLD<DATATYPE>
#define RENAME_GET_FUNC_NS( NS, OLD, NEW, DATATYPE ) constexpr auto NEW = NS::OLD<DATATYPE>

#define RENAME_FUNCTOR( OLD, NEW, DATATYPE ) using NEW = OLD<DATATYPE>
#define RENAME_FUNCTOR_NS( NS, OLD, NEW, DATATYPE ) using NEW = NS::OLD<DATATYPE>

#endif
