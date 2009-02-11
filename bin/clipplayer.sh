#!/bin/sh

################################################################################
#                                                                              #
# A clip player to test seek-in-block method. Use with a file parameter (media)#
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/ClipPlayer/clipplayer" ] ; then
    echo
    echo "The test has not been build. Please run:"
    echo 
    echo "scons test"
    echo 
    echo "...to build the tests."
    echo
    exit
fi

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BASEDIR/../src/Gdv:"
exec "$BASEDIR/../test/ClipPlayer/clipplayer" "$@"

