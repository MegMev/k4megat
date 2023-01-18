#==========================================================================
dd4hep_configure_output()
file(GLOB sources src/*.cpp)
dd4hep_add_plugin(MegatDetector GENERATED ${sources}
                                USES DD4hep::DDCore
                              )
dd4hep_configure_scripts(MegatDetector DEFAULT_SETUP WITH_TESTS)
