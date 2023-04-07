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

### Get install directory based on the directory name of this script
### - presumed directory tree: $INSTALL_PREFIX/bin/thismegat.sh
### - single arg: the directory of this script
#-----------------------------------------------------------------------------
megat_parse_this()   {
    local SOURCE=${1}
    # get param to "."
    local thisroot=$(dirname ${SOURCE})
    THISMEGAT=$(cd ${thisroot}/.. > /dev/null;pwd); export THISMEGAT
}

### Prepend a new path (arg2) into environment variable (arg1)
#-----------------------------------------------------------------------------
megat_add_path()   {
    local path_name=${1}
    local path_prefix=${2}
    eval path_value=\$$path_name
    # Prevent duplicates
    path_value=`echo ${path_value} | tr : '\n' | grep -v "${path_prefix}" | tr '\n' : | sed 's|:$||'`
    path_value="${path_prefix}${path_value:+:${path_value}}"
    eval export ${path_name}='${path_value}'
    unset path_value
}

### Helper to prepend a new directory to run time library path
### - use megat_add_path internally
#-----------------------------------------------------------------------------
megat_add_library_path()    {
    local p=${1};
    if [ @APPLE@ ]; then
        # Do not prepend system library locations on macOS. Stuff will break.
        [[ "$p" = "/usr/lib" || "$p" = "/usr/local/lib" ]] && return
        megat_add_path DYLD_LIBRARY_PATH     "$p"
        megat_add_path DD4HEP_LIBRARY_PATH   "$p"
    else
        megat_add_path LD_LIBRARY_PATH       "$p"
    fi
}
#-----------------------------------------------------------------------------

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
#-----------------------------------------------------------------------------
if [ @APPLE@ ];
then
    export MEGAT_LIBRARY_PATH=${DYLD_LIBRARY_PATH};
else
    export MEGAT_LIBRARY_PATH=${LD_LIBRARY_PATH};
fi;
#-----------------------------------------------------------------------------
export MEGAT_ROOT=${THISMEGAT}
#
unset THISMEGAT;
unset SOURCE;
#-----------------------------------------------------------------------------
