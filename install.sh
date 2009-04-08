#/bin/sh
# $Id$
#
# install-skript for non-superuser

if [ "$#" != "1" ]; then
	echo "exactly one parameter is required: prefix-directory"
	exit
fi;

PRE=$1


export PREFIX=$PRE
export BINDIR=$PREFIX/bin
export LIBDIR=$PREFIX/lib
export HEADER_INSTALLDIR=$PREFIX/include
#export DOCDIR=$PREFIX/doc


mkdir -p $PREFIX $BINDIR $LIBDIR $HEADER_INSTALLDIR
#mkdir -p $DOCDIR


export CXXFLAGS="$CXXFLAGS -I$PREFIX/include"
export LDFLAGS="-L$PREFIX/lib"


cd libAMoRE-1.0
make && make install


cd ..
make clean install

echo " "
echo " "
echo " "
echo " "
echo "note that, to run any programs linked against the installed libs, you will have to call"
echo " "
echo " $ LD_LIBRARY_PATH=\"$LIBDIR\" ./my_program"
echo " "
echo " "
echo " "
echo " "

