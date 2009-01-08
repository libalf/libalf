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
	latest_query = NULL;
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
	latest_query = NULL;
	norm = NULL;
	switch (algorithm) {
		case learning_algorithm<extended_bool>::ALG_ANGLUIN:
			alg = new angluin_simple_observationtable<extended_bool>(NULL, &logger, alphabet_size);
			hypothesis_automaton = new deterministic_finite_amore_automaton;
			logger(LOGGER_INFO, "new session: angluin observationtable\n");
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
cout << "      trying to set now...\n";
	int length;
	basic_string<int32_t>::iterator bi;
	enum normalizer::type type;

	length = blob.size();
	if(length < 2)
		return false;

	// alg->unset_normalizer also deletes the normalizer we are referencing in norm.
	alg->unset_normalizer();

	type = (enum normalizer::type) ntohl(blob[2]);

	switch (type) {
		case normalizer::NORMALIZER_NONE:
			if(length != 2)
				return false;
			// already unset :)
			return true;
		case normalizer::NORMALIZER_MSC:
			norm = new normalizer_msc;
			break;
	}

	bi = blob.begin();
	norm->deserialize(bi, blob.end());

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

		if(!sock->stream_receive_int(d))
			return false;
		type = (enum modality_type)ntohl(d);


		switch(type) {
			case MODALITY_SET_NORMALIZER:
cout << "  MODALITY_SET_NORMALIZER\n";
				if(length < 2) {
					return false;
				}
				blob.push_back(d);
				for(int c = 1; c < length; c++) {
					if(!sock->stream_receive_int(d))
						return false;
					blob.push_back(d);
				}
				if(!this->set_normalizer(blob)) {
cout << "    parsing normalizer failed.\n";
					return false;
				}
				break;
			case MODALITY_EXTEND_NORMALIZER:
cout << "  MODALITY_EXTEND_NORMALIZER\n";
				if(length < 1)
					return false;
				blob.push_back(d);
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
cout << "  MODALITY_SET_LOGLEVEL\n";
				if(length != 1)
					return false;
				if(!sock->stream_receive_int(d))
					return false;
				d = ntohl(d);
				logger.set_minimal_loglevel((enum logger_loglevel)d);
				break;
			case MODALITY_SET_LOG_ALGORITHM:
cout << "  MODALITY_SET_LOG_ALGORITHM\n";
				if(length != 1)
					return false;
				if(!sock->stream_receive_int(d))
					return false;
				d = ntohl(d);
				logger.set_log_algorithm(d != 0);
				break;
			default:
cout << "  invalid modality: " << type << ".\n";
				return false;
		}
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_MODALITIES)))
		return false;
	return sock->stream_send_int(htonl(success ? 1 : 0));
}}}
bool session::answer_status(serversocket * sock)
{{{
cout << "session answer_status\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_REQ_STATUS)))
		return false;
	basic_string<int32_t> blob;
	blob = alg->serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::set_status(serversocket * sock)
{{{
cout << "session set_status\n";
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
bool session::answer_conjecture(serversocket * sock)
{{{
cout << "session answer_conjecture\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_CONJECTURE)))
		return false;
	return sock->stream_send_int(htonl( alg->conjecture_ready() ? 1 : 0 ));
}}}
bool session::advance(serversocket * sock)
{{{
cout << "session advance\n";
	basic_string<int32_t> blob;

	if(latest_query)
		delete latest_query;

	latest_query = alg->advance(hypothesis_automaton);

	if(latest_query) {
		// send latest query to client
		if(!sock->stream_send_int(htonl(SM_SES_ACK_ADVANCE_SQT)))
			return false;
		blob = latest_query->serialize();
	} else {
		// send automaton to client
		if(!sock->stream_send_int(htonl(SM_SES_ACK_ADVANCE_AUTOMATON)))
			return false;
		blob = hypothesis_automaton->serialize();
		stats.query_count.equivalence++;
	}

	return sock->stream_send_blob(blob);
}}}
bool session::get_sqt(serversocket * sock)
{{{
cout << "session get_sqt\n";
	int32_t count;
	int32_t d;

	if(!sock->stream_receive_int(d))
		return false;
	count = ntohl(d);

	if(!latest_query) {
		logger(LOGGER_ERROR, "client sent answer for query but there is no active query! trying to ignore.\n");
		for(count = ntohl(d); count > 0; count--)
			if(!sock->stream_receive_int(d))
				return false;
		if(!sock->stream_send_int(htonl(SM_SES_ACK_ANSWER_SQT)))
			return false;
		return sock->stream_send_int(htonl(0));
	}

	basic_string<int32_t> blob;
	basic_string<int32_t>::iterator bi;

	blob += d; // count, in network byte order

	for(count = ntohl(d); count > 0; count--) {
		if(!sock->stream_receive_int(d))
			return false;
		blob += d;
	}
	bi = blob.begin();
	if(!latest_query->deserialize_acceptances(bi, blob.end()))
		return false;
	if(bi != blob.end())
		return false;

	if( ! latest_query->is_answered()) {
		// skip this sqt as something failed
		logger(LOGGER_ERROR, "client sent answers for structured query tree, but SQT seems not to be fully answered!\n");
		if(!sock->stream_send_int(htonl(SM_SES_ACK_ANSWER_SQT)))
			return false;
		return sock->stream_send_int(htonl(0));
	}

	latest_query->set_statistics(&stats);

	if( ! alg->learn_from_structured_query( *latest_query ))
		logger(LOGGER_ERROR, "client sent answer for SQT which seemed to be fine for the SQT, but algorithm can not make any sense from resulting SQT.\n");

	if(!sock->stream_send_int(htonl(SM_SES_ACK_ANSWER_SQT)))
		return false;

	return sock->stream_send_int(htonl(1));
}}}
bool session::get_counterexamples(serversocket * sock)
{{{
cout << "session get_counterexamples\n";
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

		printf("new counter example: ");
		print_word(word);
		printf("\n");

		// add it as a counter-example
		alg->add_counterexample(word);
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_COUNTEREXAMPLES)))
		return false;
	return sock->stream_send_int(htonl(1));
}}}
bool session::get_counterexamples_and_examples(serversocket * sock)
{{{
cout << "session get_counterexamples\n";
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

		printf("new counter example: ");
		print_word(word);
		printf(" answer: %d\n", (int) answer);

		// add it as a counter-example
		alg->add_counterexample(word, answer);
	}

	if(!sock->stream_send_int(htonl(SM_SES_ACK_COUNTEREXAMPLES_AND_ANSWERS)))
		return false;
	return sock->stream_send_int(htonl(1));
}}}
bool session::answer_alphabet_size(serversocket * sock)
{{{
cout << "session answer_alphabet_size\n";
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
cout << "session answer_stats\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_REQ_STATS)))
		return false;

	alg->get_memory_statistics(stats);

	basic_string<int32_t> blob;
	blob = stats.serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::set_stats(serversocket * sock)
{{{
cout << "session set_stats\n";
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
cout << "session answer_log_request\n";
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

