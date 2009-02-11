#!/bin/sh

################################################################################
#                                                                              #
# Use this script to run Diva uninstalled. You can lunch it from the build     #
# directory directly.                                                          #
#                                                                              #
################################################################################

BASEDIR=`dirname $0`

ASSEMBLY_PATHS="\
$BASEDIR/../src/Gdv/:\
$BASEDIR/../src/Diva.Exe/:\
$BASEDIR/../src/Diva.MainMenu/:\
$BASEDIR/../src/Diva.Cairo/:\
$BASEDIR/../src/Diva.Editor.Gui/:\
$BASEDIR/../src/Diva.Commands/:\
$BASEDIR/../src/Diva.Core/:\
$BASEDIR/../src/Diva.Basics/:\
$BASEDIR/../src/Gdv.Boilers/:\
$BASEDIR/../src/Diva.PluginLib/:\
$BASEDIR/../src/Diva.Config/:\
$BASEDIR/../src/Diva.Timeline/:\
$BASEDIR/../src/Diva.Widgets/:\
$BASEDIR/../src/Diva.Util/:\
$BASEDIR/../src/Diva.Editor.Model/:\
$BASEDIR/../src/Diva.Editor.Timeline/:"

export MONO_PATH="$MONO_PATH:$ASSEMBLY_PATHS"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:\
$BASEDIR/../src/Gdv/:\
$BASEDIR/../src/Diva.Widgets:\
$BASEDIR/../src/Diva.Cairo:\
$BASEDIR/../src/Diva.Plugins/:"
export DIVA_LOCALE="$BASEDIR/../po/"
export DIVA_PLUGINS="$BASEDIR/../src/Diva.Plugins/"

exec mono --debug "$BASEDIR/../src/Diva.Exe/diva.exe" "$@"
