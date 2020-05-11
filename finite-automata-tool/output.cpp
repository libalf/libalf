/* $Id$
 * vim: fdm=marker
 *
 * This file is part of Finite Automata Tools (FAT).
 *
 * FAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FAT.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009,2010,2011
 *		Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *          and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 *          and David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>

#include <unistd.h>

#include "fat.h"

#include <libalf/basic_string.h>
#include <libalf/conjecture.h>
#include <libalf/serialize.h>

#include <amore++/nondeterministic_finite_automaton.h>

// for knowledgebase:
bool leaf_is_non_accepting(knowledgebase<bool>::node* n, list<int> & sample, bool prefix_accepting = false)
// check if all leafs (i.e. states that have no suffixes that either accept or reject) accept
{{{
	list<int> w;
	if(n->is_answered()) {
		if(n->get_answer() == true)
			prefix_accepting = true;
		else
			if(n->get_answer() == false)
				prefix_accepting = false;
	}

	int i;
	bool has_children = false;
	knowledgebase<bool>::node * c;

	for(i = 0; i < n->max_child_count(); i++) {
		c = n->find_child(i);
		if(c) {
			has_children = true;
			if(leaf_is_non_accepting(c, sample, prefix_accepting))
				return true;
		}
	}
	if(!has_children && !prefix_accepting) {
		sample = n->get_word();
		return true;
	}

	return false;
}}}

bool generate_samples_rpni(amore::finite_automaton *& automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_mdfa);
	// FIXME: TO BE IMPLEMENTED
	
	return false;
}

bool generate_samples_delete2(amore::finite_automaton *& automaton, knowledgebase<bool> & base)
{{{
	list<int> word;
	set<list<int> > SP, spk;
	set<list<int> > sample_set;

	do_transformation(automaton, trans_mdfa);

	set<int> initial, final;
	initial = automaton->get_initial_states();
	final = automaton->get_final_states();

	// get SP(A) and K(A)
	for(unsigned int i = 0; i < automaton->get_state_count(); i++) {{{
		bool reachable;
		set<int> s;
		s.insert(i);
		word = automaton->shortest_run(initial, s, reachable);
		if(reachable) {
			// SP(A):
			SP.insert(word);
			spk.insert(word);
			sample_set.insert(word);
			cerr << "SP  " << word2string(word) << "\n";
			// K(A):
			bool is_empty_residual = true;
			for(unsigned int sigma = 0; sigma < automaton->get_alphabet_size(); sigma++) {
				word.push_back(sigma);
				s.clear();
				s = automaton->run(initial, word.begin(), word.end());
				if(!s.empty()) {
					is_empty_residual = false;
					spk.insert(word);
					cerr << "\tK " << word2string(word) << "\n";
				}
				word.pop_back();
			}
			// we don't want negative sinks.
			if(is_empty_residual && final.find(i) == final.end()) {
				SP.erase(word);
				spk.erase(word);
				cerr << "SP  " << word2string(word) << " is negative sink. skipping.\n";
			}
		} else {
			cerr << "not reachable: " << i << "\n";
		}
	}}}

	sample_set = spk;

	// get discriminating suffixes for all tuples (v,w) v \in SP, w \in SP \cup K
	for(set<list<int> >::iterator spi = SP.begin(); spi != SP.end(); ++spi) {{{
		list<int> sp_word = *spi;
		set<int> sp_states;
		amore::finite_automaton *sp_residual;

		sp_states = automaton->run(initial, sp_word.begin(), sp_word.end());
		sp_residual = automaton->clone();
		sp_residual->set_initial_states(sp_states);

		for(set<list<int> >::iterator spki = spk.begin(); spki != spk.end(); ++spki) {
			list<int> spk_word;
			set<int> spk_states;
			amore::finite_automaton *spk_residual;

			spk_word = *spki;
			spk_states = automaton->run(initial, spk_word.begin(), spk_word.end()); // XXX
			cerr << "discriminate " << word2string(sp_word) << " and " << word2string(spk_word) << "\n";

			spk_residual = automaton->clone();
			spk_residual->set_initial_states(spk_states);

			if(sp_states != spk_states) {
				amore::finite_automaton * difference;
				difference = spk_residual->lang_symmetric_difference(*sp_residual);

				list<int> discriminator;
				bool empty;
				discriminator = difference->get_sample_word(empty);
				if(empty) {
					cerr << "unable to discriminate " << word2string(sp_word) << " and " << word2string(spk_word) << "\n";
				} else {
					list<int> * cc;

					cc = concat(sp_word, discriminator);
					sample_set.insert(*cc);
					delete cc;

					cc = concat(spk_word, discriminator);
					sample_set.insert(*cc);
					delete cc;
				}

				delete difference;
			}
			delete spk_residual; // XXX
		}
		delete sp_residual;
	}}}

	// create knowledge in sample-set
	base.clear();
	set<list<int> >::iterator swi;
	for(swi = sample_set.begin(); swi != sample_set.end(); ++swi) {
		word = *swi;
		base.add_knowledge(word, automaton->contains(word));
	}

	// close the set so that all words are in pref(s+)
	// this is really inefficient. so what? :)
	while(leaf_is_non_accepting(base.get_rootptr(), word)) {{{
		// close w.r.t. word:
		//
		// get residual language and find an accepting word
		set<int> residual_states;
		amore::finite_automaton * residual_language;

		residual_states = automaton->run(initial, word.begin(), word.end());
		residual_language = automaton->clone();
		residual_language->set_initial_states(residual_states);

		list<int> suffix;
		bool empty;
		suffix = residual_language->get_sample_word(empty);
		if(empty) {
			cerr << "residual language of " << word2string(word) << " is empty!\naborting.\n";
			break;
		}

		cerr << "closing via " << word2string(word) << " + " << word2string(suffix) << "\n";

		list<int> * cc;
		cc = concat(word, suffix);
		base.add_knowledge(*cc, automaton->contains(*cc));
		delete cc;
	}}}

	cerr << "\n---\n\n";

	return true;
}}}

bool generate_samples_biermann(amore::finite_automaton *& automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_mdfa);
	// FIXME: TO BE IMPLEMENTED
	
	return false;
}

bool generate_samples_random(amore::finite_automaton *& automaton, knowledgebase<bool> & base, unsigned int count)
{
	// FIXME: TO BE IMPLEMENTED
	
	return false;
}

bool generate_samples_depth(amore::finite_automaton *& automaton, knowledgebase<bool> & base, unsigned int depth)
{{{
	list<int> word;

	while(word.size() <= depth) {
		base.add_knowledge(word, automaton->contains(word));
		inc_graded_lex(word, automaton->get_alphabet_size());
	}

	return true;
}}}

bool generate_samples(amore::finite_automaton *& automaton, knowledgebase<bool> & base, string sampletype)
{{{
	size_t pos;
	string st;
	string para;
	// sampletype: <rpni|delete2|biermann|random|depth>[:<int>]

	pos = sampletype.find_first_of(':');
	if(pos != string::npos) {
		st = sampletype.substr(0, pos);
		para = sampletype.substr(pos+1, string::npos);
	} else {
		st = sampletype;
		para = "";
	}

	if(st == "rpni") {
		return generate_samples_rpni(automaton, base);
	} else {
		if(st == "delete2") {
			return generate_samples_delete2(automaton, base);
		} else {
			if(st == "biermann") {
				return generate_samples_biermann(automaton, base);
			} else {
				if(st == "random") {
					int count = atoi(para.c_str());
					if(count <= 0) {
						cerr << "invalid parameter for random sample-set\n";
						return false;
					}
					return generate_samples_random(automaton, base, count);
				} else {
					if(st == "depth") {
						int depth = atoi(para.c_str());
						if(depth <= 0) {
							cerr << "invalid parameter for random sample-set\n";
							return false;
						}
						return generate_samples_depth(automaton, base, depth);
					} else {
						cerr << "invalid sample-set option\n";
						return false;
					}
				}
			}
		}
	}

	return false;
}}}

bool write_output(amore::finite_automaton *& automaton, output out, string sampletype)
{{{
	basic_string<int32_t> serial;

	serial_stretch ser;

	libalf::finite_automaton aut;

	switch(out) {
		case output_serial:
			serial = automaton->serialize();
			if(!basic_string_to_fd(serial, STDOUT_FILENO)) {
				cerr << "failed to write serial\n";
				return false;
			}
			return true;
		case output_dotfile:
			cout << automaton->visualize();
			return true;
		case output_human_readable:
			// nasty...
			serial = automaton->serialize();
			ser.init(serial);
			if(!aut.deserialize(ser)) {
				cerr << "failed to decompose automaton to human readable (garbage inside)\n";
				return false;
			}
			if(!ser.empty()) {
				cerr << "failed to decompose automaton to human readable (garbage behind)\n";
				return false;
			}
			cout << aut.write();

			return true;
		case output_regex:
			cout << automaton->to_regex() << "\n";
			return true;
		case output_sample:
		case output_sample_text:
			knowledgebase<bool> base;

			if(!generate_samples(automaton, base, sampletype)) {
				cerr << "failed to generate samples\n";
				return false;
			}

			if(out == output_sample) {
				basic_string<int32_t> serial;
				serial = base.serialize();
				if(!basic_string_to_fd(serial, STDOUT_FILENO)) {
					cerr << "failed to write serial\n";
					return false;
				}
			} else {
				if(out == output_sample_text)
					cout << base.to_string();
			}

			return true;
	}

	return false;
}}}

