
CPPFLAGS=-I${PREFIX}/include
LDFLAGS=-L${PREFIX}/lib -lalf -lAMoRE

iterator-test: iterator-test.cpp
	g++ ${CPPFLAGS} ${LDFLAGS} iterator-test.cpp -o iterator-test

clean:
	rm iterator-test

