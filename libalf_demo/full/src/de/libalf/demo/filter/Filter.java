package de.libalf.demo.filter;

import java.io.Serializable;

public interface Filter extends Serializable {

	public enum Result {
		ACCEPT, REJECT, UNKNOWN;
	}

	public Result filter(int[] input);
	
	public String getDescription();
}
