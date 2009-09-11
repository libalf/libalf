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

			int[] serialization = kb.serialize();
			System.out.println(Arrays.toString(serialization));

			kb.kill();
			
			kb = factory.createKnowledgebase();

			System.out.println(kb.deserialize(serialization));
			System.out.println(kb.deserialize(new int[0]));
			
			kb.kill();

			System.out.println("disconnect: " + DispatcherCommandError.getErrorString(factory.dispatchDisconnect()));
		} catch (Throwable e) {
			e.printStackTrace();
		}

		factory.io.printRest(500);
	}
}
