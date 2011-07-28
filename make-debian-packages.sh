#!/bin/sh

make uninstall
make clean
sudo make install

if [ "x$ARCH" = "x" ]; then
	ARCH=`uname -m`
fi;

TEMPDIR="`pwd`/tmp"
mkdir "$TEMPDIR"

DEPLOY="$TEMPDIR/deploy"
mkdir "$DEPLOY"

WORK="$TEMPDIR/work"
mkdir "$WORK"

PREFIX=/usr

prep_env() {
	DESTDIR="${WORK}/$1"
	mkdir "${DESTDIR}"
	chmod 755 "${DESTDIR}"

	if [ -f "$1/version" ]; then
		export VERSION=`cat "$1/version"`
		export SVNTAG=""
	else
		export VERSION=`cd $1; LC_ALL=POSIX svn info | awk '/^Revision:/ {print $2}'`
		export SVNTAG="-svn"
		export SVNDESC=" (svn version)"
	fi;
}

prep_deb_sysroot() {
	mkdir "$1/DEBIAN"
	chmod 755 "$1/DEBIAN"
	touch "$1/DEBIAN/control"
	chmod 644 "$1/DEBIAN/control"
}

create_deb() {
	fakeroot dpkg -b "${DESTDIR}" "${DEPLOY}/$1${SVNTAG}_${VERSION}_${ARCH}.deb"
}

#
# libalf
#
prep_env libalf
make -C libalf DESTDIR=${DESTDIR} PREFIX=${PREFIX} install || exit 1
prep_deb_sysroot "${DESTDIR}"
cat > "$DESTDIR/DEBIAN/control" << _eof_libalf
Package: libalf${SVNTAG}
Priority: optional
Section: devel
Maintainer: David R. Piegdon <david-i2@piegdon.de>
Architecture: ${ARCH}
Version: ${VERSION}
Description: The libalf learning framework${SVNDESC}
_eof_libalf
create_deb libalf || exit 1

#
# liblangen
#
prep_env liblangen
make -C liblangen DESTDIR=${DESTDIR} PREFIX=${PREFIX} install || exit 1
prep_deb_sysroot "${DESTDIR}"
cat > "$DESTDIR/DEBIAN/control" << _eof_liblangen
Package: liblangen${SVNTAG}
Priority: optional
Section: devel
Maintainer: David R. Piegdon <david-i2@piegdon.de>
Architecture: ${ARCH}
Version: ${VERSION}
Description: C++ library for generating (regular) languages by means of randomly drawn finite automata or regular expressions${SVNDESC}
_eof_liblangen
create_deb liblangen || exit 1

#
# libmVCA
#
prep_env libmVCA
make -C libmVCA DESTDIR=${DESTDIR} PREFIX=${PREFIX} install || exit 1
prep_deb_sysroot "${DESTDIR}"
cat > "$DESTDIR/DEBIAN/control" << _eof_libmVCA
Package: libmVCA${SVNTAG}
Priority: optional
Section: devel
Maintainer: David R. Piegdon <david-i2@piegdon.de>
Architecture: ${ARCH}
Version: ${VERSION}
Description: C++ library for visibly one-counter automata${SVNDESC}
_eof_libmVCA
create_deb libmVCA || exit 1

#
# libAMoRE (++)
#
prep_env libAMoRE-++
make -C libAMoRE DESTDIR=${DESTDIR} PREFIX=${PREFIX} install || exit 1
make -C libAMoRE++ DESTDIR=${DESTDIR} PREFIX=${PREFIX} install || exit 1
prep_deb_sysroot "${DESTDIR}"
cat > "$DESTDIR/DEBIAN/control" << _eof_libAMoRE
Package: libAMoRE-++${SVNTAG}
Priority: optional
Section: devel
Maintainer: David R. Piegdon <david-i2@piegdon.de>
Architecture: ${ARCH}
Version: ${VERSION}
Description: comprehensive automata library and its c++ interface${SVNDESC}
_eof_libAMoRE
create_deb libAMoRE-++ || exit 1

#
# libalf-interfaces
#
prep_env libalf-interfaces
make -C libalf_interfaces DESTDIR=${DESTDIR} PREFIX=${PREFIX} install || exit 1
prep_deb_sysroot "${DESTDIR}"
cat > "$DESTDIR/DEBIAN/control" << _eof_libalf-interfaces
Package: libalf-interfaces${SVNTAG}
Priority: optional
Section: devel
Maintainer: David R. Piegdon <david-i2@piegdon.de>
Architecture: ${ARCH}
Version: ${VERSION}
Description: The libalf learning framework interfaces${SVNDESC}
_eof_libalf-interfaces
create_deb libalf-interfaces || exit 1

#
# done
#
echo done. packages are in ${DEPLOY}

