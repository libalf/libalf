# $Id: Makefile 848 2009-09-09 00:15:23Z davidpiegdon $
# libalf dispatcher testsuite Makefile

.PHONY: all clean

CPPFLAGS+=-Wall -ggdb3 -D__cplusplus -fno-stack-protector
LDFLAGS+=

all:	protocol

protocol: protocol.o
	g++ protocol.o ${LDFLAGS} -o protocol

clean:
	-rm -f protocol *.o

