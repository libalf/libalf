#!/bin/sh

# e.g. "0.3"
VERSION="INSERT-RELEASE-TAG-HERE"
VERSIONSTRING="v$VERSION"

echo did you create all version files?
exit

svn export https://svn-i2.informatik.rwth-aachen.de/repos/libalf/tags/${VERSIONSTRING} release

cd release

rm -Rf libalf/testsuites/sample-automata

tar jcf libalf-${VERSIONSTRING}.tar.bz2                 libalf
tar jcf "libAMoRE(++)-${VERSIONSTRING}.tar.bz2"         libAMoRE libAMoRE++ 
tar jcf libmVCA-${VERSIONSTRING}.tar.bz2                libmVCA
tar jcf liblangen-${VERSIONSTRING}.tar.bz2              liblangen 
tar jcf finite-automata-tool-${VERSIONSTRING}.tar.bz2   finite-automata-tool 

mv *.tar.bz2 ../

cd ..

rm -Rf release

