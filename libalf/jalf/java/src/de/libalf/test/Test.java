package de.libalf.test;

import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.LibALFFactory.Algorithm;
import de.libalf.jni.JNIFactory;

public class Test {

	public static String intArray2String(int[] array) {
		if (array == null)
			return null;
		String res = "[";
		for (int i = 0; i < array.length; i++)
			res += array[i] + (i < array.length - 1 ? ", " : "");
		res += "]";
		return res;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		LibALFFactory factory = new JNIFactory();

		Knowledgebase base = factory.createKnowledgebase();
		LearningAlgorithm alg = factory.createLearningAlgorithm(
				Algorithm.ANGLUIN, new Object[] { base, 2 });
		
		alg.advance();
		System.out.println(alg);
	}

}
