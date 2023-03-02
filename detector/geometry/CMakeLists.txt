# Geometry model based on dd4hep
#==========================================================================

dd4hep_configure_output()
file(GLOB sources src/*.cpp)
dd4hep_add_plugin(DetGeometry GENERATED ${sources}
  USES DD4hep::DDCore
       DD4hep::DDRec
)
# dd4hep_configure_scripts(DetGeometry DEFAULT_SETUP WITH_TESTS)


# copy the geometry files
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/compact
  DESTINATION geometry
  COMPONENT install
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/cad_model
  DESTINATION geometry
  COMPONENT install
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/material
  DESTINATION geometry
  COMPONENT install
)
