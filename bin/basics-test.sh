#!/bin/sh

################################################################################
#                                                                              #
# Run some Diva.Basics test dealing with XML. This is not NUnit-ed a cause de  #
# mono-bundled nunit not being compiled with generics/2.0 support.             #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/Diva.Basics.Test/basicstest.exe" ] ; then
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
$BASEDIR/../src/Diva.Basics/:\
$BASEDIR/../test/Diva.Basics.Test/:"

export MONO_PATH="$MONO_PATH:$ASSEMBLY_PATHS"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BASEDIR/../src/Gdv/:"

exec mono --debug "$BASEDIR/../test/Diva.Basics.Test/basicstest.exe" "$@"
