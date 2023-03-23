#include <ROOT/RVec.hxx>

#define GET_FLOAT_ITEM_FUNC_TEMPLATE( FUNC, ITEM )                                                                     \
  template <typename T>                                                                                                \
  inline ROOT::RVec<float> FUNC( const ROOT::RVec<T>& in ) {                                                           \
    auto _x = []( const T& data ) { return data.ITEM; };                                                               \
    return ROOT::VecOps::Map( in, _x );                                                                                \
  }

#define INST_GET_FLOAT_ITEM_FUNC( FUNC, DATATYPE ) template ROOT::RVec<float> FUNC( const ROOT::RVec<DATATYPE>& )
#define RENAME_INST_FUNC( OLD, NEW, DATATYPE ) constexpr auto NEW = OLD<DATATYPE>

//
#define INST_GET_FLOAT_ITEM_FUNC_NS( NS, FUNC, DATATYPE )                                                              \
  template ROOT::RVec<float> NS::FUNC( const ROOT::RVec<DATATYPE>& )
#define RENAME_INST_FUNC_NS( NS, OLD, NEW, DATATYPE ) constexpr auto NEW = NS::OLD<DATATYPE>
