package de.libalf.junit;

import de.libalf.*;
import de.libalf.jni.*;
import de.libalf.LibALFFactory.Algorithm;

public class TestInferringAlg {

	public static void main(String[] args) {
	System.loadLibrary("jalf");
	JNIFactory jniFactory = JNIFactory.STATIC;
	Knowledgebase base = jniFactory.createKnowledgebase();
	Logger logger = jniFactory.createLogger();
	int alphabet_size = 2;

	bool unary_encoding = true;
	bool use_variables = true; 
	bool use_enum = true;

	
	JNIAlgorithmDeterministicInferringCSPMinisat jniAlgorithm = ((JNIAlgorithmDeterministicInferringCSPMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_CSP_MINISAT, base, logger, 2, unary_encoding));

	//JNIAlgorithmDeterministicInferringCSPZ3 jniAlgorithm = ((JNIAlgorithmDeterministicInferringCSPZ3)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_CSP_Z3, base, logger, 2, use_variables, use_enum));

	//JNIAlgorithmDeterministicInferringMinisat jniAlgorithm = ((JNIAlgorithmDeterministicInferringMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_MINISAT, base, logger, 2, unary_encoding));

	//JNIAlgorithmDeterministicInferringZ3 jniAlgorithm = ((JNIAlgorithmDeterministicInferringZ3)jniFactory.createLearningAlgorithm(Algorithm.INFER_DETERMINISTIC_Z3, base, logger, 2, use_variables, use_enum));

	//JNIAlgorithmDFAInferringMinisat jniAlgorithm = ((JNIAlgorithmDFAInferringMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_DFA_MINISAT, base, logger, 2, unary_encoding));

	//JNIAlgorithmDFAInferringZ3 jniAlgorithm = ((JNIAlgorithmDFAInferringZ3)jniFactory.createLearningAlgorithm(Algorithm.INFER_DFA_Z3, base, logger, 2, use_variables, use_enum));

	//JNIAlgorithmNFAInferringMinisat jniAlgorithm = ((JNIAlgorithmNFAInferringMinisat)jniFactory.createLearningAlgorithm(Algorithm.INFER_NFA_MINISAT, base, logger, 2, unary_encoding));


	// minisat

	boolean value = jniAlgorithm.uses_unary_encoding();
	jniAlgorithm.set_unary_encoding(!value);
	System.out.print("Unary encoding: ");		
	System.out.println(jniAlgorithm.uses_unary_encoding() != value ? "CHECK" : "ERROR");
	
	// Z3

	/*boolean value = jniAlgorithm.uses_variables();
	jniAlgorithm.set_using_variables(!value);
	System.out.print("uses_variables: ");		
	System.out.println(jniAlgorithm.uses_variables() != value ? "CHECK" : "ERROR");
	value = jniAlgorithm.uses_enum();
	jniAlgorithm.set_using_enum(!value);
	System.out.print("use_enum: ");		
	System.out.println(jniAlgorithm.uses_enum() != value ? "CHECK" : "ERROR");
	*/
	}
}
