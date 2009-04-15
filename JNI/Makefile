
.PHONY: clean

JAVA_INCLUDE ?= /usr/lib/jvm/java-6-sun-1.6.0.13/include

CPPFLAGS=-shared -fPIC -I${PREFIX}/include -I${JAVA_INCLUDE} -I${JAVA_INCLUDE}/linux/
LDFLAGS=-L${PREFIX}/lib -lalf

OBJECTS = jni_knowledgebase.o jni_algorithm_angluin.o

libalf_jni.so: ${OBJECTS}
	g++ ${OBJECTS} ${CPPFLAGS}  ${LDFLAGS} -o $@

clean:
	rm *.o libalf_jni.so


