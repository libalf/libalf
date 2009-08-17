/* $Id$
 * vim: fdm=marker
 *
 * Finite Automata Tools
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>

#include <unistd.h>

#include "fat.h"

#include <libalf/basic_string.h>
#include <libalf/automaton.h>

bool generate_samples_rpni(finite_automaton * automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_mdfa);
	
	return false;
}

bool generate_samples_delete2(finite_automaton * automaton, knowledgebase<bool> & base)
{
	list<int> word;
	set<list<int> > SP, spk;
	set<list<int> > sample_set;

	set<int> initial, final;
	initial = automaton->get_initial_states();
	final = automaton->get_final_states();

	do_transformation(automaton, trans_rfsa);

	// get SP(A) and K(A)
	for(unsigned int i = 0; i < automaton->get_state_count(); i++) {
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
		}
	}

	sample_set = spk;

	// get discriminating suffixes for all tuples (v,w) v \in SP, w \in SP \cup K
	set<list<int> >::iterator spi, spki;
	for(spi = SP.begin(); spi != SP.end(); ++spi) {
		set<int> sp_states;
		list<int> sp_word = *spi;
		sp_states = automaton->run(initial, sp_word.begin(), sp_word.end());

		for(spki = spk.begin(); spki != spk.end(); ++spki) {
			set<int> spk_states;
			list<int> spk_word;
			spk_word = *spki;
			spk_states = automaton->run(initial, spk_word.begin(), spk_word.end());
			if(sp_states != spk_states) {
				cerr << "discrimination " << word2string(sp_word) << " != " << word2string(spk_word) << " via suffix ";
				// different residual languages. find discriminating word.
				list<int> discriminator;
				bool dis_found = false;
				set<int> sp_reachable, spk_reachable;
				set<int>::iterator si;
				while(discriminator.size() <= automaton->get_state_count()) {
					//cerr << "dis " << word2string(discriminator) << " ?\n";
					sp_reachable.clear();
					spk_reachable.clear();
					bool sp_accepts = false, spk_accepts = false;
					sp_reachable = automaton->run(sp_states, discriminator.begin(), discriminator.end());
					spk_reachable = automaton->run(spk_states, discriminator.begin(), discriminator.end());

					// check if one contains accepting, the other not.
					for(si = sp_reachable.begin(); si != sp_reachable.end(); si++) {
						if(final.find(*si) != final.end()) {
							sp_accepts = true;
							break;
						}
					}
					for(si = spk_reachable.begin(); si != spk_reachable.end(); si++) {
						if(final.find(*si) != final.end()) {
							spk_accepts = true;
							break;
						}
					}
					if(sp_accepts != spk_accepts) {
						// check if both suffixes are not in a sink.
						// FIXME
						
						list<int> * cc;
						cc = concat(sp_word, discriminator);
						sample_set.insert(*cc);
						free(cc);
						cc = concat(spk_word, discriminator);
						sample_set.insert(*cc);
						free(cc);

						cerr << word2string(discriminator) << "\n";
						dis_found = true;

						break;
					}

					// try next suffix
					inc_graded_lex(discriminator, automaton->get_alphabet_size());
				}
				if(!dis_found) {
					cerr << "no good discriminator found!\n";
					cerr << "\t" << word2string(sp_word) << " has states ";
					for(si = sp_reachable.begin(); si != sp_reachable.end(); si++)
						cerr << *si << ", ";
					cerr << "\n\t" << word2string(spk_word) << " has states ";
					for(si = spk_reachable.begin(); si != spk_reachable.end(); si++)
						cerr << *si << ", ";
					cerr << "\n";
				}
			} else {
				cerr << "discrimination " << word2string(sp_word) << " ~~ " << word2string(spk_word) << "\n";
			}
		}
	}

	// close the set so that all words are in pref(s+)
	// FIXME
	

	// create knowledge in sample-set
	base.clear();
	set<list<int> >::iterator swi;
	for(swi = sample_set.begin(); swi != sample_set.end(); ++swi) {
		word = *swi;
		base.add_knowledge(word, automaton->contains(word));
	}

	cerr << "\n---\n\n";

	return true;
}

bool generate_samples_biermann(finite_automaton * automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_mdfa);
	
	return false;
}

bool generate_samples_random(finite_automaton * automaton, knowledgebase<bool> & base, int count)
{
	
	return false;
}

bool generate_samples_depth(finite_automaton * automaton, knowledgebase<bool> & base, int depth)
{
	
	return false;
}

bool generate_samples(finite_automaton * automaton, knowledgebase<bool> & base, string sampletype)
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

bool write_output(finite_automaton *& automaton, output out, string sampletype)
{{{
	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;

	bool f_is_dfa;
	int f_alphabet_size;
	int f_state_count;
	std::set<int> f_initial;
	std::set<int> f_final;
	multimap<pair<int,int>, int> f_transitions;

	switch(out) {
		case output_serial:
			serial = automaton->serialize();
			if(!basic_string_to_fd(serial, STDOUT_FILENO)) {
				cerr << "failed to write serial\n";
				return false;
			}
			return true;
		case output_dotfile:
			cout << automaton->generate_dotfile();
			return true;
		case output_human_readable:
			// nasty...
			serial = automaton->serialize();
			si = serial.begin();
			if(!deserialize_automaton(si, serial.end(), f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
				cerr << "failed to decompose automaton to human readable(1)\n";
				return false;
			}
			if(si != serial.end()) {
				cerr << "failed to decompose automaton to human readable(2)\n";
				return false;
			}
			f_is_dfa = automaton_is_deterministic(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
			cout << write_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);

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
				if(out == output_sample_text) {
					//cout << base.generate_dotfile();
					cout << base.tostring();
				}
			}

			return true;
	}

	return false;
}}}

