# $Id$
# libalf dispatcher testsuite Makefile

.PHONY: all clean

CPPFLAGS+=-Wall -ggdb3 -fno-stack-protector
LDFLAGS+=

all:	protocol

protocol: protocol.o
	${CXX} protocol.o ${LDFLAGS} -o protocol

clean:
	-rm -f protocol *.o 2>&1

