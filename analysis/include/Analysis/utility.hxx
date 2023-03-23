#define GET_ITEM_FUNC_TEMPLATE( RETTYPE, FUNC, ITEM )                                                                  \
  template <typename T>                                                                                                \
  inline ROOT::RVec<RETTYPE> FUNC( const ROOT::RVec<T>& in ) {                                                         \
    auto _x = []( const T& data ) { return data.ITEM; };                                                               \
    return ROOT::VecOps::Map( in, _x );                                                                                \
  }
#define INST_GET_ITEM_FUNC( RETTYPE, FUNC, DATATYPE ) template ROOT::RVec<RETTYPE> FUNC( const ROOT::RVec<DATATYPE>& )
#define INST_GET_ITEM_FUNC_NS( RETTYPE, NS, FUNC, DATATYPE )                                                           \
  template ROOT::RVec<RETTYPE> NS::FUNC( const ROOT::RVec<DATATYPE>& )
#define RENAME_INST_FUNC( OLD, NEW, DATATYPE ) constexpr auto NEW = OLD<DATATYPE>
#define RENAME_INST_FUNC_NS( NS, OLD, NEW, DATATYPE ) constexpr auto NEW = NS::OLD<DATATYPE>

//
#define GET_FLOAT_ITEM_FUNC_TEMPLATE( FUNC, ITEM ) GET_ITEM_FUNC_TEMPLATE( float, FUNC, ITEM )
#define GET_INT32_ITEM_FUNC_TEMPLATE( FUNC, ITEM ) GET_ITEM_FUNC_TEMPLATE( int32_t, FUNC, ITEM )
#define GET_UINT64_ITEM_FUNC_TEMPLATE( FUNC, ITEM ) GET_ITEM_FUNC_TEMPLATE( uint64_t, FUNC, ITEM )

#define INST_GET_FLOAT_ITEM_FUNC( FUNC, DATATYPE ) INST_GET_ITEM_FUNC( float, FUNC, DATATYPE )
#define INST_GET_INT32_ITEM_FUNC( FUNC, DATATYPE ) INST_GET_ITEM_FUNC( int32_t, FUNC, DATATYPE )
#define INST_GET_UINT64_ITEM_FUNC( FUNC, DATATYPE ) INST_GET_ITEM_FUNC( uint64_t, FUNC, DATATYPE )

//
#define INST_GET_FLOAT_ITEM_FUNC_NS( NS, FUNC, DATATYPE ) INST_GET_ITEM_FUNC_NS( float, NS, FUNC, DATATYPE )
#define INST_GET_INT32_ITEM_FUNC_NS( NS, FUNC, DATATYPE ) INST_GET_ITEM_FUNC_NS( int32_t, NS, FUNC, DATATYPE )
#define INST_GET_UINT64_ITEM_FUNC_NS( NS, FUNC, DATATYPE ) INST_GET_ITEM_FUNC_NS( uint64_t, NS, FUNC, DATATYPE )
