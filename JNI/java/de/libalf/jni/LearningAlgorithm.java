package de.libalf.jni;

/**
 * @author Daniel Neider, Chair of Computer Science 7, RWTH Aachen University
 * @version 1.0
 */
public interface LearningAlgorithm {

	public void set_alphabet_size(int alphabet_size);
	
	public int get_alphabet_size();
	
	public void increase_alphabet_size(int new_asize);
	
	public void set_knowledge_source(Knowledgebase base);

	public Knowledgebase get_knowledge_source();
	
	public boolean conjecture_ready();
	
	public BasicAutomaton advance();
	
	public void add_counterexample(int[] counterexample);

	public boolean sync_to_knowledgebase();

	public boolean supports_sync();

	public int[] serialize();

	public boolean deserialize(int[] serialization);

	public String toString();
}
