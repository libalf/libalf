package de.libalf.test;

import java.util.LinkedList;

import de.libalf.Knowledgebase;
import de.libalf.LibALFFactory;
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
		System.out.println(base);
		
		base.add_knowledge(new int[] { 0, 1, 2 }, true);
		base.resolve_or_add_query(new int[] { 1, 1 });
		System.out.println(base);

		int[] serialization = base.serialize();
		base.clear();
		
		System.out.println(base);
		
		base.deserialize(serialization);
		System.out.println(base);
	}

}
