#!/bin/sh

################################################################################
#                                                                              #
# A script to run a Diva.Cairo test application.                               #
# Displays some nice cairo-drawn stuff we're using in the main app too.        #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/Diva.Cairo.Test/cairotest.exe" ] ; then
    echo 
    echo "The test has not been build. Please run:"
    echo 
    echo "scons test"
    echo 
    echo "...to build the tests."
    echo
    exit
fi

ASSEMBLY_PATHS="\
$BASEDIR/../src/Gdv/:\
$BASEDIR/../src/Diva.Cairo/:\
$BASEDIR/../test/Diva.Cairo.Test/:"

export MONO_PATH="$MONO_PATH:$ASSEMBLY_PATHS"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BASEDIR/../src/Gdv/:$BASEDIR/../src/Diva.Cairo:"

exec mono --debug "$BASEDIR/../test/Diva.Cairo.Test/cairotest.exe" "$@"
