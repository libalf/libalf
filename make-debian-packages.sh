#!/bin/sh
# vim:fdm=marker

# functions for debian package builting {{{

log() {
	echo -e "\033[1;34m$@\033[m"
}

prep_env() {
	PNAME="$1"
	SDIR="$2"
	DESTDIR="${WORK}/$PNAME"

	mkdir "${DESTDIR}"
	chmod 755 "${DESTDIR}"

	if [[ -f "$SDIR/version" ]]; then
		export VERSION=`cat "$SDIR/version"`
		export SVNTAG=""
	else
		export VERSION=`cd $SDIR; LC_ALL=POSIX svn info | awk '/^Revision:/ {print $2}'`
		export SVNTAG="-svn"
		export SVNDESC=" (svn version)"
	fi;
}

prep_deb_sysroot() {
	rm -Rf "$DESTDIR/DEBIAN" > /dev/null 2>&1
	mkdir "$DESTDIR/DEBIAN"
	chmod 755 "$DESTDIR/DEBIAN"
	touch "$DESTDIR/DEBIAN/control"
	chmod 644 "$DESTDIR/DEBIAN/control"
}

create_deb() {
	PNAME="$1"
	fakeroot dpkg -b "${DESTDIR}" "${DEPLOY}/${PNAME}${SVNTAG}_${VERSION}_${ARCH}.deb"
}

# }}}

log ""
log "preparing..."
# prepare environment vars {{{

if [[ "x$PREFIX" == "x" ]]; then
	PREFIX=/usr
fi;
MAKEOPTS="-j3"

# TEMPDIR has to be an absolute path
TEMPDIR="`pwd`/tmp"
DEPLOY="$TEMPDIR/deploy"
WORK="$TEMPDIR/work"
LOGS="$TEMPDIR/log"

if [[ "x$ARCH" = "x" ]]; then
	ARCH=`uname -m`
fi;

# }}}
# prepare building directories {{{

rm -Rf "$TEMPDIR"

mkdir "$TEMPDIR" || exit 1
mkdir "$DEPLOY"
mkdir "$WORK"
mkdir "$LOGS"

# }}}
log "all logfiles in ${LOGS}"
log "you may be required to give your password for sudo"

log ""
log "installing deps for correct linking"
# install libs for linking {{{

# clean up so there are no bad linkings
log " + initial clean"
sudo make PREFIX="${PREFIX}" uninstall > "${LOGS}/0_0_cleanup" || exit 1
sudo make clean > /dev/null >> "${LOGS}/0_cleanup" || exit 1

# install dependencies
for LIB in libalf liblangen libmVCA libAMoRE libAMoRE++; do
	log " + ${LIB}"
	sudo make ${MAKEOPTS} -C ${LIB} PREFIX="${PREFIX}" install > "${LOGS}/0_1_install_deps" || exit 1
done;

log " + cleanup"
# and clean again so user-rights do not conflict later
rm "${LOGS}/2_clean_deps"
for LIB in libalf liblangen libmVCA libAMoRE libAMoRE++; do
	sudo make -C ${LIB} clean >> "${LOGS}/0_2_clean_deps" || exit 1
done;

# }}}

log ""
log "creating packages:"
log " + libalf"
# libalf {{{

prep_env libalf libalf
make -C libalf ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install > "${LOGS}/1_0_libalf" || exit 1
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

create_deb libalf >> "${LOGS}/1_0_libalf" || exit 1

# }}}
log " + liblangen"
# liblangen {{{

prep_env liblangen liblangen
make -C liblangen ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install > "${LOGS}/1_1_liblangen" || exit 1
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

create_deb liblangen >> "${LOGS}/1_1_liblangen" || exit 1

# }}}
log " + libmVCA"
# libmVCA {{{

prep_env libmVCA libmVCA
make -C libmVCA ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install > "${LOGS}/1_2_libmVCA" || exit 1
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

create_deb libmVCA >> "${LOGS}/1_2_libmVCA" || exit 1

# }}}
log " + libAMoRE-++"
# libAMoRE (++) {{{

prep_env libAMoRE-++ libAMoRE++
make -C libAMoRE ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install > "${LOGS}/1_3_libAMoRE-++" || exit 1
make -C libAMoRE++ ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install >> "${LOGS}/1_3_libAMoRE-++" || exit 1
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

create_deb libAMoRE-++ >> "${LOGS}/1_3_libAMoRE-++" || exit 1

# }}}
log " + libalf-interfaces"
# libalf-interfaces {{{

prep_env libalf-interfaces libalf_interfaces
make -C libalf_interfaces ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install > "${LOGS}/1_4_libalf-interfaces" || exit 1
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

create_deb libalf-interfaces >> "${LOGS}/1_4_libalf-interfaces" || exit 1

# }}}
log " + finite-automata-tools"
# finite-automata-tools {{{

prep_env finite-automata-tool finite-automata-tool
make -C finite-automata-tool ${MAKEOPTS} DESTDIR=${DESTDIR} PREFIX=${PREFIX} install > "${LOGS}/1_5_finite-automata-tools" || exit 1
prep_deb_sysroot "${DESTDIR}"

cat > "$DESTDIR/DEBIAN/control" << _eof_libalf-interfaces
Package: finite-automata-tool${SVNTAG}
Priority: optional
Section: devel
Maintainer: David R. Piegdon <david-i2@piegdon.de>
Architecture: ${ARCH}
Version: ${VERSION}
Description: finite-automata-tool${SVNDESC}
_eof_libalf-interfaces

create_deb finite-automata-tool >> "${LOGS}/1_5_finite-automata-tools" || exit 1

# }}}

log ""
log "cleanup"
# cleanup {{{

sudo make PREFIX=${PREFIX} uninstall > "${LOGS}/2_cleanup"

# }}}

log ""
log "done"
log "packages are in ${DEPLOY}"

