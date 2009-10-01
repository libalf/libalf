package de.libalf;

import java.io.Serializable;

public interface Conjecture extends Serializable {
	public static Conjecture NONE = new Conjecture() {
		private static final long serialVersionUID = 1L;
	};
}
