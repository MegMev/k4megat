################################################################################
# Package: SimSensitive
################################################################################

dd4hep_configure_output()
dd4hep_add_plugin(SimSensitive
                  SOURCES src/*.cpp
                  USES DD4hep::DDCore
                       DD4hep::DDG4
                       SimKernel
                  INCLUDES include
                  )
