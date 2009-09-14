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
			DispatcherLearningAlgorithm a = factory.createLearningAlgorithm(Algorithm.ANGLUIN_COLUMN, kb, 7);

			ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("blah.jdat"));
			out.writeObject(a);
			out.close();
			
			a.destroy();
//			kb.destroy();
			factory.destroy();
			
			////
			
			ObjectInputStream in = new ObjectInputStream(new FileInputStream("blah.jdat"));
			a = (DispatcherLearningAlgorithm) in.readObject();
			in.close();
			
			System.out.println(a);
			System.out.println(a.get_knowledge_source());
			System.out.println(a.supports_sync());
			System.out.println(a.sync_to_knowledgebase());
			
			factory = a.factory;
			factory.sendNoOp();
			factory.destroy();
			
			System.exit(0);
			
			////
			
			

			factory.sendNoOp();

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
				
				a.destroy();
			} catch (Exception e) {
				e.printStackTrace();
			}

			kb.destroy();

//			System.out.println(l.receive_and_flush());
			
//			l.kill();

			factory.destroy();
		} catch (Throwable e) {
			e.printStackTrace();
			factory.printRest(500);
		}
	}
}
