package de.libalf.dispatcher;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;

import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.LibALFFactory.Algorithm;
import de.libalf.jni.JNIFactory;

// TODO: remove class
@Deprecated
public class Test {
	public static void main(String[] args) throws Throwable {
		LibALFFactory factory;
		factory = new JNIFactory();
		factory = new DispatcherFactory("127.0.0.1", 24940);
		try {
			Knowledgebase kb = factory.createKnowledgebase();
			Logger l = factory.createLogger();
			LearningAlgorithm a = factory.createLearningAlgorithm(Algorithm.ANGLUIN, kb, 7, l);

			ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("blah.jdat"));
			out.writeObject(a);
			out.close();

			System.out.println(kb);

			a.destroy();
			kb.destroy();
			l.destroy();
			factory.destroy();

			////

			ObjectInputStream in = new ObjectInputStream(new FileInputStream("blah.jdat"));
			a = (LearningAlgorithm) in.readObject();
			in.close();

			System.out.println(a.advance());
			System.out.println(kb = a.get_knowledge_source());

			factory.destroy();

			System.exit(0);

			////

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
			if (factory instanceof DispatcherFactory)
				((DispatcherFactory) factory).printRest(500);
		}
	}
}
