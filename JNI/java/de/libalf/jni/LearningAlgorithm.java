package de.libalf.jni;

public interface LearningAlgorithm {

	public void set_alphabet_size(int alphabet_size);
	
	public int get_alphabet_size();
	
	public void increase_alphabet_size(int new_asize);
	
	public void set_knowledge_source(Knowledgebase base);
	
	public boolean conjecture_ready();
	
	public BasicAutomaton advance();
	
	public void add_counterexample(int[] counterexample);
	
}
