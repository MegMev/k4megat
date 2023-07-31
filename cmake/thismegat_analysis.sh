#!/usr/bin/env bash
# Source this script to set up the megat installation that this script is part of.
#
# - csh shell is not supported
#
# Code borrowed from thisdd4hep.sh of DD4hep library
#
# Author: Yong
# Original Author: Pere Mato. F. Gaede, M.Frank
#-------------------------------------------------------------------------------

### import helper functions
source $(dirname "$0")/thismagat_internal.sh

### set THIS variable to the install prefix of this script
#-----------------------------------------------------------------------------
SOURCE=${BASH_SOURCE[0]:-${(%):-%x}}
megat_parse_this ${SOURCE} test

### dd4hep: source thisdd4hep.sh, which in turn setup the following:
### - ROOT
### - Geant4
### - XCERCES (optional)
#-----------------------------------------------------------------------------


# edm4hep & podio

# gaudi
#----PATH---------------------------------------------------------------------
megat_add_path PATH       @Gaudi_BINARY_DIR@;
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    @Gaudi_LIBRARY_DIR@;
#----PYTHONPATH---------------------------------------------------------------
megat_add_path PYTHONPATH @Gaudi_PYTHON_DIR@;
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH @Gaudi_INCLUDE_DIR@;
#-----------------------------------------------------------------------------

### megat specific
#----PATH---------------------------------------------------------------------
megat_add_path PATH       ${THIS}/@CMAKE_INSTALL_BINDIR@;
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    ${THIS}/@CMAKE_INSTALL_LIBDIR@;
#----PYTHONPATH---------------------------------------------------------------
megat_add_path PYTHONPATH ${THIS}/@GAUDI_INSTALL_PYTHONDIR@;
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH ${THIS}/@CMAKE_INSTALL_INCLUDEDIR@;
#-----------------------------------------------------------------------------
if [ @APPLE@ ];
then
    export MEGAT_LIBRARY_PATH=${DYLD_LIBRARY_PATH};
else
    export MEGAT_LIBRARY_PATH=${LD_LIBRARY_PATH};
fi;
#-----------------------------------------------------------------------------
#
unset THIS;
unset SOURCE;
#-----------------------------------------------------------------------------
