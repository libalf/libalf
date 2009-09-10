package de.libalf.dispatcher;

import java.util.Arrays;

import de.libalf.LibALFFactory.Algorithm;

public class Test {
	public static void main(String[] args) throws Throwable {
		DispatcherFactory factory = new DispatcherFactory("127.0.0.1", 24940);
		try {

			factory.dispatchHelloCarsten(23);

			System.out.println("creating kb ...");
			DispatcherKnowledgebase kb = factory.createKnowledgebase();

			System.out.println(kb);

			System.out.println("creating l ...");
			DispatcherLogger l = factory.createLogger();
			System.out.println(l);
			System.out.println(l.receive_and_flush());

			factory.dispatchGetObjectType(l);

			l.finalize();
			
			System.out.println(Arrays.toString(kb.serialize()));

			kb.finalize();

			kb = factory.createKnowledgebase();

			kb.finalize();

			System.out.println("disconnect: " + DispatcherConstants.getErrorString(factory.dispatchDisconnect()));
		} catch (Throwable e) {
			e.printStackTrace();
		}

		factory.io.printRest(500);
	}
}
