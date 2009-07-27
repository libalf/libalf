# $Id$
# libalf JNI Makefile

.PHONY: all test_java_dir clean install windows-deps

# FOR WINDOWS:
# - use MinGW (tested with MinGW-GCC 3.4)
# - get LibGMP (e.g. http://progressive-living.com/opensource/gmp/gmp-4.2.2_win_lib_h.zip)
# - set JAVA_INCLUDE (at the top of this file) to your JDK include path
# - just run "make windows"

SYS = $(shell uname)

ifeq (${SYS}, Windows)
  JAVA_INCLUDE ?= d:\Progra~1\Java\jdk1.6.0_14\include
  CPPFLAGS += -I${JAVA_INCLUDE}/win32 -Wl,--kill-at
#                                      ^ sonst klappt JNI-DLL nicht (http://www.mingw.org/wiki/JNI-MinGW-DLL)

  LDFLAGS = -static ${LDFLAGS} -dynamic
  LDFLAGS += -lgmpxx -lgmp -lwsock32
  LDFLAGS += -L../src -L../LanguageGenerator/src
  TARGET=libalf_jni.dll
else
  # assuming linux
  #JAVA_INCLUDE ?= /usr/lib/jvm/java-6-sun-1.6.0.14/include
  JAVA_INCLUDE ?= ${JAVA_HOME}/include
  CPPFLAGS += -I${JAVA_INCLUDE}/linux
  LDFLAGS += -L${PREFIX}/lib -lalf -llangen
  TARGET=libalf_jni.so
endif

CPPFLAGS += -I../include -I../LanguageGenerator/include
CPPFLAGS += -shared -fPIC -I${PREFIX}/include -I${JAVA_INCLUDE}

OBJECTS = jni_knowledgebase.o jni_algorithm.o jni_algorithm_angluin.o jni_algorithm_angluin_column.o jni_algorithm_biermann_minisat.o jni_algorithm_nlstar.o jni_algorithm_rpni.o jni_buffered_logger.o jni_tools.o jni_libalf_object.o jni_query_iterator.o jni_random_generator.o

all:	test_java_dir libalf_jni.so

libalf_jni.so: ${OBJECTS}
	g++ ${OBJECTS}  ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} -o $@

libalf_jni.dll: ${OBJECTS}
	make -C ../src libalf.a
	make -C ../LanguageGenerator/src liblangen.a
	g++ ${OBJECTS}  ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} -o $@

test_java_dir:
	@echo -e "\nSystem is ${SYS}\n"
	@echo -e "Testing if JAVA include directory (env JAVA_INCLUDE) is set correctly...."
	test -d ${JAVA_INCLUDE}
	@echo

clean:
	-rm *.o libalf_jni.so

install: all
	@echo -e "\n\nNO JNI INSTALLED. generic rule is missing."

