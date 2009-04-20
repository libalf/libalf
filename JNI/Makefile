# $Id$
# libalf JNI Makefile

.PHONY: clean test_java_dir all

JAVA_INCLUDE ?= /usr/lib/jvm/java-6-sun-1.6.0.13/include

CPPFLAGS += -shared -fPIC -I${PREFIX}/include -I${JAVA_INCLUDE} -I${JAVA_INCLUDE}/linux/
LDFLAGS += -L${PREFIX}/lib -lalf

OBJECTS = jni_knowledgebase.o jni_algorithm.o jni_algorithm_angluin.o jni_algorithm_biermann_minisat.o jni_buffered_logger.o jni_tools.o

all:	test_java_dir libalf_jni.so

libalf_jni.so: ${OBJECTS}
	g++ ${OBJECTS}  ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} -o $@

test_java_dir:
	@echo "testing if JAVA include directory (env JAVA_INCLUDE) is set correctly...."
	test -d ${JAVA_INCLUDE}

clean:
	rm *.o libalf_jni.so

