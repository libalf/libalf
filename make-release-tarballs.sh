#!/bin/sh

VERSION="v0.1"

svn export https://svn-i2.informatik.rwth-aachen.de/repos/libalf/trunk release

cd release

rm -Rf libalf/testsuites/NLstar_count_eq_queries/found libalf/testsuites/online_performance_tester/stats-*

tar jcf libalf-${VERSION}.tar.bz2		libalf
tar jcf libAMoRE++-${VERSION}.tar.bz2		libAMoRE libAMoRE++ 
tar jcf liblangen-${VERSION}.tar.bz2		liblangen 
tar jcf finite-automata-tool-${VERSION}.tar.bz2	finite-automata-tool 

mv *.tar.bz2 ../

cd ..

rm -Rf release

