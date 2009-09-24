package de.libalf.dispatcher;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;

import de.libalf.BasicAutomaton;
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
			LearningAlgorithm a = factory.createLearningAlgorithm(Algorithm.ANGLUIN, kb, 2, l);

			System.out.println(kb);

			while (true) {
				BasicAutomaton auto = a.advance();
				System.out.println(auto);
				if (auto != null)
					break;

				for (int[] word : kb.get_queries()) {
					System.out.print("? " + Arrays.toString(word));
					boolean b = word.length == 0 || word.length == word[word.length - 1];
					System.out.println(" ! " + b);
					kb.add_knowledge(word, b);
				}
			}

			////

			File file = new File("blah.jdat");
			file.deleteOnExit();

			ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(file));
			out.writeObject(a);
			out.close();

			a.destroy();
			kb.destroy();
			l.destroy();
			factory.destroy();

			////

			ObjectInputStream in = new ObjectInputStream(new FileInputStream(file));
			a = (LearningAlgorithm) in.readObject();
			in.close();

			System.out.println(a.advance());
			System.out.println(kb = a.get_knowledge_source());

			for (int[] word : kb.get_knowledge())
				System.out.println(Arrays.toString(word) + " > " + kb.resolve_query(word));

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
