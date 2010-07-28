# $Id: Makefile 814 2009-09-04 08:28:54Z stefanschulz $
# libalf JNI Makefile

.PHONY: all test_java_dir clean install windows-deps

# FOR WINDOWS:
# - use MinGW (tested with MinGW-GCC 3.4)
# - get LibGMP (e.g. http://progressive-living.com/opensource/gmp/gmp-4.2.2_win_lib_h.zip)
# - set JAVA_INCLUDE (at the top of this file) to your JDK include path

LDFLAGS += -L${PREFIX}/lib -L${PREFIX}/src -L../../src

ifeq (${OS}, Windows_NT)
  # assuming Windows
  JAVA_INCLUDE ?= c:\Progra~1\Java\JDK16~1.0_1\include

  CPPFLAGS += -I${JAVA_INCLUDE}/win32

  LDFLAGS += -lws2_32

  TARGET=jalf.dll
else
  # assuming linux
  #JAVA_INCLUDE ?= /usr/lib/jvm/java-6-sun-1.6.0.14/include
  JAVA_INCLUDE ?= ${JAVA_HOME}/include

  CPPFLAGS += -I${JAVA_INCLUDE}/linux

  TARGET=libjalf.so
endif

CPPFLAGS += -I../include -I../../include
CPPFLAGS += -shared -fPIC -I${PREFIX}/include -I${JAVA_INCLUDE}

OBJECTS = jni_tools.o jni_object.o jni_knowledgebase.o jni_buffered_logger.o jni_normalizer.o jni_learning_algorithm.o jni_algorithm_angluin.o jni_algorithm_angluin_column.o jni_algorithm_nlstar.o jni_algorithm_kearns_vazirani.o jni_algorithm_rpni.o jni_algorithm_biermann_original.o jni_algorithm_biermann_minisat.o jni_algorithm_delete2.o 

all:	${TARGET}

libjalf.so: test_java_dir ${OBJECTS}
	make -C ../../src libalf.a
	g++ ${OBJECTS} ${CPPFLAGS} ${CXXFLAGS} -Xlinker -soname=libjalf ${LDFLAGS} -static -lalf -o libjalf.so
	
libjalf.so-dynamic: test_java_dir ${OBJECTS}
	make -C ../../src libalf.so
	g++ ${OBJECTS} ${CPPFLAGS} ${CXXFLAGS} -Xlinker -soname=libjalf ${LDFLAGS} -lalf -o libjalf.so

jalf.dll: ${OBJECTS}
	make -C ../../src libalf.a
	g++ ${OBJECTS} ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} -static -lalf -o jalf.dll -Wl,--kill-at
#	                                                                            ^^^^^^^^^^^^^ needed for JNI-DLL (http://www.mingw.org/wiki/JNI-MinGW-DLL)

jalf.dll-dynamic: ${OBJECTS}
	make -C ../../src libalf.dll
	g++ ${OBJECTS} ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} -lalf -o jalf.dll -Wl,--kill-at
#	                                                                    ^^^^^^^^^^^^^ needed for JNI-DLL (http://www.mingw.org/wiki/JNI-MinGW-DLL)

test_java_dir:
	@echo
	@echo "Testing if JAVA include directory (env JAVA_INCLUDE) is set correctly...."
	test -d ${JAVA_INCLUDE}
	@echo

clean:
	-rm *.o libjalf.so jalf.dll

install: all
	@echo -e "\n\nNO JNI INSTALLED. generic rule is missing."

