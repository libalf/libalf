# $Id$
# libalf JNI Makefile

.PHONY: all test_java_dir clean install windows-deps

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

ifeq (${OS}, Windows_NT)
  # FOR WINDOWS:
  # - use MinGW (tested with MinGW-GCC 3.4)
  # - get LibGMP (e.g. http://progressive-living.com/opensource/gmp/gmp-4.2.2_win_lib_h.zip)
  # - set JAVA_INCLUDE (at the top of this file) to your JDK include path
  JAVA_INCLUDE ?= c:\Progra~1\Java\JDK16~1.0_1\include
  CPPFLAGS += -I${JAVA_INCLUDE}/win32
  LDFLAGS += -lws2_32
  TARGET=jalf.dll
else
  # assuming linux
  JAVA_INCLUDE ?= ${JAVA_HOME}/include
  CPPFLAGS += -I${JAVA_INCLUDE}/linux
  TARGET=libjalf.so
endif


CPPFLAGS += -I../include -I../../include -I${INCLUDEDIR} -I${JAVA_INCLUDE}
CPPFLAGS += -shared -fpic


LDFLAGS += -L${LIBDIR} -L../../../libalf/src
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
		jni_algorithm_delete2.o


all:	${TARGET}

test_java_includes:
	@echo
	@echo "Testing if JAVA include directory (env JAVA_INCLUDE) is set correctly...."
	test -d ${JAVA_INCLUDE}
	@echo

clean:
	-rm *.o libjalf.so jalf.dll

install: all
	@echo -e "\n\nNO JNI INSTALLED. generic rule is missing."



# Linux targets
libjalf.so: test_java_includes ${OBJECTS}
	+make -C ../../../libalf/src libalf.a
	g++ ${LDFLAGS}  -Xlinker -soname=libjalf ../../../libalf/src/libalf.a  ${OBJECTS}  -o libjalf.so

libjalf.so-dynamic: test_java_includes ${OBJECTS}
	+make -C ../../../libalf/src libalf.so
	g++ ${LDFLAGS}  -Xlinker -soname=libjalf -lalf  ${OBJECTS}  -o libjalf.so


# Windows targets
jalf.dll: ${OBJECTS}
	+make -C ../../../libalf/src libalf.a
	g++ ${LDFLAGS}  ../../../libalf/src/libalf.a  ${OBJECTS}  -o jalf.dll -Wl,--kill-at
#	                                                                      ^^^^^^^^^^^^^ needed for JNI-DLL (http://www.mingw.org/wiki/JNI-MinGW-DLL)
jalf.dll-dynamic: ${OBJECTS}
	+make -C ../../src libalf.dll
	g++ ${LDFLAGS}  -lalf  ${OBJECTS}  -o jalf.dll -Wl,--kill-at
#	                                               ^^^^^^^^^^^^^ needed for JNI-DLL (http://www.mingw.org/wiki/JNI-MinGW-DLL)



