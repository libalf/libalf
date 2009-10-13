package de.libalf.demo;

import java.io.Serializable;
import java.util.HashMap;
import java.util.LinkedList;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class Statistics implements Serializable {

	private static final long serialVersionUID = 1L;

	private HashMap<String, Object> statistics = new HashMap<String, Object>();

	private LinkedList<ChangeListener> changeListener = new LinkedList<ChangeListener>();

	public void setValue(String key, Object value) {
		statistics.put(key, value);
		commitChange();
	}

	public void setIntValue(String key, Integer value) {
		statistics.put(key, value);
		commitChange();
	}

	public void setBooleanValue(String key, Boolean value) {
		statistics.put(key, value);
		commitChange();
	}

	public void setLongValue(String key, Long value) {
		statistics.put(key, value);
		commitChange();
	}

	public Object getValue(String key) {
		return statistics.get(key);
	}

	public Integer getIntValue(String key) {
		return (Integer) statistics.get(key);
	}

	public Long getLongValue(String key) {
		return (Long) statistics.get(key);
	}

	public Boolean getBooleanValue(String key) {
		return (Boolean) statistics.get(key);
	}

	public void addChangeListener(ChangeListener l) {
		changeListener.add(l);
	}

	public void removeChangeListener(ChangeListener l) {
		changeListener.remove();
	}

	private void commitChange() {
		for (ChangeListener l : changeListener)
			l.stateChanged(new ChangeEvent(this));
	}

	public boolean containsKey(String key) {
		return statistics.containsKey(key);
	}

	public boolean valueIsNull(String key) {
		return statistics.get(key) == null;
	}
}
