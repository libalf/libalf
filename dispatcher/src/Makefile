# $Id$
# libalf dispatcher Makefile

.PHONY: all clean install uninstall

CPPFLAGS+=-Wall -I../../include/ -I ../include/ -ggdb3
CPPFLAGS+=-D__cplusplus
LDFLAGS+=-lalf -lAMoRE
#-lboost_system -lpthread

OBJECTS=main.o

all:	dispatcher

dispatcher: ${OBJECTS}
	g++ ${OBJECTS} ${LDFLAGS} -o dispatcher

clean:
	-rm *.o
	-rm dispatcher

install: dispatcher

uninstall:

