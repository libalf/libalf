# $Id$
# Makefile for java jalf testcase

.PHONY: all clean run

all:	Test.class

Test.class: Test.java
	javac -cp ../jalf.jar Test.java

clean:
	-rm Test.class 2>&1

run:	Test.class
	java -cp ".:..:../jalf.jar" -Djava.library.path=.. Test

