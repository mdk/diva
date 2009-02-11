#!/bin/sh

################################################################################
#                                                                              #
# A track player to test the GdvTrack.                                         #
# It relies on the media files repository.                                     #
# It creates a sample track with various media aligned one after another, and  #
# plays it back                                                                #
#                                                                              #
################################################################################

cd `dirname $0`

if [ ! -f "../test/TrackPlayer/trackplayer" ] ; then
    echo
    echo "The test has not been build. Please run:"
    echo 
    echo "scons test"
    echo 
    echo "...to build the tests."
    echo
    exit
fi

if [ ! -d "../media" ] ; then
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

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../src/Gdv:"
exec "../test/TrackPlayer/trackplayer"

