/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libalf_session_h__
# define __libalf_session_h__

#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/answer.h>
#include <libalf/statistics.h>
#include <libalf/automaton_constructor.h>

using namespace libalf;

class session {
	private:
		buffered_logger logger;
		statistics stats;
		learning_algorithm<extended_bool> * alg;
		enum learning_algorithm<extended_bool>::algorithm alg_type;
		basic_automaton_holder hypothesis_automaton;
		knowledgebase<extended_bool> knowledge;
		normalizer * norm;
	public:
		session();
		session(enum learning_algorithm<extended_bool>::algorithm algorithm, int alphabet_size);
		~session();

		// protocol implementation:
		bool set_modalities(serversocket * sock);
		bool answer_status(serversocket * sock);
		bool set_status(serversocket * sock);
		bool answer_knowledge(serversocket * sock);
		bool set_knowledge(serversocket * sock);
		bool answer_conjecture(serversocket * sock);
		bool advance(serversocket * sock);
		bool get_query_answer(serversocket * sock);
		bool get_counterexamples(serversocket * sock);
		bool answer_alphabet_size(serversocket * sock);
		bool set_alphabet_size(serversocket * sock);
		bool increase_alphabet_size(serversocket * sock);
		bool answer_stats(serversocket * sock);
		bool set_stats(serversocket * sock);
		bool answer_log_request(serversocket * sock);
		bool undo(serversocket * sock);
		// debugging commands:
		bool log_table(serversocket * sock);
		bool normalize_word(serversocket * sock);
		bool log_knowledgebase(serversocket * sock);

	protected:
		bool set_normalizer(basic_string<int32_t> blob);
};


#endif // __libalf_session_h__

