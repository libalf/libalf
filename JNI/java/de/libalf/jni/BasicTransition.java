package de.libalf.jni;
public class BasicTransition {

	public int source, destination, label;

	public BasicTransition(int source, int label, int destination) {
		this.source = source;
		this.label = label;
		this.destination = destination;
	}

	public String toString() {
		return "(" + source + ", " + label + ", " + destination + ")";
	} 
}
