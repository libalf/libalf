# $Id$
# Makefile for java jalf testcase

.PHONY: all clean run

all:	Test

Test:
	javac -cp jalf.jar Test.java

clean:
	-rm Test.class

run:
	#java -cp ".:jalf.jar" -Djava.library.path=. Test
	java -cp ".:jalf.jar" Test

