#!/bin/sh

################################################################################
#                                                                              #
# A script to run the nunit tests. A special repo needs to be fetched to get   #
# the media test files                                                         #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/Gdv.NUnit/gdvnunit.dll" ] ; then
    echo 
    echo "The test has not been build. Please run:"
    echo 
    echo "scons test"
    echo 
    echo "...to build the tests."
    echo
    exit
fi

if [ ! -d "$BASEDIR/../media" ] ; then
    echo
    echo "You don't appear to have the required media test files. Please run:"
    echo 
    echo "    svn co http://svn.diva-project.org/media"
    echo 
    echo "...in the root diva source directory. Please note that fetching this"
    echo "directory will take some time!"
    echo
    exit
fi

export MONO_PATH="$MONO_PATH:$BASEDIR/../src/Gdv/:"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../src/Gdv/:"

exec nunit-console "$BASEDIR/../test/Gdv.NUnit/gdvnunit.dll"
