#!/bin/sh

################################################################################
#                                                                              #
# A seek player to test seek-in-block method. Use with a file parameter (media)#
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

if [ ! -f "$BASEDIR/../test/SeekPlayer/seekplayer" ] ; then
    echo
    echo "The test has not been build. Please run:"
    echo 
    echo "scons test"
    echo 
    echo "...to build the tests."
    echo
    exit
fi

exec "$BASEDIR/../test/SeekPlayer/seekplayer" "$@"

