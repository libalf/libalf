#!/bin/sh

# e.g. "0.3"
VERSION="INSERT-RELEASE-TAG-HERE"
VERSIONSTRING="v$VERSION"

echo "did you check the version \"$VERSION\" ?"
exit

echo "are we really in a tag and not in trunk?"
exit

echo $VERSION > finite-automata-tool/version
echo $VERSION > libalf/version
echo $VERSION > libalf_interfaces/version
echo $VERSION > libAMoRE/version
echo $VERSION > libAMoRE++/version
echo $VERSION > liblangen/version
echo $VERSION > libmVCA/version

svn add finite-automata-tool/version libalf/version libalf_interfaces/version libAMoRE/version libAMoRE++/version liblangen/version libmVCA/version

echo "now svn commit and make release tarballs."
