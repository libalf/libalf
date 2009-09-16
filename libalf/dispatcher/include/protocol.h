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

#ifndef __libalf_dispatcher_protocol_h__
# define __libalf_dispatcher_protocol_h__

#define DISPATCHER_PROTOCOL_VERSION 1

#define DISPATCHER_DEFAULT_PORT 24940

enum command_error_code {
	ERR_SUCCESS = 0,
	ERR_NO_OBJECT = 1,
	ERR_BAD_OBJECT = 2,
	ERR_BAD_OBJECT_STATE = 3,

	ERR_BAD_COMMAND = 10,
	ERR_NOT_IMPLEMENTED = 11,

	ERR_BAD_PARAMETER_COUNT = 15,
	ERR_BAD_PARAMETERS = 16,

	ERR_COMMAND_FAILED = 20,

	ERR_UNRESOLVED_REFERENCES_REMOVED = 40,
	ERR_REMAINING_OBJECTS = 41,

	ERR_AUTHENTICATION_REQUIRED = 100,
	ERR_AUTHENTICATION_FAILED = 101,

	ERR_TLS_REQUIRED = 110,

	ERR_OUT_OF_MEM = 1000,
	ERR_INTERNAL_ERROR = 1001,
};

inline const char* err2string(int errno)
{{{
	switch(errno) {
		case 0:
			return "success";
		case 1:
			return "object does not exist";
		case 2:
			return "bad object";
		case 3:
			return "object is in bad state";
		case 10:
			return "bad command";
		case 11:
			return "command not implemented";
		case 15:
			return "bad parameter count";
		case 16:
			return "bad parameters";
		case 20:
			return "command failed";
		case 40:
			return "unresolved references removed during deletion";
		case 41:
			return "disconnecting with remaining objects";
		case 100:
			return "authentication required";
		case 101:
			return "authentication failed";
		case 110:
			return "TLS required";
		case 1000:
			return "out of memory";
		case 1001:
			return "internal error";
		default:
			return "unknown error";
	};
}}}

enum client_command {
	CLCMD_REQ_CAPA = 0,
	CLCMD_REQ_VERSION = 1,

	CLCMD_DISCONNECT = 10,
	CLCMD_STARTTLS = 11,
	CLCMD_AUTH = 12,

	CLCMD_CREATE_OBJECT = 20,
	CLCMD_DELETE_OBJECT = 21,
	CLCMD_GET_OBJECTTYPE = 22,
	CLCMD_OBJECT_COMMAND = 23,
	CLCMD_COUNT_DISPATCHER_REFERENCES = 30,
	/*
	CLCMD_ENABLE_CLIENT_REFERENCING = 35,
	CLCMD_DISABLE_CLIENT_REFERENCING = 36,
	CLCMD_COUNT_CLIENT_REFERENCES = 37,
	CLCMD_INCREASE_CLIENT_REFERENCES = 38,
	CLCMD_DECREASE_CLIENT_REFERENCES = 39,
	*/

	CLCMD_HELLO_CARSTEN = 99,

	// reserved for extensions: >= 1000
};

inline const char* cmd2string(int cmd)
{{{
	if(cmd == 0)
		return "request server capabilities";
	if(cmd == 1)
		return "request server version";

	if(cmd == 10)
		return "disconnect";
	if(cmd == 11)
		return "STARTTLS";
	if(cmd == 12)
		return "authenticate";

	if(cmd == 20)
		return "create object";
	if(cmd == 21)
		return "delete object";
	if(cmd == 22)
		return "get object_type";
	if(cmd == 23)
		return "object command";
	if(cmd == 30)
		return "count dispatcher references";
	/*
	if(cmd == 35)
		return "enable client-referencing framework";
	if(cmd == 36)
		return "disable client-referencing framework";
	if(cmd == 37)
		return "count client references";
	if(cmd == 38)
		return "decrease client references";
	if(cmd == 39)
		return "increase client references";
	*/

	if(cmd == 99)
		return "say hello to carsten";

	return "invalid";
}}}

enum object_type {
	OBJ_NONE = -1,
	OBJ_LOGGER = 0,
	OBJ_KNOWLEDGEBASE = 10,
	OBJ_KNOWLEDGEBASE_ITERATOR = 11,
	OBJ_LEARNING_ALGORITHM = 20,
	OBJ_NORMALIZER = 30,
};

inline const char* obj2string(int type)
{{{
	if(type == -1)
		return "OBJ_NONE";
	if(type == 0)
		return "logger";
	if(type == 10)
		return "knowledgebase";
	if(type == 11)
		return "knowledgebase::iterator";
	if(type == 20)
		return "learning_algorithm";
	if(type == 30)
		return "normalizer";
	return "invalid";
}}}

enum logger_command {
	LOGGER_RECEIVE_AND_FLUSH = 0,
	LOGGER_SET_MIN_LOGLEVEL = 1,
	LOGGER_LOG_ALGORITHM = 2,
	LOGGER_NOT_LOG_ALGORITHM = 3,
};

enum knowledgebase_command {
	KNOWLEDGEBASE_SERIALIZE = 0,
	KNOWLEDGEBASE_DESERIALIZE = 1,
	KNOWLEDGEBASE_ASSIGN = 2,

	KNOWLEDGEBASE_TO_DOTFILE = 5,
	KNOWLEDGEBASE_TO_STRING = 6,

	KNOWLEDGEBASE_IS_ANSWERED = 10,
	KNOWLEDGEBASE_IS_EMPTY = 11,
	KNOWLEDGEBASE_GET_ALPHABET_SIZE = 12,
	KNOWLEDGEBASE_COUNT_QUERIES = 13,
	KNOWLEDGEBASE_COUNT_ANSWERS = 14,
	KNOWLEDGEBASE_COUNT_RESOLVED_QUERIES = 15,
	KNOWLEDGEBASE_GET_MEMORY_USAGE = 16,

	KNOWLEDGEBASE_RESOLVE_QUERY = 20,
	KNOWLEDGEBASE_RESOLVE_OR_ADD_QUERY = 21,
	KNOWLEDGEBASE_ADD_KNOWLEDGE = 22,

	KNOWLEDGEBASE_GET_QUERY_TREE = 25,
	KNOWLEDGEBASE_MERGE_TREE = 26,
	KNOWLEDGEBASE_DESERIALIZE_QUERY_ACCEPTANCES = 27,

	// iterator:
	KNOWLEDGEBASE_BEGIN = 30,
	KNOWLEDGEBASE_END = 31,
	KNOWLEDGEBASE_QBEGIN = 32,
	KNOWLEDGEBASE_QEND = 33,

	KNOWLEDGEBASE_CLEAR = 40,
	KNOWLEDGEBASE_CLEAR_QUERIES = 41,

	KNOWLEDGEBASE_UNDO = 50,
};

enum knowledgebase_iterator_command {
	KITERATOR_IS_VALID = 0,
	KITERATOR_ASSIGN = 1,
	KITERATOR_COMPARE = 2,

	KITERATOR_IS_ANSWERED = 10,
	KITERATOR_IS_REQUIRED = 11,

	KITERATOR_GET_WORD = 20,
	KITERATOR_GET_ANSWER = 21,

	KITERATOR_NEXT = 30,
	KITERATOR_ANSWER = 31,
};

enum algorithm_command {
	LEARNING_ALGORITHM_SERIALIZE = 0,
	LEARNING_ALGORITHM_DESERIALIZE = 1,
	LEARNING_ALGORITHM_TO_STRING = 2,
	LEARNING_ALGORITHM_DESERIALIZE_MAGIC = 3,

	LEARNING_ALGORITHM_ASSOCIATE_LOGGER = 10,
	LEARNING_ALGORITHM_REMOVE_LOGGER = 11,
	LEARNING_ALGORITHM_SET_KNOWLEDGE_SOURCE = 12,
	LEARNING_ALGORITHM_GET_KNOWLEDGE_SOURCE = 13,
	LEARNING_ALGORITHM_SET_NORMALIZER = 14,
	LEARNING_ALGORITHM_GET_NORMALIZER = 15,
	LEARNING_ALGORITHM_UNSET_NORMALIZER = 16,

	LEARNING_ALGORITHM_GET_MEMORY_STATISTICS = 20,
	LEARNING_ALGORITHM_GET_TIMING_STATISTICS = 21,
	LEARNING_ALGORITHM_ENABLE_TIMING = 22,
	LEARNING_ALGORITHM_DISABLE_TIMING = 23,
	LEARNING_ALGORITHM_RESET_TIMING = 24,

	LEARNING_ALGORITHM_GET_ALPHABET_SIZE = 30,
	LEARNING_ALGORITHM_INCREASE_ALPHABET_SIZE = 31,

	LEARNING_ALGORITHM_CONJECTURE_READY = 40,
	LEARNING_ALGORITHM_ADVANCE = 41,
	LEARNING_ALGORITHM_ADD_COUNTEREXAMPLE = 42,

	LEARNING_ALGORITHM_SUPPORTS_SYNC = 50,
	LEARNING_ALGORITHM_SYNC_TO_KNOWLEDGEBASE = 51,
};

enum normalizer_command {
	NORMALIZER_SERIALIZE = 0,
	NORMALIZER_DESERIALIZE = 1,
	NORMALIZER_DESERIALIZE_EXTENSION = 2,

	NORMALIZER_GET_TYPE = 10,

	NORMALIZER_NORMALIZE_A_WORD_PNF = 20,
	NORMALIZER_NORMALIZE_A_WORD_SNF = 21,
};

#endif // __libalf_dispatcher_protocol_h__


