#!/usr/bin/env bash

# Source this script to set up the megat running environment.
# Only the MegatSW related environments are configured.
# The running environment of dependency should be setup in other methods
# like key4hep-spack environment.
#
# - csh shell is not supported
#
# Code borrowed from thisdd4hep.sh of DD4hep library
#
# Author: Yong
# Original Author: Pere Mato. F. Gaede, M.Frank
#-------------------------------------------------------------------------------

### import helper functions
source $(dirname "$0")/thismegat_internal.sh

### set this variable to the install prefix of this script
#-----------------------------------------------------------------------------
SOURCE=${BASH_SOURCE[0]:-${(%):-%x}}
megat_parse_this ${SOURCE}

### megat specific
#----PATH---------------------------------------------------------------------
megat_add_path PATH       ${THISMEGAT}/@CMAKE_INSTALL_BINDIR@;
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    ${THISMEGAT}/@CMAKE_INSTALL_LIBDIR@;
#----PYTHONPATH---------------------------------------------------------------
megat_add_path PYTHONPATH ${THISMEGAT}/@GAUDI_INSTALL_PYTHONDIR@;
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH ${THISMEGAT}/@CMAKE_INSTALL_INCLUDEDIR@;
#----CMAKE_PREFIX_PATH--------------------------------------------------------
megat_add_path CMAKE_PREFIX_PATH ${THISMEGAT};

#----MEGAT_ROOT---------------------------------------------------------------
export MEGAT_ROOT=${THISMEGAT}
#----MEGAT_LIBRARY_PATH-------------------------------------------------------
if [ @APPLE@ ];
then
    export MEGAT_LIBRARY_PATH=${DYLD_LIBRARY_PATH};
else
    export MEGAT_LIBRARY_PATH=${LD_LIBRARY_PATH};
fi;

###
unset THISMEGAT;
unset SOURCE;
#-----------------------------------------------------------------------------
