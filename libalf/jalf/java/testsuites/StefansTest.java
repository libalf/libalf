import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;

import de.libalf.Conjecture;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.Normalizer;
import de.libalf.LibALFFactory.Algorithm;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.jni.JNIFactory;

// TODO: remove class
@Deprecated
public class StefansTest {
	public static void main(String[] args) throws Throwable {
		File file = new File("blah.jdat");
		file.deleteOnExit();

		LibALFFactory factory = false ? new JNIFactory() : new DispatcherFactory("127.0.0.1", 24940);
		try {
			Knowledgebase kb = factory.createKnowledgebase();
			Logger l = factory.createLogger();
			LearningAlgorithm a = factory.createLearningAlgorithm(Algorithm.ANGLUIN, kb, 2, l);

			Normalizer n = factory.createNormalizer(Normalizer.Type.MSC);
			a.set_normalizer(n);

			{
				ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(file));
				out.writeObject(a);
				out.close();
			}

			a.destroy();
			kb.destroy();
			l.destroy();
			n.destroy();
			factory.destroy();

			////

			{
				ObjectInputStream in = new ObjectInputStream(new FileInputStream(file));
				a = (LearningAlgorithm) in.readObject();
				in.close();
			}

			System.out.println(a.get_normalizer());

			System.exit(0);

			////

			System.out.println(kb);
			System.out.println(kb.resolve_or_add_query(new int[] { 1, 1, 1, 1, 1 }));
			kb.add_knowledge(new int[] { 1, 1, 0 }, true);
			kb.add_knowledge(new int[] { 1, 1, 1 }, false);
			kb.add_knowledge(new int[] { 1, 0, 0 }, true);
			System.out.println(kb);
			a.advance();
			System.out.println(kb);

			System.exit(0);

			////

			System.out.println(kb);

			while (true) {
				Conjecture con = a.advance();
				System.out.println(con);
				if (con != null)
					break;

				for (int[] word : kb.get_queries()) {
					System.out.print("? " + Arrays.toString(word));
					boolean b = word.length == 0 | word.length == word[0];
					System.out.println(" ! " + b);
					kb.add_knowledge(word, b);
				}
			}

			////

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
		}
	}
}
