/**** get RVec<RVec<>> ****/
#define GET_RVEC_FUNC_TEMPLATE( FUNC, ITEM )                                                                           \
  template <typename HitType, edm4hep::utils::EnableIfEDM4hepVector3DType<decltype( HitType::ITEM )> = false>          \
  constexpr auto FUNC( const ROOT::RVec<HitType>& in ) {                                                               \
    using namespace edm4hep::utils;                                                                                    \
    using VecType = decltype( HitType::ITEM );                                                                         \
    using RetType = ValueType<VecType>;                                                                                \
    auto mapper   = []( const HitType& data ) {                                                                        \
      return ROOT::RVec<RetType>{ vector_x( data.ITEM ), vector_y( data.ITEM ), vector_z( data.ITEM ) };             \
    };                                                                                                                 \
    return ROOT::VecOps::Map( in, mapper );                                                                            \
  }
