/** \file
 *  \brief parser data structures
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *  This file is part of the libAMoRE library.
 *
 *  libAMoRE is  free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with the GNU C Library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA.
 */
#ifndef _PARSER_TYPES_H
#define _PARSER_TYPES_H

#include <amore/dfa.h>
#include <amore/nfa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** a tree element
 *  @ingroup PARSER
 */
struct tree_element {
	char op;		/*< Operand */
	char* expr;
	int sons_no;		/*< Number of sons                      */
	struct tree_element **son;	/*< Pointerarray to sons                */
	struct tree_element *father;	/*< Pointer to father                   */
	posint son_passed,	/*< Last son, that was already visited  */
	 q_no,			/*< Number of states of automaton       */
	 init;			/*< Initial state of automaton          */
	boole is_reg_ab;	/*< TRUE <=> no insecch or complch above node */
	dfa da;			/*< Pointer to dfa */
	nfa na;			/*< Pointer to nfa */
};

/** pointer to a tree element
 *  @ingroup PARSER
 */
typedef struct tree_element *t_elem;

/** a rule
 *  @ingroup PARSER
 */
struct str_rule {
	char* leftside;
	char* rightside;
};

/** pointer to a rule
 *  @ingroup PARSER
 */
typedef struct str_rule *RULE;

/** no doc
 *  @ingroup PARSER
 */
struct str_idgroups {
	char* FirstSyms;	/*< First symbols of the identifier(-group) */
	boole FollByNum;	/*< TRUE iff identifier of the group consist of
				   FirstSyms+ number */
	char TypeSymbol;	/*< Symbol representing the (group of) identifier(s)
				   in the grammar */
};

/** pointer to a str_idgroups structure
 *  @ingroup PARSER
 */
typedef struct str_idgroups IdGroup;

/** grammar
 *  @ingroup PARSER
 */
struct str_grammar {
	posint ruleno;
	char startsymbol;
	RULE *rules;
	IdGroup *fident_list;
	array_of_c_string ident_list;
	array_of_c_string operator_list;
	array_of_c_string prefix_list;
	posint maxid;
	posint maxidlen;
	posint maxop;
	posint maxpref;
	arrayofb_array isinrule;
};

/** pointer to a str_grammar structure
 *  @ingroup PARSER
 */
typedef struct str_grammar *GRAMMAR;

/** part of a reg. expr.
 *  @ingroup PARSER
 */
struct part {
	int lastpos;		/*< 0 to lastpos-1 in info used */
	struct part *prev, *next;
	char *info;		/*< string of information */
};
/** pointer to a part structure
 *  @ingroup PARSER
 */
typedef struct part *strstack;

/** result of parsing.
 *  iff parsing fails, tree == NULL and there is an error message
 *  @ingroup PARSER
 */
struct str_parse_result {
	t_elem tree;		/* Root of syntax-tree regular exp. */
	char* error_message;	/* Error message iff tree == NULL */
	posint error_pos;	/* Position of error */
	posint error_line;	/* Line in which error ocurred */
};
/** pointer to a str_parse_result
 */
typedef struct str_parse_result *PARSE_RESULT;

/** undocumented
 *  @ingroup PARSER
 */
struct str_tbuf_elem {
	char type;
	char* symbols;
	posint position;
	posint line;
	t_elem node;
};
/** undocumented
 *  @ingroup PARSER
 */
typedef struct str_tbuf_elem *tbuf_elem;

/** undocumented
 *  @ingroup PARSER
 */
struct str_symbol_stack {
	char op;
	posint position;
	posint line;
	struct str_symbol_stack *prev;
	struct str_symbol_stack *next;
};
/** undocumented
 *  @ingroup PARSER
 */
typedef struct str_symbol_stack *symbol_stack;

/** undocumented
 *  @ingroup PARSER
 */
struct str_sentence_stack {
	char op;
	char* symbols;
	posint position;
	posint line;
	struct str_sentence_stack *prev;
	struct str_sentence_stack *next;
	t_elem node;
};
/** undocumented
 *  @ingroup PARSER
 */
typedef struct str_sentence_stack *sentence_stack;

/** undocumented
 *  @ingroup PARSER
 */
struct str_reduce_result {
	char* rule;
	posint synchr;
	sentence_stack s_elem;
	posint errorpos;
};
/** undocumented
 *  @ingroup PARSER
 */
typedef struct str_reduce_result *REDUCE_RESULT;

/** input to the parser
 *  @ingroup PARSER
 */
struct str_parse_input {
  /** List of operators */
	char* op_list;
  /** List of identifiers */
	IdGroup *fid_list;
  /** List of identifiers */
	char* id_list;
	posint maxid;
  /** Stringarray with the rules of the grammar */
	array_of_c_string rules;
  /** Number of rules in the grammar */
	posint maxrule;
  /** Startsymbol of the grammar */
	char startsymbol;
  /** Precedence relation */
	array_of_int_array precedence;
  /** Input, that shall be parsed */
	strstack buf;
  /** Mapping {operators}->|{operators}| */
	array_of_int index;
};

/** pointer to str_parse_input
 *  @ingroup PARSER
 */
typedef struct str_parse_input *PARSE_INPUT;

/** undocumented (string expression)
 *  @ingroup PARSER
 */
struct str_express {
	char* expr;
	posint length;
};

/** undocumented (pointer to str_express)
 *  @ingroup PARSER
 */
typedef struct str_express *EXPRESS;

/** \defgroup PARSE_ALLOC Macros for Memory Allocation
 *  \ingroup PARSER
 *  @{
 */
/*! undocumented */
#define newbuf_parse_input() ((PARSE_INPUT) newbuf(1, sizeof(struct str_parse_input)))

/*! undocumented */
#define newbuf_array(N) ((array_of_int) newbuf(N, sizeof(posint)))

/*! undocumented */
#define newbuf_strar(N) ((array_of_c_string) newbuf(N, sizeof(char*)))

/*! undocumented */
#define newbuf_string(N) ((char*) newbuf(N, sizeof(char)))

/*! undocumented */
#define newbuf_tbuf_elem() ((tbuf_elem) newbuf(1, sizeof(struct str_tbuf_elem)))

/*! undocumented */
#define newbuf_reduce_result() ((REDUCE_RESULT) newbuf(1, sizeof(struct str_reduce_result)))

/*! new buffer-tree element */
#define newbuf_t_elem() ((t_elem) newbuf(1, sizeof(struct tree_element)))

/*! undocumented */
#define newbuf_t_ar(N) ((t_elem *) newbuf(N, sizeof(t_elem)))

/*! undocumented */
#define newbuf_parse_result() ((PARSE_RESULT) newbuf(1, sizeof(struct str_parse_result)))

/*! undocumented */
#define newbuf_strstack() ((strstack) newbuf(1, sizeof(struct part)))

/*! undocumented */
#define newbuf_part(A)      { A = (strstack)newbuf(1,sizeof(struct part)); }

//@}



/** \defgroup PARSE_CH Characters
 *  \ingroup PARSER
 * @{
 */

#define spacech		' '	/*!< space  */
#define unionch		'U'	/*!< union  */
#define concatch	'.'	/*!< concatenation  */
#define starch		'*'	/*!< Kleene-star  */
/*! Kleene-plus (L<plusch> == L<starch> <minusch> <epsch>  */
#define plusch		'+'
#define complch		'~'	/*!< complement  */
#define insecch		'&'	/*!< intersection  */
#define minusch		'-'	/*!< set difference  */
#define langch		'%'	/*!< the empty set  */
#define epsch		'@'	/*!< the empty word  */
#define endch           '$'	/*!< end  */
#define leftbrch        '('	/*!< left bracket  */
#define rightbrch       ')'	/*!< right bracket  */
#define identch         '?'	/*!< identifier  */
/*@}*/


/** \defgroup PARSE_STRF String form
 *  \ingroup PARSER
 * @{
 */
#define strcomp		"~"	/*!< complement */
#define strunion	"U"	/*!< union */
#define strspaunion	" U "	/*!< <space>union<space> */
#define streps		"@"	/*!< empty word */
#define strspaeps	" @"	/*!< <space> empty word */
#define straddeps 	"@U"	/*!< <epsch><unionch> */
#define strcataddeps	".@U"	/*!< <concatch><epsch><unionch> */
#define stralph		"A*"	/*!< A<starch> */
#define stropalph	"(A*"	/*!< (A<starch> */
#define stralphcl   	"A*)"	/*!< A<starch>) */
#define strinter	" & "	/*!< intersection */
#define strminus	" - "	/*!< set difference */
#define strstar		"*"	/*!< star */
#define strplus		"+"	/*!< plus */
#define strconcat	"."	/*!< concatenation char */
#define emptysetstr	"%"	/*!< string for the empty set */
/*@}*/

/** \defgroup PARSE_PREC Precedence Table Entries
 *  \ingroup PARSER
 * @{
 */
#define  EQUAL          0
#define  LESS           1
#define  GREATER        2
#define  UNUSED         3
#define  UNBALBR        4	/*!< Unbalanced left bracket */
#define  UNBARBR        5	/*!< Unbalanced right bracket */
#define  ENDTEXT        6
/*@}*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif
