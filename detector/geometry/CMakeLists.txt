# Geometry model based on dd4hep
#==========================================================================

dd4hep_configure_output()
file(GLOB sources src/*.cpp)
dd4hep_add_plugin(MegatDetector GENERATED ${sources}
  USES DD4hep::DDCore
)
dd4hep_configure_scripts(MegatDetector DEFAULT_SETUP WITH_TESTS)


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
