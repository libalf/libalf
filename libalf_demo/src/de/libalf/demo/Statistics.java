/*
 * This file is part of libalf-demo.
 *
 * libalf-demo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf-demo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf-demo.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009 Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

package de.libalf.demo;

import java.io.Serializable;
import java.util.HashMap;
import java.util.LinkedList;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
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
