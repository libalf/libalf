# $Id$
# libalf JNI Makefile

.PHONY: all test_java_dir clean install

JAVA_INCLUDE ?= ${JAVA_HOME}/include
#JAVA_INCLUDE ?= /usr/lib/jvm/java-6-sun-1.6.0.14/include

CPPFLAGS += -shared -fPIC -I${PREFIX}/include -I${JAVA_INCLUDE} -I${JAVA_INCLUDE}/linux/
LDFLAGS += -L${PREFIX}/lib -lalf -llangen

OBJECTS = jni_knowledgebase.o jni_algorithm.o jni_algorithm_angluin.o jni_algorithm_angluin_column.o jni_algorithm_biermann_minisat.o jni_algorithm_nlstar.o jni_algorithm_rpni.o jni_buffered_logger.o jni_tools.o jni_libalf_object.o jni_query_iterator.o jni_random_generator.o

all:	test_java_dir libalf_jni.so

libalf_jni.so: ${OBJECTS}
	g++ ${OBJECTS}  ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} -o $@

test_java_dir:
	@echo -e "\ntesting if JAVA include directory (env JAVA_INCLUDE) is set correctly...."
	test -d ${JAVA_INCLUDE}
	@echo

clean:
	-rm *.o libalf_jni.so

install: all
	@echo -e "\n\nNO JNI INSTALLED. generic rule is missing."

