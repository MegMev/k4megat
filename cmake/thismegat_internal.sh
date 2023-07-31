#!/bin/bash

### Get install directory based on the directory name of this script
### - presumed directory tree: $INSTALL_PREFIX/bin/thismegat.sh
### - single arg: the directory of this script
### - side-effect: THISMEGAT variable is exported
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
