This file describes how to run the libalf demo. For more detailed instructions
please refer to the libalf website http://libalf.informatik.rwth-aachen.de.

You need to have a Java Runtime Environment 1.6 or higher installed in order to
run the libalf demo.


To run the libalf demo, just follow these simple steps:

1) Download libalf, compile the jalf.jar and copy it to the folder the
   libalf_demo.jar is located.

2) Compile the libalf and jalf C++ and copy the compiled libraries into your
   demo folder.
  
4) Download
   * brics (http://www.brics.dk/automaton/) and
   * JGraph (http://www.jgraph.com/)
   and copy the jar files into your demo folder.
   
5) Run
             java -classpath "..." de.libalf.demo.Starter
	
	where the classpath points to all four jar files. It should look like
	
	         -classpath "jalf.jar:jalf_demo.jar:jgraph.jar:automaton.jar"
	
6) In order to use JNI you have to specify the location of the libalf and jalf
   C++ libraries. You can do so by specifying -Djava.library.path=... just
   before the "-classpath" parameter.
   
   If libalf is dynamically linked into the jalf library, you also need to make
   sure that Java finds the libalf library. On Windows you should be fine as
   Windows searches in the current working directory for the library. If this
   does not work, try to add the demo folder to your PATH variable. On Linux add
   LD_LIBRARY_PATH=. right befor the java command to set the Linux library path
   temporarily to the current directory. 