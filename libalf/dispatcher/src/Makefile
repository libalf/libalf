# $Id$
# libalf dispatcher Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

LOCALCHANGES := $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION := $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION ?= svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../../include/ -I ../include/ -I${PREFIX}/include -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\"" -DVERBOSE_DEBUG
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-L${PREFIX}/lib -L../../src

OBJECTS=main.o serversocket.o servant.o co_learning_algorithm.o co_knowledgebase.o co_knowledgebase_iterator.o co_logger.o co_normalizer.o

all:	dispatcher

dispatcher: ${OBJECTS}
	g++ ${OBJECTS} ${LDFLAGS} -lalf -o dispatcher

dispatcher-static: ${OBJECTS}
	make -C ../../src libalf.a
	${CC} ${OBJECTS} ${LDFLAGS} -static -lalf -o dispatcher

clean:
	-rm -f dispatcher *.o

install: dispatcher
	cp dispatcher ${PREFIX}/bin/
	chmod 755 ${PREFIX}/bin/dispatcher

uninstall:
	echo removing dispatcher binary
	rm ${PREFIX}/bin/dispatcher

