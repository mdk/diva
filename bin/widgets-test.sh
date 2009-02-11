#!/bin/sh

################################################################################
#                                                                              #
# A script to run a Diva.Widgets test application.                             #
# Primarly meant to test the cell renderers                                    #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/Diva.Widgets.Test/widgetstest.exe" ] ; then
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
$BASEDIR/../src/Diva.Widgets/:\
$BASEDIR/../test/Diva.Widgets.Test/:"

export MONO_PATH="$MONO_PATH:$ASSEMBLY_PATHS"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:\
$BASEDIR/../src/Gdv/:\
$BASEDIR/../src/Diva.Cairo:\
$BASEDIR/../src/Diva.Widgets:"

exec mono --debug "$BASEDIR/../test/Diva.Widgets.Test/widgetstest.exe" "$@"
