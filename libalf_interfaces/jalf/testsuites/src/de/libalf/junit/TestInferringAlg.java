package de.libalf.junit;

import org.junit.Test;
import junit.framework.TestCase;
import static org.junit.Assert.*;

import de.libalf.*;
import de.libalf.jni.*;
import de.libalf.LibALFFactory.Algorithm;
    
public class TestInferringAlg
{
	int alphabet_size = 2;
	JNIFactory jniFactory = JNIFactory.STATIC;
	boolean value1 = true;
	boolean value2 = true;

	@Test
	public void detCSPMinisat()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmDeterministicInferringCSPMinisat jniAlgorithm = ((JNIAlgorithmDeterministicInferringCSPMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_CSP_MINISAT, base, logger, alphabet_size, value1));

		boolean temp1 = jniAlgorithm.uses_unary_encoding();
		jniAlgorithm.set_unary_encoding(!value1);
		assertTrue(temp1 != jniAlgorithm.uses_unary_encoding());
	}

	@Test
	public void detMinisat()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmDeterministicInferringMinisat jniAlgorithm = ((JNIAlgorithmDeterministicInferringMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_MINISAT, base, logger, alphabet_size, value1));
		
		boolean temp1 = jniAlgorithm.uses_symmetry_breaking();
		jniAlgorithm.set_symmetry_breaking(!value1);
		assertTrue(temp1 != jniAlgorithm.uses_symmetry_breaking());
	}

	@Test
	public void dfaMinisat()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmDFAInferringMinisat jniAlgorithm = ((JNIAlgorithmDFAInferringMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_DFA_MINISAT, base, logger, alphabet_size, value1));
		
		boolean temp1 = jniAlgorithm.uses_symmetry_breaking();
		jniAlgorithm.set_symmetry_breaking(!value1);
		assertTrue(temp1 != jniAlgorithm.uses_symmetry_breaking());
	}

	@Test
	public void nfaMinisat()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmNFAInferringMinisat jniAlgorithm = ((JNIAlgorithmNFAInferringMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_NFA_MINISAT, base, logger, alphabet_size, value1));
		
		boolean temp1 = jniAlgorithm.uses_symmetry_breaking();
		//jniAlgorithm.set_symmetry_breaking(!value1);
		//assertTrue(temp1 != jniAlgorithm.uses_symmetry_breaking());
	}

	@Test
	public void detCSPZ3()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmDeterministicInferringCSPZ3 jniAlgorithm = ((JNIAlgorithmDeterministicInferringCSPZ3)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_CSP_Z3, base, logger, alphabet_size, value1, value2));
		
		boolean temp1 = jniAlgorithm.is_using_variables();
		jniAlgorithm.set_using_variables(!value1);
		assertTrue(temp1 != jniAlgorithm.is_using_variables());
		boolean temp2 = jniAlgorithm.is_using_enum();
		jniAlgorithm.set_using_enum(!value2);
		assertTrue(temp2 != jniAlgorithm.is_using_enum());
	}
	
	@Test
	public void detZ3()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmDeterministicInferringZ3 jniAlgorithm = ((JNIAlgorithmDeterministicInferringZ3)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_Z3, base, logger, alphabet_size, value1, value2));
		
		boolean temp1 = jniAlgorithm.is_using_variables();
		jniAlgorithm.set_using_variables(!value1);
		assertTrue(temp1 != jniAlgorithm.is_using_variables());
		boolean temp2 = jniAlgorithm.is_using_enum();
		jniAlgorithm.set_using_enum(!value2);
		assertTrue(temp2 != jniAlgorithm.is_using_enum());
	}

	@Test
	public void dfaZ3()
	{
		Knowledgebase base = jniFactory.createKnowledgebase();
		Logger logger = jniFactory.createLogger();
		JNIAlgorithmDFAInferringZ3 jniAlgorithm = ((JNIAlgorithmDFAInferringZ3)jniFactory.createLearningAlgorithm(Algorithm.INFER_DFA_Z3, base, logger, alphabet_size, value1, value2));
		
		boolean temp1 = jniAlgorithm.is_using_variables();
		jniAlgorithm.set_using_variables(!value1);
		assertTrue(temp1 != jniAlgorithm.is_using_variables());
		boolean temp2 = jniAlgorithm.is_using_enum();
		jniAlgorithm.set_using_enum(!value2);
		assertTrue(temp2 != jniAlgorithm.is_using_enum());
	}

}
