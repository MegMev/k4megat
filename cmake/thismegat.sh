#!/usr/bin/env bash

# Source this script to set up the full megat running environment.
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

### dd4hep: source thisdd4hep.sh, which in turn setup the following:
### - ROOT
### - Geant4
### - CLHEP
### - XCERCES (if used)
#-----------------------------------------------------------------------------
if [ -n "@DD4hep_DIR@" ]; then
   source @DD4hep_DIR@/bin/thisdd4hep.sh
fi

### podio
#----PATH---------------------------------------------------------------------
megat_add_path PATH       $(cd @podio_INCLUDE_DIR@/../bin > /dev/null;pwd);
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    @podio_LIBRARY_DIR@;
#----PYTHONPATH---------------------------------------------------------------
megat_add_path PYTHONPATH @podio_PYTHON_DIR@;
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH @podio_INCLUDE_DIR@;
#----CMAKE_PREFIX_PATH--------------------------------------------------------
megat_add_path CMAKE_PREFIX_PATH $(cd @podio_INCLUDE_DIR@/.. > /dev/null;pwd);
#-----------------------------------------------------------------------------

### edm4hep
#----PATH---------------------------------------------------------------------
megat_add_path PATH       $(cd @EDM4HEP_INCLUDE_DIR@/../bin > /dev/null;pwd);
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    $(cd @EDM4HEP_INCLUDE_DIR@/../lib > /dev/null;pwd);
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH @EDM4HEP_INCLUDE_DIR@;
#----CMAKE_PREFIX_PATH--------------------------------------------------------
megat_add_path CMAKE_PREFIX_PATH $(cd @EDM4HEP_INCLUDE_DIR@/.. > /dev/null;pwd);

### k4FWCore
#----PATH---------------------------------------------------------------------
megat_add_path PATH       $(cd @k4FWCore_DIR@/../../../bin > /dev/null;pwd);
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    $(cd @k4FWCore_DIR@/../../../lib > /dev/null;pwd);
#----PYTHONPATH---------------------------------------------------------------
megat_add_path PYTHONPATH $(cd @k4FWCore_DIR@/../../../python > /dev/null;pwd);
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH $(cd @k4FWCore_DIR@/../../../include > /dev/null;pwd);
#----CMAKE_PREFIX_PATH--------------------------------------------------------
megat_add_path CMAKE_PREFIX_PATH $(cd @k4FWCore_DIR@/../../.. > /dev/null;pwd);
#-----------------------------------------------------------------------------

### gaudi
#----PATH---------------------------------------------------------------------
megat_add_path PATH       @Gaudi_BINARY_DIR@;
#----LIBRARY_PATH-------------------------------------------------------------
megat_add_library_path    @Gaudi_LIBRARY_DIR@;
#----PYTHONPATH---------------------------------------------------------------
megat_add_path PYTHONPATH @Gaudi_PYTHON_DIR@;
#----ROOT_INCLUDE_PATH--------------------------------------------------------
megat_add_path ROOT_INCLUDE_PATH @Gaudi_INCLUDE_DIR@;
#----CMAKE_PREFIX_PATH--------------------------------------------------------
megat_add_path CMAKE_PREFIX_PATH $(cd @Gaudi_LIBRARY_DIR@/.. > /dev/null;pwd);
#-----------------------------------------------------------------------------

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
#----MEGAT_LIBRARY_PATH-------------------------------------------------------
if [ @APPLE@ ];
then
    export MEGAT_LIBRARY_PATH=${DYLD_LIBRARY_PATH};
else
    export MEGAT_LIBRARY_PATH=${LD_LIBRARY_PATH};
fi;
#----MEGAT_ROOT---------------------------------------------------------------
export MEGAT_ROOT=${THISMEGAT}

### clean up
unset THISMEGAT;
unset SOURCE;
#-----------------------------------------------------------------------------
