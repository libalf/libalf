/*
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

/* parse.c
 */

#include "parse.h"

#include <stdlib.h> /* for qsort, bsearch */
#include "global.h" /* for pi2s */

/* static arrayofb_array new_bfield()
 * static GRAMMAR new_grammar()
 * static symbol_stack new_symbol_stack()
 * static sentence_stack new_sentence_stack()
 * static t_elem new_node()
 * static int stringcomp()
 * static boole isoperator()
 * static boole isidentifier()
 * static boole isprefix()
 * static GRAMMAR read_grammar()
 * static GRAMMAR init_grammar()
 * static arrayofstring get_list()
 * static arrayofstring init_ident_list() 
 * static arrayofstring init_prefix_list()
 * static arrayofstring init_operator_list()
 * static arrayofb_array init_isinrule()
 * static posint skip_blanks()
 * static posint next_symbol()
 * static string getident()
 * static posint get_type()
 * static REDUCE_RESULT reduce()
 * static void shift()
 * static string getmessage()
 * static void red_err() 
 * static void prec_diag()
 * static void stack_err()
 * static PARSE_RESULT parse()
 */


/** Error messages echoed by the editor */
static char empty_buf[] = "Empty input buffer",
  no_id_lst[] = "Empty identifier list",
  no_op_lst[] = "Empty operator list",
  unexp_sym[] = "Unexpected symbol ",
  miss_op_b[] = "Missing operand before symbol ",
  miss_op_a[] = "Missing operand after symbol ",
  un_ex_eob[] = "Unexpected end of input buffer",
  unba_ribr[] = "Unbalanced right bracket",
  unba_lebr[] = "Unbalanced left bracket",
  no_val_in[] = "No valid input buffer",
  no_ex_b_p[] = "No expression between parentheses";


static arrayofb_array new_bfield(posint n, posint m) {
  posint i;
  arrayofb_array outfield = newarrayofb_array(n);

  for (i = 0; i < n; ++i)
    outfield[i] = newb_array(m);

  return outfield;
}

/** Allocates memory for structure GRAMMAR
 *  @param n the number of rules in the grammar 
 */

static GRAMMAR new_grammar(posint n) {
  posint i;
  GRAMMAR outgrammar = (GRAMMAR) newbuf(1, sizeof(struct str_grammar));

  outgrammar->ruleno = n;
  outgrammar->rules = (RULE*) newbuf(n, sizeof(RULE));
  for (i = 0; i < n; ++i)
    (outgrammar->rules)[i] = (RULE) newbuf(1, sizeof(struct str_rule));

  return outgrammar;
}

static symbol_stack new_symbol_stack(char op, posint p, posint l) {
  symbol_stack outstack =
    (symbol_stack) newbuf(1, sizeof(struct str_symbol_stack));

  outstack->op   = op;
  outstack->position   = p;
  outstack->line       = l;

  return outstack;
}

static sentence_stack new_sentence_stack(char op, 
                                         string s, 
                                         posint p, 
                                         posint l) {
  sentence_stack outstack =
    (sentence_stack) newbuf(1, sizeof(struct str_sentence_stack));

  outstack->op   = op;
  outstack->symbols    = s;
  outstack->position   = p;
  outstack->line       = l;

  return outstack;
}



/** Create tree element with n sons 
 */
static t_elem new_node(posint n) {
  t_elem hlp = newbuf_t_elem();
  
  hlp->sons_no = n;
  if (n > 0) hlp->son = newbuf_t_ar(n);

  return hlp;
}


/* Compare-function for qsort and bsearch */
/* Compares two strings using strcmp */

static int stringcomp(arg1, arg2)
     string *arg1;
     string *arg2;
{
  return strcmp(*arg1, *arg2);
}


/** Returns TRUE iff (*line)->symbols[*p] is an entry in operator_list.
 *  The function uses bsearch, thus operator_list must be sorted 
 */

static boole isoperator(symb, operator_list, maxop)
     char symb;
     arrayofstring operator_list;
     posint maxop;
{
  string key = newstring(2);
  key[0] = symb;

  return (boole) (bsearch(&key, operator_list, maxop, sizeof(string),
			  stringcomp) != NULL);
}


/** Returns TRUE iff s is an entry in ident_list
 * The function uses bsearch, thus ident_list must be sorted 
 */

static boole isidentifier(s, ident_list, maxid)
     string s;
     arrayofstring ident_list;
     posint maxid;
{
  return (boole) (bsearch(&s, ident_list, maxid, sizeof(string),
			  stringcomp) != NULL);
}

/** Returns TRUE iff s is an entry in prefix_list 
 *  The function uses bsearch, thus prefix_list must be sorted 
 */

static boole isprefix(s, prefix_list, maxpref)
     string s;
     arrayofstring prefix_list;
     posint maxpref;
{
  if (prefix_list == NULL) return FALSE;
  else return (boole) (bsearch(&s, prefix_list, maxpref, sizeof(string),
			       stringcomp) != NULL);
}


/* Fills a structure GRAMMAR from an array of strings of format A->B|C|..
 * n is the number of rules of the grammar (not the number of the strings 
 * in the array!!), startsym is the startsymbol of the grammar 
 */

static GRAMMAR read_grammar(posint n, 
                            char startsym, 
                            arrayofstring strar) {
  posint i = 0,
    sc = 0,
    j, k, l, sl;
  string r, s, t;
  GRAMMAR outgrammar = new_grammar(n);

  if (n <= 0) return NULL;                  /* No rules */
  do
    {
      s = strar[sc];
      sl = strlen(s);
      j = 0;
      while (((s[j] != '-') || (s[j + 1] != '>')) && (j < (sl - 2))) ++j;
      /* Look for -> */
      if (j == (sl - 2)) return NULL;        /* No right side for rule */
      t = newstring(j + 1);                
      for (k = 0; k < j; ++k) t[k] = s[k];   /* t is left side of a rule */
      j += 2;
      do                                     /* Look for right sides for t */
	{
	  k = j;
	  while ((s[j] != '\0') && (s[j] != '|')) ++j;
	  if ((j - k) == 0) return NULL;      /* No right side for rule */
                                             
	  r = newstring(j - k + 1);          /* Right side found */
	  l = 0;
	  while (k < j) { r[l] = s[k]; ++l; ++k;};
	  outgrammar->rules[i]->leftside = t;
	  outgrammar->rules[i]->rightside = r;
	  ++i;
	  if (s[j] == '|') ++j;               /* Look for further right sides */
	} while (s[j] != '\0');
      ++sc;
    } while (i < n);

  outgrammar->startsymbol = startsym; 
  return outgrammar;
}

/** Initializes the grammar 
 */
static GRAMMAR init_grammar(arrayofstring rules, 
                            char startsymbol, 
                            posint maxrule) {
  GRAMMAR outgrammar; 
  
  outgrammar = read_grammar(maxrule, startsymbol, rules);
  
  return outgrammar;
}

/* Function for init_ident_list and init_operator_list */
/* Builds an array of strings from a single textline s*/
/* A space separates the substrings in the textline */
/* Upon termination, maxlen contains the length of the largest substring in s */

static arrayofstring get_list(s, count, maxlen)
     string s;
     posint *count;
     posint *maxlen;
{
  posint   i = 0,
    hcount = 0,
    max = 0,
    chcount, j;
  arrayofstring outstrar;

  do                /* Compute the number of substrings in s */
    {
      while (s[i] == spacech) ++i;  /* Look for the first non-space character */
      if (s[i] != 0)                /* End of line not yet reached */
	{
	  ++hcount;                   /* We have found a valid substring */
	  while ((s[i] != spacech) && (s[i] != 0)) ++i;
	  /* Skip the found substring */
	}
    } while (s[i] != 0);
  if (hcount == 0) return NULL;     /* No substring found */
   
  (*count) = hcount;
  outstrar = newarrayofstring(hcount);     /* Get memory for string-array */
  i = 0;
  hcount = 0;
  do
    {
      while (s[i] == spacech) ++i;  /* Go to the first substring */
      if (s[i] != 0)                /* While end of line not reached */
	{
	  chcount = i;
	  while ((s[i] != spacech) && (s[i] != 0)) ++i;
	  max = (max <= (i - chcount)) ? (i - chcount) : max;  
	  /* Compute the maximal length of a */
	  /* substring in s */
	  outstrar[hcount] = newstring(i - chcount + 1);
	  /* Get sufficient memory for */
	  /* the substring */
	  for (j = chcount; j < i; ++j) outstrar[hcount][j - chcount] = s[j];
	  /* GOTCHA!! */
	  ++hcount;
	}
    } while (s[i] != 0);
  (*maxlen) = max;
  return outstrar;
}


/* Initializes list of identifiers */
/* The funtion uses get_list to divide a single textline into substrings */
/* The array of substrings is sorted by qsort */

static arrayofstring init_ident_list(id_list, maxid, maxidlen)
     string id_list;
     posint *maxid;
     posint *maxidlen;
{
  arrayofstring outlist = get_list(id_list, maxid, maxidlen);

  qsort(outlist, (*maxid), sizeof(string), stringcomp);

  return outlist;
}

/* Initialize list of prefixes of the identifiers */
/* The number of found prefixes is returned in maxpref */
/* The list of prefixes is sorted using qsort */

static arrayofstring init_prefix_list(ident_list, maxid, maxpref)
     arrayofstring ident_list;
     posint maxid;
     posint *maxpref;
{
  posint   i, j, k, l, sl,
    count = 0;
  string   s;
  arrayofstring outlist;

  for (i = 0; i < maxid; ++i)      /* Count the number of (real) prefixes */
    if (strlen(ident_list[i]) > (size_t) 1) 
      count += ((int) strlen(ident_list[i]) - 1);
  if (count == 0) return NULL;     /* No real prefix */
  outlist = newarrayofstring(count);
  l = 0;
  for (i = 0; i < maxid; ++i)
    {
      s = ident_list[i];            /* Get the i-th entry in the ident_list */
      sl = strlen(s);
      if (sl > 1)                   /* If there exists a real prefix */
	for (j = 0; j < (sl - 1); ++j)
	  {                          /* Put all real prefixes into the outlist */
            outlist[l] = newstring(j + 2);
            for (k = 0; k <= j; ++k) outlist[l][k] = s[k];
            ++l;
	  }
    }
  (*maxpref) = count;
  qsort(outlist, count, sizeof(string), stringcomp);

  return outlist;
}


/* Initializes list of operators */
/* The funtion uses get_list to divide a single textline into substrings */
/* The array of substrings is sorted by qsort */

static arrayofstring init_operator_list(op_list, maxop)
     string op_list;
     posint *maxop;
{
  posint dummy;  /* Only used to avoid a warning from the compiler */
  arrayofstring outlist = get_list(op_list, maxop, &dummy);

  qsort(outlist, (*maxop), sizeof(string), stringcomp);

  return outlist;
}


/* Initializes a boolean field */
/* isinrule[i][j] == TRUE iff the operator with the index i is */
/* a symbol of the rule j in the grammar gr */

static arrayofb_array init_isinrule(gr, index)
     GRAMMAR gr;
     array index;
{
  posint   i, j;
  string   rightside;
  arrayofb_array outfield = new_bfield(gr->maxop, gr->ruleno);
  
  for (i = 0; i < gr->ruleno; ++i)
    {
      rightside = gr->rules[i]->rightside;
      for (j = 0; j < strlen(rightside); ++j)
	if (isoperator(rightside[j], gr->operator_list, gr->maxop))
	  outfield[index[(int) rightside[j]]][i] = TRUE;
    };

  return outfield;
}


/* Advances the text buffer to the next non-space character */

static posint skip_blanks(pos, line, lno)
     posint pos;
     strstack *line;
     posint *lno;
{
  strstack l = *line;
  posint p = pos;

  while (l->info[p] == spacech)
    {
      if (p == l->lastpos)       /* End of textline reached */
	{
	  l = l->next;            /* Proceed next line */
	  p = 0;
	  ++(*lno);
	}
      else ++p;                  /* Get next character on actual line */
    }
  *line = l;                    /* Change line pointer */
            
  return p;                     /* Return new position */
}

/* Advances the text buffer to the next character */

static posint next_symbol(pos, line, lno)
     posint pos;
     strstack *line;
     posint *lno;
{
  strstack    l           = *line;
  posint      p           = pos;

  if (p < l->lastpos) ++p; /* End of textline not yet reached */
  else
    if (l->next != NULL)       /* End of textline: if a next line exists */
      {
	l = l->next;            /* change to next line, else leave */
	p = 0;                  /* pointers unchanged */
	++(*lno);
      }
  *line = l;                    /*Change line pointer */

  return p;                     /* Return new position */
}


/* Reads an identifier from the text buffer */

static string getident(line, pos, lno, gr)
     strstack *line;
     posint *pos;
     posint *lno;
     GRAMMAR gr;
{
  string   s = newstring(gr->maxidlen + 2);
  /* Allocate maximal space for */
  /* an identifier + 1 for the error- */
  strstack     maxl, hl = (*line);           /* flag identch */
  posint   z, maxp, maxlno,
    maxz = gr->maxidlen + 1,
    hp = (*pos),
    hlno = (*lno);

  for (z = 0;;++z)
    {
      s[z] = hl->info[hp];
      if (isidentifier(s, gr->ident_list, gr->maxid))
	/* Identifier is found */
	{
	  maxz = z;
	  maxp   = hp;                /* Remind the identifier and */
	  maxl  = hl;                 /* and the text pointers */
	  maxlno   = hlno;                     
	}
      if (! isprefix(s, gr->prefix_list, gr->maxpref))
	/* Unknown identifier detected */
	{
	  if (maxz == (gr->maxidlen + 1))
	    {
	      (*pos)   = hp;                /* Change text pointers to error */
	      (*line)  = hl;                /* position */
	      (*lno)   = hlno;
	      s[0]     = s[z];              /* If an error ocurred, the first */
	      s[1]     = identch;           /* character of s is changed to the */
	      /* unexpected symbol and the next is */
	      return s;                     /* changed to the identch */
	    }
	  else
	    {
	      s[maxz + 1] = '\0';
	      (*pos)   = maxp;                /* Change text pointers to error */
	      (*line)  = maxl;                /* position */
	      (*lno)   = maxlno;
	      return s;
	    }
	}
      if (hp == hl->lastpos)      /* End of textline reached */
	{
	  hp = 0;                       /* Process next line */
	  hl = hl->next;
	  ++hlno;
	}
      else ++hp;                       /* Proceed with next symbol */
    }
}


/* Lexical analysis of input line (number lno) at position pos */
/* operator_list is the list of valid operators, ident_list is the */
/* list of valid identifiers */
/* Return: new text position */

static posint get_type(lasttype, elem, pos, line, lno, gr)
     char lasttype;
     tbuf_elem *elem;
     posint pos;
     strstack *line;
     posint *lno;
     GRAMMAR gr;
{
  strstack    l           = *line;
  posint      p           = pos;
  tbuf_elem   outtbufelem = *elem;
  char        type;
  string      ident;

  p = skip_blanks(p, &l, lno);  /* Don't care about spaces */

  outtbufelem->position   = p;
  outtbufelem->line       = (*lno);
  if (isoperator(l->info[p], gr->operator_list, gr->maxop))
    {                             /* An operator was found */
      /* (all operators have a length of one) */
      outtbufelem->symbols = newstring(2);
      if  (((l->info[p] == leftbrch)  ||  (l->info[p] == complch)) &&
	   ((lasttype == identch)   ||       (lasttype == rightbrch) ||
	    (lasttype == plusch)    ||       (lasttype == starch)))
	type = concatch;        /* A few rules to avoid typing many .'s */
      /* No advancing to the next symbol here !! */
      else
	{
	  type = l->info[p];
	  p = next_symbol(p, &l, lno);
	}   
      outtbufelem->type       = type;
      outtbufelem->symbols[0] = type;
    }
  else
    {                             /* It's not an operator, thus it's an */
      /* identifier */
      /* (It's not a trick, it's a SONY) */
      if ((lasttype == identch) || (lasttype == rightbrch) ||
          (lasttype == plusch) || (lasttype == starch))
	{                          /* Fill in missing '.' */
	  outtbufelem->type       = concatch;
	  outtbufelem->symbols    = newstring(2);
	  outtbufelem->symbols[0] = concatch;
	  /* No advancing to the next symbol here !! */
	}
      else
	{
	  outtbufelem->type = identch;
	  ident = getident(&l, &p, lno, gr);
	  if (ident[1] == identch)   /* Unknown identifier */
	    {
	      outtbufelem->type       = ident[0];
	      outtbufelem->symbols    = NULL;        /* Error-flag for */
	      /* unexpected symbols */
	      outtbufelem->position   = p;
	      outtbufelem->line       = (*lno);
	    }
	  else
	    {
	      outtbufelem->symbols = ident; /* ident is a valid identifier */
	      p = next_symbol(p, &l, lno);  /* Advance text pointers to the */
	      /* next symbol */
	    }
	}
    }
  *line = l;                    /* Change line pointer */

  return p;                     /* Return new position */
}

/* Finds a handle on the symbol stack (according to the precedence */
/* relation) and then tries to find a right side of a rule in the */
/* grammar gr, that contains the handle. */
/* Finally, the function tries to find the rule on the sentence stack */
/* an to reduce it there to the left side of the found rule */
/* If any of these operations fails, an error (return structure nonempty) */
/* is returned */

static REDUCE_RESULT reduce(top_symbol, top_sentence, gr, index, precedence)
     symbol_stack *top_symbol;
     sentence_stack *top_sentence;
     GRAMMAR gr;
     array index;
     array *precedence;
{
  string         handle, rightside;
  symbol_stack   new_top, rightmost,
    sy_elem     = (*top_symbol)->prev;
  sentence_stack s_elem, up, down;
  char           pretype     = sy_elem->op;
  posint         synchr, r, i, j, errorpos, nonterm,
    lenhandle   = 1,
    nextindex   = index[(int)(*top_symbol)->op],
    preindex    = index[(int)pretype];
  REDUCE_RESULT  outres      = newbuf_reduce_result();
  t_elem         hlp;

  while (precedence[preindex][nextindex] == EQUAL)
    {                                      /* Get the size of the handle */
      ++lenhandle;
      nextindex   = preindex;
      sy_elem     = sy_elem->prev;
      pretype     = sy_elem->op;
      preindex    = index[(int)pretype];
    }
  handle = newstring(lenhandle + 1);    /* Copy the handle in the string */
  /* 'handle' */
  new_top = sy_elem;
  for (sy_elem = sy_elem->next, i = 0; sy_elem != (*top_symbol);
       sy_elem = sy_elem->next, ++i)
    handle[i] = sy_elem->op;
  handle[i] = (*top_symbol)->op;

  for (r = 0; r < gr->ruleno; ++r)
    {                       /* Look for a rule that contains all symbols */
      /* of the handle */
      for (i = 0; i < lenhandle; ++i)
	if (! gr->isinrule[index[(int)handle[i]]][r]) break;

      if (i == lenhandle)  /* We have found such a rule */
	{
	  rightside = gr->rules[r]->rightside;
	  rightmost = (*top_symbol); /* Rightmost character of handle */
	  i = strlen(rightside) - (size_t) 1;
	  while (rightside[i] != rightmost->op) --i;
	  s_elem    = (*top_sentence);
	  while (! ((s_elem->op == rightmost->op) &&
		    (s_elem->position == rightmost->position) &&
		    (s_elem->line     == rightmost->line)))
            s_elem = s_elem->prev;

	  /* Situation: i is now the position of the rightmost character */
	  /* of the handle in the right side of the matching rule */
	  /* s_elem points to the corresponding element of the */
	  /* sentence stack */ 
	  /* Now: go downwards from i in rightside and downwards from */
	  /* s_elem in the sentence stack as long as both operators */
	  /* match or the beginning of rightside is found, */
	  /* then scan upwards from i in rightside and upwards from */
	  /* s_elem in the sentence stack */

	  down = s_elem->prev;
	  for (j = i; j > 0; --j, down = down->prev)
            if (down->op != rightside[j - 1]) break;
	  down = down->next;
	  if (j > 0)           
	    {              /* The downward comparison failed */
	      errorpos = j - 1;
	      synchr = i;
	      continue;
	    }

	  j = i;
	  up = s_elem;
	  while ((up->next != NULL) && (j < strlen(rightside)) &&
		 (up->op == rightside[j]))
	    {
	      ++j;
	      up = up->next;
	    }
	  if (up->op != rightside[j])   
	    {              /* The upward comparison failed */
	      errorpos = j;
	      synchr = i;
	      continue;
	    }
	  if (j < (strlen(rightside) - (size_t) 1))
	    {              /* Only a real prefix of the rule was found */
	      errorpos = j + 1;
	      synchr = i;
	      continue;
	    }
	  break;
	}
    }
   
  if (r == gr->ruleno)             /* No rule contains all symbols of */
    {                                /* the handle */
      outres            = newbuf_reduce_result();  /* This structure will */
      outres->rule      = rightside;            /* be an input for the */
      outres->synchr    = synchr;               /* procedure red_err, which */
      outres->s_elem    = s_elem;               /* handles these reduction */
      outres->errorpos  = errorpos;             /* errors */
      return outres;
    }

  nonterm  = strlen(rightside) - lenhandle; /* Number of nonterminals */
  hlp      = new_node(nonterm);             /* in the rule */
  hlp->op  = gr->rules[r]->leftside[0];

  for (i = 0, s_elem = down; i < nonterm; s_elem = s_elem->next)
    if (! isoperator(s_elem->op, gr->operator_list, gr->maxop))
      {                                      /* Each nonterminal represents */
	hlp->son[i] = s_elem->node;         /* a son of the new node */
	hlp->son[i]->father = hlp;
	++i;
      }

  hlp->expr         = (i == 0) ? down->symbols : handle; 
  /* i == 0 <=> the rule consist */
  /* only of terminals */
  down->op    = hlp->op;
  down->symbols     = handle;
  down->node        = hlp;
  down->next        = up->next;             
  if (up->next != NULL) (up->next)->prev = down;
  (*top_sentence)   = down;                 /* Reduce the sentence stack */
  (*top_symbol)     = new_top;              /* Reduce the symbol stack */

  return NULL;
}

/* Shift the tbuf_elem elem onto both stacks */
/* Self declaring */

static void shift(elem, top_symbol, top_sentence)
     tbuf_elem elem;
     symbol_stack *top_symbol;
     sentence_stack *top_sentence;
{
  string s = newstring(strlen(elem->symbols) + (size_t)1);
  posint i;

  for (i = 0; i < strlen(elem->symbols); ++i) s[i] = elem->symbols[i];
  (*top_symbol)->next = new_symbol_stack(elem->type, elem->position,
					 elem->line);
  ((*top_symbol)->next)->prev = (*top_symbol);
  (*top_symbol) = (*top_symbol)->next;

  (*top_sentence)->next = new_sentence_stack(elem->type, s, elem->position,
					     elem->line);
  ((*top_sentence)->next)->prev = (*top_sentence);
  (*top_sentence) = (*top_sentence)->next;

  return;
}


/* Build an errormessage of the form 'message in line lno at position pos' */
static string getmessage(message, symbol, pos, lno)
     string message; 
     char symbol; 
     posint pos; 
     posint lno;
{
  string poss, lins, outmess, symbs;
  posint errmsglen, j;
  
  poss = pi2s(pos);
  lins = pi2s(lno);
  errmsglen = strlen(poss) + strlen(lins) + (size_t) 22;
  errmsglen += strlen(message);
  if (symbol != spacech)	
    {
      if (isprint(symbol)) ++errmsglen;
      else 
	{ 
	  symbs = pi2s(symbol);
	  errmsglen += (1 + strlen(symbs));
	}	
    }       
  outmess = newstring(errmsglen + 1);
  strcpy(outmess, message);
  j = strlen(message);
  if (symbol != spacech)
    {
      if (isprint(symbol)) outmess[j++] = symbol;
      else 
	{
	  outmess[j++] = '\\';
	  strcpy(&outmess[j], symbs);
	  j += strlen(symbs);
	}
    }	
  strcpy(&outmess[j], " in line ");
  j += 9;
  strcpy(&outmess[j], lins);
  j += strlen(lins);
  strcpy(&outmess[j], " at position ");
  j += 13;
  strcpy(&outmess[j], poss);

  return outmess;
}

/* Error procedure that is invoked, if a handle could not be reduced */
/* properly, i.e. every rule, that contained the handle, couldn't be found */
/* entirely on the sentence stack */

static void red_err(inres, inred_res,
		    top_sentence)
     PARSE_RESULT inres;
     REDUCE_RESULT inred_res;
     sentence_stack top_sentence;
{
  string   outmessage;
  char     errorsymbol;
  sentence_stack s_elem = inred_res->s_elem;

  inres->error_pos  = s_elem->position;
  inres->error_line = s_elem->line;
  if (inred_res->rule[inred_res->synchr] != rightbrch)
    {
      outmessage  = (inred_res->errorpos < inred_res->synchr) ? miss_op_b :
	miss_op_a;
      errorsymbol = s_elem->op;
    }
  else
    {
      outmessage  = no_ex_b_p;
      errorsymbol = spacech;
    }
  inres->error_message = getmessage(outmessage, errorsymbol,
				    s_elem->position + 1, s_elem->line + 1);  
   
  return;
}


/* Diagnosys procedure for unnormal precedence realations */
/* This procedure depends on the grammar and must be changed */
/* if the grammar is changed */

static void prec_diag(inres, topsymbol, elem, prec)
     PARSE_RESULT inres;
     symbol_stack topsymbol;
     tbuf_elem elem;
     posint prec;
{
  string errormessage;

  switch (prec)
    {
    case UNBALBR:  errormessage = unba_lebr;  /* Unbalanced left bracket */
      inres->error_message =
	getmessage(errormessage, spacech, topsymbol->position + 1,
		   topsymbol->line + 1);
      inres->error_pos = topsymbol->position;
      inres->error_line = topsymbol->line;		
      break;

    case UNBARBR:  errormessage = unba_ribr;  /* Unbalanced left bracket */
      inres->error_message =
	getmessage(errormessage, spacech, elem->position + 1,
		   elem->line + 1);
      inres->error_pos = elem->position;
      inres->error_line = elem->line;
      break;

    case ENDTEXT:  break;                     /* End of the input buffer */
      /* reached, not necessarily */
      /* an error */
    default:; /* Should not happen */
    }
   
  return;
}

/* Error procedure, if the sentence-stack is not properly reduced to the */
/* startsymbol of the grammar */

static void stack_err(inres, top_sentence)
     PARSE_RESULT inres;
     sentence_stack top_sentence;
{
  inres->error_message = no_val_in;
  inres->error_pos = (inres->error_line = 0);
  return;
}


/* Parses the text-buffer inbuf */
/* Input text-buffer inbuf, grammar gr, mapping index, relation */
/* precedence */
/* Returns outres, a pointer to a structure of type str_parse_result */
/* outbuf->tree == NULL iff inbuf was not a word of the grammar gr */
/* if outbuf->tree == NULL, outbuf->message is an errormessage */
/* and outbuf->error_pos is the position of the detected error in inbuf */

static PARSE_RESULT parse(strstack inbuf, 
			  GRAMMAR gr, 
			  array index, 
			  array *precedence) {

  PARSE_RESULT   outres = newbuf_parse_result();
  REDUCE_RESULT  red_res;
  posint         prec, lastpos,
    lno      = 0,
    pos      = 0;
  strstack       endline,
    line     = inbuf;
  tbuf_elem      elem;
  char           lasttype;
  boole          endparse    = FALSE,
    reduction   = FALSE;
  symbol_stack   top_symbol;
  sentence_stack top_sentence;
  
  if (line->info[0] == '\0') 
    {
      outres->tree = NULL;
      outres->error_message = no_val_in;
      return outres;
    }
  newbuf_part(endline);
  endline->info = newstring(2);
  endline->info[0] = endch;     /* Build textline as an endoftext-flag */
  endline->lastpos = 1;
  
  while (line->next != NULL)       /* Append endline to the text */
    line = line->next;
  line->next = endline;
  
  line              = inbuf;       /* A few initializations */
  lasttype          = endch;
  elem              = newbuf_tbuf_elem();
  top_symbol        = new_symbol_stack(endch, 0, 0);
  top_symbol->prev  = top_symbol;
  top_sentence      = new_sentence_stack(endch, "", 0, 0);
  top_sentence->prev = top_sentence;
  do
    {
      if (! reduction)              /* If a reduction was done, the last */
	/* symbol must be processed again */
	{  
	  lastpos = pos;
	  pos = get_type(lasttype, &elem, pos, &line, &lno, gr);
	  if (elem->symbols == NULL) /* An unexpected symbol ocurred */
	    {
	      outres->tree            = NULL;
	      outres->error_message   = (elem->type == endch) ? un_ex_eob :
		getmessage(unexp_sym, elem->type,
			   pos + 1, lno + 1);
	      outres->error_pos       = (elem->type == endch) ? lastpos : pos;
	      outres->error_line      = (elem->type == endch) ? lno - 1 : lno;
	      line = inbuf;
	      while (line->next != endline)     /* Remove endline from the text */
		line = line->next;
	      line->next = NULL;
	      return outres;                
	    }
	}
      prec = precedence[index[(int)top_symbol->op]][index[(int)elem->type]];
      switch (prec)
	{
	case  GREATER  :  red_res =  reduce(&top_symbol, &top_sentence,
					    gr, index, precedence);
	if (red_res == NULL) reduction = TRUE;
	/* reduction must be set to TRUE, because */
	/* the old symbol must be processed again */
	else
	  {  /* The reduction failed, red_err produces */
	    /* the errormessage */   
	    red_err(outres, red_res, top_sentence); 
	    endparse = TRUE;
	  }
	break;
	
	case  EQUAL    :  ;
	case  LESS     :  shift(elem, &top_symbol, &top_sentence);
	  reduction = FALSE;
	  /* reduction is FALSE now, because we need a */
	  /* new symbol from the input */
	  break;
	  
	default        :  prec_diag(outres, top_symbol, elem, prec);
	  /* Not 'normal' precedence relations are */
	  /* processed by prec_diag */
	  endparse = TRUE;
	}
      lasttype = elem->type;
    } while (! endparse);
  
  if (outres->error_message != NULL)
    {
      outres->tree = NULL;
      line = inbuf;
      while (line->next != endline)       /* Remove endline from the text */
	line = line->next;
      line->next = NULL;
      
      return outres;
    }
  if (top_sentence->op == gr->startsymbol)
    {
      outres->tree = top_sentence->node;
    }
  else stack_err(outres, top_sentence);
   
  line = inbuf;
  while (line->next != endline)       /* Remove endline from the text */
    line = line->next;
  line->next = NULL;
   
  return outres;

}

/** returns null on error */

PARSE_RESULT parser(PARSE_INPUT instruct) {

  GRAMMAR        gr;
  PARSE_RESULT   res = NULL;

  gr                = init_grammar(instruct->rules, instruct->startsymbol,
				   instruct->maxrule);
  gr->maxidlen      = 0;  
  gr->ident_list    = init_ident_list(instruct->id_list, &(gr->maxid),
				      &(gr->maxidlen));
  gr->prefix_list   = init_prefix_list(gr->ident_list, gr->maxid,
				       &(gr->maxpref));
  gr->operator_list = init_operator_list(instruct->op_list, &(gr->maxop));
  gr->isinrule      = init_isinrule(gr, instruct->index);


  if (instruct->buf == NULL)                     /* No input text */    
    {
      res = newbuf_parse_result();
      res->error_message = empty_buf;
    }
  if (gr->ident_list == NULL)          /* Empty identifier list */
    {
      res = newbuf_parse_result();
      res->error_message = no_id_lst;
    }
  if (gr->operator_list == NULL)       /* Empty operator list */
    {
      res = newbuf_parse_result();
      res->error_message = no_op_lst;
    }
  
  if (res == NULL) res = parse(instruct->buf, gr, instruct->index,
			       instruct->precedence);
  else res->error_pos = (res->error_line = 0);
  
  return res;
}
