#!/bin/sh

################################################################################
#                                                                              #
# Unit tests for Diva.Util                                                     #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/Diva.Util.NUnit/divautilnunit.dll" ] ; then
    echo 
    echo "The test has not been build. Please run:"
    echo 
    echo "scons test"
    echo 
    echo "...to build the tests."
    echo
    exit
fi

export MONO_PATH="$MONO_PATH:../../src/Gdv/:../../src/Diva.Util/:"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../src/Gdv/:"

exec nunit-console "$BASEDIR/../test/Diva.Util.NUnit/divautilnunit.dll"
