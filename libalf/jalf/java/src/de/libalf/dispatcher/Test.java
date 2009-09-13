package de.libalf.dispatcher;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;

import de.libalf.LibALFFactory.Algorithm;

public class Test {
	public static void main(String[] args) throws Throwable {
		DispatcherFactory factory = new DispatcherFactory("127.0.0.1", 24940);
		try {
			DispatcherKnowledgebase kb = factory.createKnowledgebase();
			DispatcherLearningAlgorithm a = factory.createLearningAlgorithm(Algorithm.NL_STAR, kb, 7);

			ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("blah.jdat"));
			out.writeObject(a);
			out.close();
			
			factory.kill();
			
			////
			
			ObjectInputStream in = new ObjectInputStream(new FileInputStream("blah.jdat"));
			a = (DispatcherLearningAlgorithm) in.readObject();
			in.close();
			
			System.out.println(a);
			
			factory = a.factory;
			factory.dispatchHelloCarsten(23);
			factory.kill();
			
			System.exit(0);
			
			////
			
			

			factory.dispatchHelloCarsten(23);

//			DispatcherLogger l = factory.createLogger();

//			DispatcherKnowledgebase kb = factory.createKnowledgebase();

//			DispatcherLearningAlgorithm a = factory.createLearningAlgorithm(Algorithm.ANGLUIN, kb, 10);
			System.out.println(a);
			System.out.println(a.advance());
			System.out.println(Arrays.toString(a.serialize()));

//			a.set_logger(l);
//			a.remove_logger();
			
			a.set_knowledge_source(kb);
			
			try {
//				System.out.println(a.sync_to_knowledgebase());
//				System.out.println(a.sync_to_knowledgebase());
//				System.out.println(a.sync_to_knowledgebase());
				
				a.kill();
			} catch (Exception e) {
				e.printStackTrace();
			}

			kb.kill();

//			System.out.println(l.receive_and_flush());
			
//			l.kill();

			factory.kill();
		} catch (Throwable e) {
			e.printStackTrace();
			factory.printRest(500);
		}
	}
}
