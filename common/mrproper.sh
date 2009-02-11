#!/bin/bash

################################################################################
#                                                                              #
# Purifies the build directory. Use with care. Use after scons -c to get rid   #
# of some remaining db files.                                                  #
#                                                                              #
################################################################################

cd `dirname $0`

cd ..

find . \( -iname "*.gladep" -o -iname "*.bak" -o -iname ".sconsign" -o \
-iname "*.tmp" -o -iname "config.log" -o -iname "*.dll" -o -name "*.exe" -o \
-iname "*.rej" -o -iname "*.mdb" -o -iname "AssemblyInfo.cs" -o -iname "*~" \) -exec rm -f {} \;

rm -fr .sconsign.dblite
rm -fr .sconf_temp
rm -fr diva.conf

echo "Cleaned..."
