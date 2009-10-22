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
import java.util.LinkedList;

import de.libalf.LibALFFactory.Algorithm;
import de.libalf.demo.filter.Filter;
import dk.brics.automaton.Automaton;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 *
 */
public class Scenario implements Serializable {

	public static final int LIBALF_DEFAULT_PORT = 24940;

	private static final long serialVersionUID = 2L;

	private String description, server;

	private int port, alphabetSize;

	private Algorithm algorithm;

	private Automaton teacher;

	private boolean jniConnection, useTeacher;

	private LinkedList<Filter> filters;

	private LinkedList<Sample> samples;

	private Statistics statistics;

	public Scenario(String description, String server, int port,
			int alphabetSize, Algorithm algorithm, Automaton teacher,
			boolean useTeacher, boolean jniConnection,
			LinkedList<Filter> filters, LinkedList<Sample> samples,
			Statistics statistics) {
		super();
		this.description = description;
		this.server = server;
		this.port = port;
		this.alphabetSize = alphabetSize;
		this.algorithm = algorithm;
		this.useTeacher = useTeacher;
		this.teacher = teacher;
		this.jniConnection = jniConnection;
		this.filters = filters == null ? new LinkedList<Filter>() : filters;
		this.samples = samples == null ? new LinkedList<Sample>() : samples;
		this.statistics = statistics;
	}

	public Scenario() {
		this("", "libalf.informatik.rwth-aachen.de", LIBALF_DEFAULT_PORT, 2,
				Algorithm.ANGLUIN, null, false, true, new LinkedList<Filter>(),
				new LinkedList<Sample>(), new Statistics());
	}

	public static Scenario createDefaultScenario() {
		return new Scenario();
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public String getServer() {
		return server;
	}

	public void setServer(String server) {
		this.server = server;
	}

	public int getPort() {
		return port;
	}

	public void setPort(int port) {
		this.port = port;
	}

	public int getAlphabetSize() {
		return alphabetSize;
	}

	public void setAlphabetSize(int alphabetSize) {
		this.alphabetSize = alphabetSize;
	}

	public Algorithm getAlgorithm() {
		return algorithm;
	}

	public void setAlgorithm(Algorithm algorithm) {
		this.algorithm = algorithm;
	}

	public Automaton getTeacher() {
		return teacher;
	}

	public void setTeacher(Automaton teacher) {
		this.teacher = teacher;
	}

	public boolean isJniConnection() {
		return jniConnection;
	}

	public void setJniConnection(boolean jniConnection) {
		this.jniConnection = jniConnection;
	}

	public LinkedList<Filter> getFilters() {
		return filters;
	}

	public void setFilters(LinkedList<Filter> filters) {
		this.filters = filters;
	}

	public void addFilter(Filter f) {
		if (!filters.contains(f))
			filters.add(f);
	}

	public void removeFilter(Filter f) {
		filters.remove(f);
	}

	public LinkedList<Sample> getSamples() {
		return samples;
	}

	public void setSamples(LinkedList<Sample> samples) {
		this.samples = samples;
	}

	/**
	 * <p>
	 * Adds a new sample to the list.
	 * </p>
	 * <p>
	 * The sample is not added if
	 * <ul>
	 * <li>either the list already contains the sample</li>
	 * <li>or the sample is contradicting, i.e. is already</li>
	 * </ul>
	 * </p>
	 * 
	 * @param s
	 *            the new sample to add
	 */
	public void addSample(Sample s) {
		for (Sample sample : samples) {
			if (wordsAreEqual(s.word, sample.word))
				return;
		}
		samples.add(s);
	}

	public void removeSample(int[] sample) {
		samples.remove(sample);
	}

	private boolean wordsAreEqual(int[] w1, int[] w2) {
		if (w1 == null && w2 == null)
			return true;
		else if (w1 == null && w2 != null)
			return false;
		else if (w1 != null && w2 == null)
			return false;
		else {
			if (w1.length == w2.length && w1.length == 0)
				return true;
			else if (w1.length != w2.length)
				return false;
			else {
				for (int i = 0; i < w1.length; i++)
					if (w1[i] != w2[i])
						return false;
				return true;
			}
		}
	}

	public Statistics getStatistics() {
		return statistics;
	}

	public void setStatistics(Statistics statistics) {
		this.statistics = statistics;
	}

	public boolean isUseTeacher() {
		return useTeacher;
	}

	public void setUseTeacher(boolean useTeacher) {
		this.useTeacher = useTeacher;
	}
}
