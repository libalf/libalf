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

#ifndef __libalf_protocol_h__
# define __libalf_protocol_h__

#define DISPATCHER_DEFAULT_PORT 23005

enum client_message {
	CM_DISCONNECT = 0,
	CM_REQ_CAPA = 1,
	CM_REQ_LOG = 2,
	CM_STARTTLS = 10,
	CM_REQ_SESSION = 20,
	CM_SES_SET_MODALITIES = 21,
	CM_SES_REQ_STATUS = 22,
	CM_SES_SET_STATUS = 23,
	CM_SES_CONJECTURE = 24,
	CM_SES_ADVANCE = 25,
	CM_SES_ANSWER_SQT = 27,
	CM_SES_GIVE_COUNTEREXAMPLES = 28,
	CM_SES_REQ_STATS = 30
};

enum server_message {
	SM_ACK_DISCONNECT = 0,
	SM_ACK_CAPA = 1,
	SM_ACK_LOG = 2,
	SM_ACK_STATTLS = 10,
	SM_ACK_SESSION = 20,
	SM_SES_ACK_MODALITIES = 21,
	SM_SES_ACK_REQ_STATUS = 22,
	SM_SES_ACK_SET_STATUS = 23,
	SM_SES_ACK_CONJECTURE = 24,
	SM_SES_ACK_ADVANCE_AUTOMATA = 25,
	SM_SES_ACK_ADVANCE_SQT = 26,
	SM_SES_ACK_ANSWER_SQT = 27,
	SM_SES_ACK_COUNTEREXAMPLES = 28,
	SM_SES_ACK_STATS = 30
};

enum capability_type {
	CAPA_STARTTLS = 0
};

enum modality_type {
	MODALITY_TOTAL_ORDER_FUNC = 0,
	MODALITY_SET_LOGLEVEL = 1,
	MODALITY_SET_LOG_ALGORITHM = 2
};

#endif // __libalf_protocol_h__


