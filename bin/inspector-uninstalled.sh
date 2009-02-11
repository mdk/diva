#!/bin/sh

################################################################################
#                                                                              #
# Use this script to run uninstalled diva-inspector. Inspector can be used to  #
# get some info about the media files. And to test compatibility with Gdv/Gst  #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

ASSEMBLY_PATHS="$BASEDIR/../src/Gdv/:$BASEDIR/../src/Diva.Util/:"

export MONO_PATH="$MONO_PATH:$ASSEMBLY_PATHS"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BASEDIR/../src/Gdv/:"
export DIVA_LOCALE="$BASEDIR/../po/"

exec mono --debug "$BASEDIR/../src/Diva.Inspector/inspector.exe" "$@"
