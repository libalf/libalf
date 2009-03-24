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

#include <iostream>

#include <libalf/alf.h>
#include <libalf/algorithm_angluin.h>
#include <libalf/algorithm_biermann_minisat.h>
#include <libalf/automata_amore.h>
#include <libalf/normalizer_msc.h>

#include <arpa/inet.h>

#include <serversocket.h>
#include <session.h>
#include <protocol.h>

using namespace std;
using namespace libalf;

session::session()
{{{
	alg = NULL;
	alg_type = learning_algorithm<extended_bool>::ALG_NONE;
	hypothesis_automaton = NULL;
	norm = NULL;
	logger.set_minimal_loglevel(LOGGER_DEBUG);
	logger.set_log_algorithm(true);
	logger(LOGGER_WARN, "new session: default constructor\n");
}}}

session::session(enum learning_algorithm<extended_bool>::algorithm algorithm, int alphabet_size)
{{{
	logger.set_minimal_loglevel(LOGGER_DEBUG);
	logger.set_log_algorithm(true);
	alg_type = algorithm;
	norm = NULL;
	switch (algorithm) {
		case learning_algorithm<extended_bool>::ALG_ANGLUIN:
			alg = new angluin_simple_observationtable<extended_bool>(NULL, &knowledge, &logger, alphabet_size);
			hypothesis_automaton = new deterministic_finite_amore_automaton;
			logger(LOGGER_INFO, "new session: angluin observationtable\n");
			break;
		case learning_algorithm<extended_bool>::ALG_BIERMANN:
			alg = new MiniSat_biermann<extended_bool>(&knowledge, &logger, alphabet_size);
			hypothesis_automaton = new deterministic_finite_amore_automaton;
			logger(LOGGER_INFO, "new session: biermann with CSP2SAT and MiniSat SAT-solver\n");
			break;
		default:
			alg = NULL;
			hypothesis_automaton = NULL;
			logger(LOGGER_ERROR, "new session: received invalid algorithm '%d'\n", algorithm);
			break;
	}
}}}

session::~session()
{{{
	if(alg)
		delete alg;
}}}

bool session::set_normalizer(basic_string<int32_t> blob)
{{{
	int length;
	basic_string<int32_t>::iterator bi;
	enum normalizer::type type;

	length = blob.size();
	if(length < 2) {
		return false;
	}

	// alg->unset_normalizer also deletes the normalizer we are referencing in norm.
	alg->unset_normalizer();

	type = (enum normalizer::type) ntohl(blob[1]);

	switch (type) {
		case normalizer::NORMALIZER_NONE:
			if(length != 2)
				return false;
			// already unset :)
			return true;
		case normalizer::NORMALIZER_MSC:
			norm = new normalizer_msc();
			break;
		default:
			return false;
	}

	bi = blob.begin();
	if(!norm->deserialize(bi, blob.end())) {
		return false;
	}

	if(bi != blob.end()) {
		delete norm;
		return false;
	}

	alg->set_normalizer(norm);
	return true;
}}}

bool session::set_modalities(serversocket * sock)
{{{
	int count;
	int32_t d;
	bool success = true;

	if(!sock->stream_receive_int(d))
		return false;

	d = ntohl(d);
	for(count = d; count > 0; count--) {
		int32_t d;
		int length;
		enum modality_type type;
		basic_string<int32_t> blob;
		basic_string<int32_t>::iterator bi;

		if(!sock->stream_receive_int(d))
			return false;
		length = ntohl(d);

		if(length < 1) {
			return false;
		}

		if(!sock->stream_receive_int(d))
			return false;
		type = (enum modality_type)ntohl(d);
		length--;


		switch(type) {
			case MODALITY_SET_NORMALIZER:
				if(length < 2) {
					return false;
				}
				for(/* -- */; length > 0; length--) {
					if(!sock->stream_receive_int(d))
						return false;
					blob.push_back(d);
				}
				if(!this->set_normalizer(blob)) {
					return false;
				}
				break;
			case MODALITY_EXTEND_NORMALIZER:
				if(length < 1)
					return false;
				for(/* -- */; length > 0; length--) {
					if(!sock->stream_receive_int(d))
						return false;
					blob.push_back(d);
				}

				bi = blob.begin();
				if(!norm->deserialize_extension(bi, blob.end()))
					return false;
				if(bi != blob.end())
					return false;

				break;
			case MODALITY_SET_LOGLEVEL:
				if(length != 1)
					return false;
				if(!sock->stream_receive_int(d))
					return false;
				d = ntohl(d);
				logger.set_minimal_loglevel((enum logger_loglevel)d);
				break;
			case MODALITY_SET_LOG_ALGORITHM:
				if(length != 1)
					return false;
				if(!sock->stream_receive_int(d))
					return false;
				d = ntohl(d);
				logger.set_log_algorithm(d != 0);
				break;
			default:
				return false;
		}
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_MODALITIES)))
		return false;
	return sock->stream_send_int(htonl(success ? 1 : 0));
}}}
bool session::answer_status(serversocket * sock)
{{{
	if(!sock->stream_send_int(htonl(SM_SES_ACK_REQ_STATUS)))
		return false;
	basic_string<int32_t> blob;
	blob = alg->serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::set_status(serversocket * sock)
{{{
	int count;
	int32_t d;
	basic_string<int32_t> blob; // ntohl of this is done in alg->deserialize() !
	basic_string<int32_t>::iterator bi;

	if(!sock->stream_receive_int(d))
		return false;
	blob += d;

	for(count = ntohl(d); count > 0; count--) {
		if(!sock->stream_receive_int(d))
			return false;
		blob += d;
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_SET_STATUS)))
		return false;

	bi = blob.begin();
	d = alg->deserialize(bi, blob.end());

	// send status of deserialization to client
	if(!sock->stream_send_int(htonl(d)))
		return false;

	return (bi == blob.end());
}}}
bool session::answer_knowledge(serversocket * sock)
{{{
	if(!sock->stream_send_int(htonl(SM_SES_ACK_REQ_KNOWLEDGE)))
		return false;
	basic_string<int32_t> blob;
	blob = knowledge.serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::set_knowledge(serversocket * sock)
{{{
	int count;
	int32_t d;
	basic_string<int32_t> blob; // ntohl of this is done in knowledge.deserialize() !
	basic_string<int32_t>::iterator bi;
	bool clear_old;
	bool ok = true;

	// bool: clear old?
	if(!sock->stream_receive_int(d))
		return false;
	clear_old = ntohl(d);

	// size
	if(!sock->stream_receive_int(d))
		return false;
	blob += d;

	// receive full knowledgebase data
	for(count = ntohl(d); count > 0; count--) {
		if(!sock->stream_receive_int(d))
			return false;
		blob += d;
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_SET_KNOWLEDGE)))
		return false;

	bi = blob.begin();

	if(clear_old) {
		knowledge.clear();
		knowledge.deserialize(bi, blob.end());
	} else {
		knowledgebase<extended_bool> newknowledge;
		newknowledge.deserialize(bi, blob.end());
		ok = knowledge.merge_knowledgebase(newknowledge);
	}

	// send status of deserialization to client
	if(!sock->stream_send_int(htonl(ok)))
		return false;

	return (bi == blob.end());
}}}
bool session::answer_conjecture(serversocket * sock)
{{{
	if(!sock->stream_send_int(htonl(SM_SES_ACK_CONJECTURE)))
		return false;
	return sock->stream_send_int(htonl( alg->conjecture_ready() ? 1 : 0 ));
}}}
bool session::advance(serversocket * sock)
{{{
	basic_string<int32_t> blob;

	if(alg->advance(hypothesis_automaton)) {
		// send automaton to client
		if(!sock->stream_send_int(htonl(SM_SES_ACK_ADVANCE_AUTOMATON)))
			return false;
		blob = hypothesis_automaton->serialize();
		stats.query_count.equivalence++;
	} else {
		// send query to client
		if(!sock->stream_send_int(htonl(SM_SES_ACK_ADVANCE_QUERIES)))
			return false;

		knowledgebase<extended_bool> * querytree;
		querytree = knowledge.create_query_tree();
		stats.query_count.uniq_membership += querytree->count_queries();
		blob = querytree->serialize();
		// send query count
		if(!sock->stream_send_int(htonl(querytree->count_queries())))
			return false;
		delete querytree;
	}

	return sock->stream_send_blob(blob);
}}}
bool session::get_query_answer(serversocket * sock)
{{{
	int count;
	int32_t d;
	basic_string<int32_t> blob; // ntohl of this is done in alg->deserialize() !
	basic_string<int32_t>::iterator bi;

	if(!sock->stream_receive_int(d))
		return false;
	blob += d;

	for(count = ntohl(d); count > 0; count--) {
		if(!sock->stream_receive_int(d))
			return false;
		blob += d;
	}

	bi = blob.begin();
	if(!knowledge.deserialize_query_acceptances(bi, blob.end()))
		return false;

	if(!sock->stream_send_int(htonl(SM_SES_ACK_ANSWER_QUERIES)))
		return false;

	return (bi == blob.end());
}}}
bool session::get_counterexamples(serversocket * sock)
{{{
	int wordcount, count;
	int32_t d;

	list<int> word;

	if(!sock->stream_receive_int(d))
		return false;

	for(wordcount = ntohl(d); wordcount > 0; wordcount--) {
		word.clear();

		// receive serialized word
		if(!sock->stream_receive_int(d))
			return false;
		for(count = ntohl(d); count > 0; count--) {
			if(!sock->stream_receive_int(d))
				return false;

			word.push_back(ntohl(d));
		}

		// add it as a counter-example
		alg->add_counterexample(word);
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_COUNTEREXAMPLES)))
		return false;
	return sock->stream_send_int(htonl(1));
}}}
bool session::get_counterexamples_and_answer(serversocket * sock)
{{{
	int wordcount, count;
	int32_t d;

	list<int> word;
	extended_bool answer;

	if(!sock->stream_receive_int(d))
		return false;

	for(wordcount = ntohl(d); wordcount > 0; wordcount--) {
		word.clear();

		// receive serialized word
		if(!sock->stream_receive_int(d))
			return false;
		for(count = ntohl(d); count > 0; count--) {
			if(!sock->stream_receive_int(d))
				return false;

			word.push_back(ntohl(d));
		}

		// answer
		if(!sock->stream_receive_int(d))
			return false;

		answer = (int32_t) ntohl(d);

		// add it as a counter-example
		alg->add_counterexample(word, answer);
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_COUNTEREXAMPLES_AND_ANSWERS)))
		return false;
	return sock->stream_send_int(htonl(1));
}}}
bool session::answer_alphabet_size(serversocket * sock)
{{{
	if(!sock->stream_send_int(htonl(SM_SES_ACK_ALPHABET_SIZE)))
		return false;

	if(alg) {
		if(!sock->stream_send_int(htonl(alg->get_alphabet_size())))
			return false;
	} else {
		if(!sock->stream_send_int(htonl(0)))
			return false;
	}

	return true;
}}}
bool session::set_alphabet_size(serversocket * sock)
{{{
	int32_t d;

	if(!sock->stream_receive_int(d))
		return false;

	d = ntohl(d);

	alg->increase_alphabet_size(d);

	return sock->stream_send_int(htonl(SM_SES_ACK_SET_ALPHABET_SIZE));
}}}
bool session::increase_alphabet_size(serversocket * sock)
{{{
	int32_t d;

	if(!sock->stream_receive_int(d))
		return false;

	d = ntohl(d) + alg->get_alphabet_size();

	alg->increase_alphabet_size(d);

	return sock->stream_send_int(htonl(SM_SES_ACK_INC_ALPHABET_SIZE));
}}}
bool session::answer_stats(serversocket * sock)
{{{
	if(!sock->stream_send_int(htonl(SM_SES_ACK_REQ_STATS)))
		return false;

	alg->get_memory_statistics(stats);

	basic_string<int32_t> blob;
	blob = stats.serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::set_stats(serversocket * sock)
{{{
	int count;
	int32_t d;
	basic_string<int32_t> blob;
	basic_string<int32_t>::iterator bi;

	if(!sock->stream_receive_int(d))
		return false;
	blob += d;

	for(count = ntohl(d); count > 0; count--) {
		if(!sock->stream_receive_int(d))
			return false;
		blob += d;
	}

	bi = blob.begin();
	d = stats.deserialize(bi, blob.end());
	if(!d || bi != blob.end())
		return false;

	return sock->stream_send_int(htonl(SM_SES_ACK_SET_STATS));
}}}
bool session::answer_log_request(serversocket * sock)
{{{
	std::string * s;
	const char * c;
	int l;

	if(!sock->stream_send_int(htonl(SM_SES_ACK_LOG)))
		return false;

	s = logger.receive_and_flush();
	c = s->c_str();
	l = s->length();

	if(!sock->stream_send_int(htonl(l)))
		return false;
	if(!sock->stream_send(c, l))
		return false;

	delete s;

	return true;
}}}
bool session::undo(serversocket * sock)
{{{
	int d;

	// get undo-count
	if(!sock->stream_receive_int(d))
		return false;

	if(alg->supports_sync()) {
		knowledge.undo(ntohl(d));
		d = alg->sync_to_knowledgebase();
	} else {
		logger(LOGGER_WARN, "sessions algorithm does not support undo operations. skipping.\n");
		d = 0;
	}


	if(!sock->stream_send_int(htonl(SM_SES_ACK_UNDO)))
		return false;

	return sock->stream_send_int(d);
}}}
bool session::log_table(serversocket * sock)
{{{
	string s = alg->tostring();
	logger(LOGGER_DEBUG, "%s", s.c_str());
	return sock->stream_send_int(htonl(SM_SES_ACK_LOG_TABLE));
}}}
bool session::normalize_word(serversocket * sock)
{{{
	int count;
	int32_t d;

	list<int> word;
	bool pnf;
	bool bottom = false;

	// get PNF-flag
	if(!sock->stream_receive_int(d))
		return false;
	pnf = ntohl(d);

	// receive serialized word
	if(!sock->stream_receive_int(d))
		return false;
	for(count = ntohl(d); count > 0; count--) {
		if(!sock->stream_receive_int(d))
			return false;

		word.push_back(ntohl(d));
	}

	if(norm) {
		if(pnf)
			word = norm->prefix_normal_form(word, bottom);
		else
			word = norm->suffix_normal_form(word, bottom);
	} else {
		logger(LOGGER_WARN, "you requested a word to be normalized but"
				"there was no normalizer set. returning same word.\n");
	}

	basic_string<int32_t> ret;

	ret += htonl(SM_SES_ACK_NORMALIZE_WORD);
	ret += htonl(bottom ? 1 : 0);
	ret += serialize_word(word);

	return sock->stream_send_blob(ret);
}}}
bool session::log_knowledgebase(serversocket * sock)
{{{
	string s = knowledge.generate_dotfile();
	logger(LOGGER_DEBUG, "%s", s.c_str());
	return sock->stream_send_int(htonl(SM_SES_ACK_LOG_KNOWLEDGE_DOTFILE));
}}}

