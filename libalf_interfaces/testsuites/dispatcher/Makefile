# $Id$
# Makefile for java jalf testcase

.PHONY: all clean run

all:	Test.class

Test.class:
	javac -cp jalf.jar Test.java

clean:
	-rm Test.class

run:	Test.class
	#java -cp ".:jalf.jar" -Djava.library.path=. Test
	#  -Djava.library.path=. is only required for jalf
	java -cp ".:jalf.jar" Test

