# $Id$
# libalf dispatcher testsuite Makefile

.PHONY: all clean

CPPFLAGS+=-Wall -ggdb3 -D__cplusplus -fno-stack-protector
LDFLAGS+=

all:	protocol

protocol: protocol.o
	g++ protocol.o ${LDFLAGS} -o protocol

clean:
	-rm -f protocol *.o

