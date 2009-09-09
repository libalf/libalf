package de.libalf.dispatcher;

import java.io.IOException;

public class Test {
	public static void main(String[] args) throws IOException {
		DispatcherFactory factory = null;
		try {
			factory = new DispatcherFactory("134.130.57.19", 24940);
			factory.dispatchHelloCarsten(23);

			System.out.println("creating kb ...");
			DispatcherKnowledgebase kb = factory.createKnowledgebase();

			System.out.println(kb.id);

//			while (true) {
				System.out.println("creating l ...");
				DispatcherLogger l = factory.createLogger();
				System.out.println(l.id);
				System.out.println(l.receive_and_flush());
				l.finalize();
//			}			
			kb.finalize();
		} catch (Throwable e) {
			e.printStackTrace();
		}

//		while (true) {
//			System.out.println(Integer.toHexString(factory.io.readInt()));
//		}
	}
}
