package de.libalf;

public interface Normalizer extends LibALFObject {

	public static enum Type {
		MSC;
	}

	public boolean deserialize(int[] serialization) throws AlfException;

	public int get_type() throws AlfException;

}