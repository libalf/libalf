# $Id$
# libalf JNI Makefile

.PHONY: all test_java_dir clean install windows-deps

include ../../config.mk

VERSIONTAG:=$(shell cat "../../version" 2>/dev/null)
ifeq (${VERSIONTAG},)
GITHASH := $(shell git --no-pager rev-parse --short HEAD)
DIRTYFLAG := $(shell git --no-pager status -s | wc -c | sed -e 's/^[1-9][0-9]*$$/-DIRTY/' -e 's/^0$$//')
VERSION := git-${GITHASH}${DIRTYFLAG}
LIBVERSIONTAG := .git
else
# release branch
VERSION := v${VERSIONTAG}
LIBVERSIONTAG := .${VERSIONTAG}
endif

# Choose OS
ifeq (${OS}, Windows_NT)
  # FOR WINDOWS:
  # - use MinGW (tested with MinGW-GCC 3.4)
  # - get LibGMP (e.g. http://progressive-living.com/opensource/gmp/gmp-4.2.2_win_lib_h.zip)
  # - set JAVA_INCLUDE (at the top of this file) to your JDK include path
  #   When cross-compiling, make sure that you point to the include path of the JDK of the target system!
  JAVA_INCLUDE ?= c:\Progra~1\Java\JDK16~1.0_1\include
  CPPFLAGS += -I${JAVA_INCLUDE}/win32
  #LDFLAGS += -lws2_32
  TARGET=jalf.dll
else
  # assuming linux
  JAVA_INCLUDE ?= ${JAVA_HOME}/include
  CPPFLAGS += -I${JAVA_INCLUDE}/linux
  TARGET=libjalf.so
endif

INSTALL_SHARED_NAME=${TARGET}${LIBVERSIONTAG}

CPPFLAGS += -I../include -I${INCLUDEDIR} -I${JAVA_INCLUDE}
CPPFLAGS += -shared -fpic
CPPFLAGS += -O3

LDFLAGS += -L${LIBDIR}
LDFLAGS += -shared

OBJECTS =	jni_tools.o					\
		jni_object.o 					\
		jni_knowledgebase.o				\
		jni_buffered_logger.o				\
		jni_normalizer.o				\
		jni_learning_algorithm.o			\
		jni_algorithm_angluin.o				\
		jni_algorithm_angluin_column.o			\
		jni_algorithm_nlstar.o				\
		jni_algorithm_kearns_vazirani.o			\
		jni_algorithm_rivest_schapire.o			\
		jni_algorithm_rpni.o				\
		jni_algorithm_biermann_original.o		\
		jni_algorithm_biermann_minisat.o		\
		jni_algorithm_delete2.o				\
		jni_algorithm_deterministic_inferring_csp_minisat.o \
		jni_algorithm_deterministic_inferring_csp_z3.o 	\
		jni_algorithm_deterministic_inferring_minisat.o	\
		jni_algorithm_deterministic_inferring_z3.o 	\
		jni_algorithm_dfa_inferring_minisat.o 		\
		jni_algorithm_dfa_inferring_z3.o	 	\
		jni_algorithm_nfa_inferring_minisat.o		\
		jni_algorithm_counterexample_learning.o


all:	${TARGET}

test_java_includes:
	@echo
	@echo "Testing if JAVA include directory (env JAVA_INCLUDE) is set correctly...."
	test -d ${JAVA_INCLUDE}
	@echo

clean:
	-rm -f *.o libjalf.so jalf.dll 2>&1

install: all
	@echo
	@echo installing ${TARGET} to ${DESTDIR}${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${LIBDIR}
	install -T -v -m 755 ${TARGET} ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	# symlinks
	-rm -f ${DESTDIR}${LIBDIR}/${TARGET} 2>&1
	ln -s ${LIBDIR}/${INSTALL_SHARED_NAME} ${DESTDIR}${LIBDIR}/${TARGET}
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing ${TARGET} from ${DESTDIR}${LIBDIR} ...
	@echo
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	-test `id -u` -eq 0 && /sbin/ldconfig; true


# Linux targets
libjalf.so: test_java_includes ${OBJECTS}
	${CXX} ${LDFLAGS} -Xlinker -soname=libjalf ${OBJECTS} ${LIBDIR}/libalf.a -o libjalf.so

libjalf.so-dynamic: test_java_includes ${OBJECTS}
	${CXX} ${LDFLAGS} -Xlinker -soname=libjalf ${OBJECTS} -lalf -o libjalf.so


# Windows targets
jalf.dll: ${OBJECTS}
	${CXX} ${LDFLAGS} ${OBJECTS} ${LIBDIR}/libalf.a -lws2_32 -o jalf.dll -Wl,--kill-at

jalf.dll-dynamic: ${OBJECTS}
	${CXX} ${LDFLAGS} ${OBJECTS} -lalf -lws2_32 -o jalf.dll -Wl,--kill-at



