#!/bin/bash
# Travis Hansen
# Michael Dominic K.
# Thijs Vermeir
# 20060417
# This file is used to update the list of files to be included in the POTFILES.in
# and create a diva.pot template file

PODIR=`pwd`
PROJECTNAME="diva"

echo
echo "Searching for project files to include in POTFILES.in..."
cd ../
find src/ -name "*.c" -o -name "*.h" -o -name "*.cs" -o -name "*.glade" > ${PODIR}/POTFILES.in

# This is to remove all references to Diva.Config assembly
# There are no translatable strings there, and the tool 
# is dumb and picks up gconf-paths as translatable strings!
echo "Stripping all references to Diva.Config assembly..."
sed "/^src\/Diva.Config/d" -i ${PODIR}/POTFILES.in

cd ${PODIR}

echo "Generating ${PROJECTNAME}.pot file..."
intltool-update -p -g ${PROJECTNAME}

echo "Updating po files"

# Copy the next 4 lines to the end of the file 

LANG="Dutch"
POFILE="nl.po"
echo "$LANG : $POFILE"
msgmerge -U ${POFILE} ${PROJECTNAME}.pot && msgfmt --statistics ${POFILE}

# and change the LANG en POFILE variable

LANG="French"
POFILE="fr.po"
echo "$LANG : $POFILE"
msgmerge -U ${POFILE} ${PROJECTNAME}.pot && msgfmt --statistics ${POFILE}
