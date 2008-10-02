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

/* grexnfa.c 3.0 (KIEL) 11.94 */

#include "genrex2nfa.h"

/***********************************************************
 *                                                         *
 * Convert generalized regular expressions to nfa's        *
 *                                                         *
 * functions included:                                     *
 * nfa genrex2nfa(regex)                                     *
 *                                                         *
 ***********************************************************/
/* hierarchie
 * nfa genrex2nfa()
 *    static t_elem postfix2tree()
 *       static t_elem new_node0()
 *       static t_elem new_node1()
 *       static t_elem new_node2()
 *       static string cutstr()
 *    static void optimize_tree() 
 *    static void mark_tree()
 *    static void reorder_tree()
 *       static void reorder_node()
 *    static void first_run()
 *       static regex makerex()
 *       static dfa plus_star_dfa()
 *       static dfa conc_dfa()
 *       static dfa boole_dfa()
 *         static posint insert_elt()
 *    static void second_run()
 */ 
 
#define alph 'A'
#define regch 'r'
#define boolech 'b'
#define headch ' '
#define newt_elem()             (t_elem)newbuf((posint)1,(posint)sizeof(struct tree_element))
#define newt_ar(A)              (t_elem*)newbuf((posint)A,(posint)sizeof(t_elem))
#define newb_t_elem()           (b_t_elem)newbuf((posint)1,(posint)sizeof(struct bin_tree_element))
#define neww_elem()             (w_elem)newbuf((posint)1,(posint)sizeof(struct waiting_element))

#define is_letter(CH) (('a' <= CH) && (CH <= 'z'))
#define is_digit(D) (('0' <= D) && (D <= '9'))
#define is_leftson(N) (N->father->son[0] == N)
#define searchnoson(V) while (V->sons_no != 0) V = V->son[0]
#define godown(V) while (V->op == op) v = v->son[0]
#define passps(V) while ((V->op == plusch) || (V->op == starch)) V = V->son[0]
#define searchcil(V) while (!(((V->op == boolech) &&\
                               (V->expr[0] != unionch)) ||\
                              (V->sons_no == 0))) V = V->son[0]
#define searchreg(V) while (V->op != regch) V = V->son[0]
#define searchauto(V) while ((V->na == NULL) && (V->da == NULL)) V = V->son[0]
#define epstrans(F, T, B, E)\
  for (s = 1; s <= S; ++s)\
    for (p = B; p != E; ++p)\
      if testcon(delta, s, T, p) connect(delta, s, F, p);\
  if isfinal(infin[T]) setfinalT(infin[F])
#define copyddelta(ND, Q1, DD, Q2) \
  for (l = 1; l <= sno; ++l) connect(ND, l, Q1, DD[l][Q2]);

#define copyda(DA) \
  for (q = 0; q <= DA->qno; ++q) \
  { if (DA->final[q]) setfinalT(infin[q + offset]); \
    for (s = 1; s <= S; ++s) \
      connect(delta, s, q + offset, DA->delta[s][q] + offset); \
  }
#define copyna(NA) \
  for (p = 0; p <= NA->qno; ++p) \
  { if isfinal(NA->infin[p]) setfinalT(infin[p + offset]); \
    for (s = 1; s <= S; ++s) \
      for (q = 0; q <= NA->qno; ++q) \
        cpdelta(delta, s, p + offset, q + offset, NA->delta, s, p, q); \
  }


typedef struct bin_tree_element
        { array tupel;
          posint no;
          struct bin_tree_element *lson, *rson;
        } *b_t_elem;

typedef struct waiting_element
        { array tupel;
          array delta_s;
          struct waiting_element *next;
        } *w_elem;

typedef struct dfa_array
        { dfa *da;
          posint no;
        } *d_array;

typedef struct stack_element
        { posint first, last;
          t_elem node;
        } *s_elem;
 
typedef s_elem *stack;

posint S;

/***********************************************************
 *                                                         *
 * Functions for postfix2tree                              *
 *                                                         *
 ***********************************************************/

/* Create tree element without sons */
static t_elem new_node0(s)
string s;
{
  t_elem hlp = newt_elem();
  hlp->op = ((strlen(s) == (size_t) 1) && (! is_letter(s[0]))) ? s[0] : regch;
  hlp->expr = s;
  return hlp;
}


/* Create tree element with one son */
static t_elem new_node1(son, z)
char z;
t_elem son;
{
  t_elem hlp = newt_elem();
  hlp->expr = newstring(2);
  hlp->expr[0] = z;
  hlp->op = (z == complch) ? boolech : z;
  hlp->sons_no = 1;
  hlp->son = newt_ar(1);
  hlp->son[0] = son;
  son->father = hlp;
  return hlp;
}


/* Create tree element with two sons */

static t_elem new_node2(lson, rson, z)
char z;
t_elem lson, rson;
{
  t_elem hlp = newt_elem();
  hlp->expr = newstring(2);
  hlp->expr[0] = z;
  hlp->op = (z == concatch) ? z : boolech;
  hlp->sons_no = 2;
  hlp->son = newt_ar(2);
  hlp->son[0] = lson;
  hlp->son[1] = rson;
  lson->father = rson->father = hlp;
  return hlp;
}


 
/* Cut string from f to l out of s*/
 
static string cutstr(s, first, last)
string s;
posint first, last;
{
  string hlp = newstring(last - first + 2);
  posint count = first,
         count1=0;

  while(count<=last)
    hlp[count1++] = s[count++];
  return hlp;
}
 
 
/*                                             *
 * Convert expanded regular expression to tree *
 *                                             */
 
static t_elem postfix2tree(re)
regex re;
{
  posint p,sp = 0;
  posint n=(re->erexl/2)+1;
  string s = re->exprex;
  t_elem lson, rson;
  stack st;
 
  st = (stack)newbuf(n, sizeof(s_elem));
  for (p = 0; p != n; ++p)
     st[p] = (s_elem)newbuf(1,sizeof(struct stack_element));
 
  p=0; 
  while (s[p] != '\0')
  { switch (s[p])
    { case unionch: ;
      case concatch : --sp;
                      if (st[sp - 1]->node == NULL)
                        if (st[sp]->node == NULL) st[sp - 1]->last = st[sp]->last + 1;
                        else st[sp-1]->node=new_node2(new_node0(cutstr(s,st[sp-1]->first,st[sp-1]->last)),st[sp]->node,s[p]);
                       
                      else
                        if (st[sp]->node == NULL)
                          st[sp-1]->node=new_node2(st[sp-1]->node,new_node0(cutstr(s,st[sp]->first,st[sp]->last)),s[p]);
                        else
                          st[sp - 1]->node = new_node2(st[sp - 1]->node, st[sp]->node, s[p]);
                      break;
      case minusch: ;
      case insecch: --sp;
                    lson = (st[sp - 1]->node == NULL) ?
                            new_node0(cutstr(s, st[sp - 1]->first, st[sp - 1]->last)) :
                            st[sp - 1]->node;
                    rson = (st[sp]->node == NULL) ?
                            new_node0(cutstr(s, st[sp]->first, st[sp]->last)) :       
                            st[sp]->node;
                    st[sp - 1]->node = new_node2(lson, rson, s[p]);
                    break;
 
      case starch : ;
      case plusch : if (st[sp - 1]->node == NULL) ++st[sp - 1]->last;
                    else st[sp - 1]->node = new_node1(st[sp - 1]->node, s[p]); 
                    break;
 
      case complch : lson = (st[sp - 1]->node == NULL) ?
                     new_node0(cutstr(s, st[sp - 1]->first, st[sp - 1]->last)) :      
                     st[sp - 1]->node;
                     st[sp - 1]->node = new_node1(lson, complch);
                     break;
 
      case alph : ;
      case langch : ;
      case epsch : st[sp]->first = (st[sp]->last = p);
                   st[sp++]->node = NULL;
                   break;
 
      case 'a' : st[sp]->first = p;
                 st[sp]->node = NULL;
                 if (S > ALPHSIZE) while is_digit(s[p + 1]) ++p;
                 st[sp++]->last = p;
                 break;
 
      default: if is_letter(s[p])
               { st[sp]->first = (st[sp]->last = p);
                 st[sp++]->node = NULL;
             }
             else /* Should not happen */ ;
    }
    ++p;
  }
  lson = (st[0]->node == NULL) ? new_node0(cutstr(s, 0, p - 1))  : st[0]->node; 
  return new_node1(lson, ' ');
}


/***********************************************************
 *                                                         *
 * Funtions for optimize_tree and tree2nfa                 *
 *                                                         *
 ***********************************************************/


/*                            *
 * Optimize tree (nine rules) *
 *                            */

void optimize_tree(head)
t_elem head;
 
{
 t_elem v = head->son[0];
 int son;
 
 searchnoson(v);
 while (v != head)
  switch (v->sons_no)
  { case 2: if (v->son_passed == 1) { v->son_passed = 0;
                                      v = v-> father;
                                    }
            else { v->son_passed = 1;
                   v = v->son[1];
                   searchnoson(v);
                 }
            break;
    case 1: if ((v->expr[0] == complch) && (v->father->expr[0] == complch))
            { if (is_leftson(v->father))
                v->father->father->son[0] = v->son[0];
  /* ~~L=L */ else v->father->father->son[1] = v->son[0];
              v->son[0]->father = v->father->father;
              v = v->son[0];
              if (v->sons_no == 2) v->son_passed = 1;
            }
            else v = v->father;
            break;
    case 0: switch (v->op)
            { case langch: switch (v->father->expr[0])
            /* %*=@ */     { case starch: v = v->father;
                                          v->op = epsch;
                                          v->expr[0] = epsch;
                                          v->sons_no = 0;
                                          break;
            /* %+=% */       case plusch: ;
            /* %&L=% */      case insecch: ;
            /* %.L=% */      case concatch: v = v->father;
                                            v->op = langch;
                                            v->expr[0] = langch;
                                            v->sons_no = 0;
                                            v->son_passed = 0;
                                            break;
            /* %UL=L */      case unionch: son = is_leftson(v) ? 1 : 0;
                                           v = v->father;
                                           if is_leftson(v)
                                             v->father->son[0] = v->son[son];
                                           else
                                             v->father->son[1] = v->son[son];
                                           v->son[son]->father = v->father;
                                           v = v->son[son];
                                           if (son == 0)
                                           { if (v->sons_no == 2)
                                             v->son_passed = 1;
                                           }
                                           else searchnoson(v);
                                           break;
                             case minusch: if is_leftson(v)      
                  /* %-L=% */                   { v = v->father;
                                                  v->op = langch;
                                                  v->expr[0] = langch;
                                                  v->sons_no = 0;
                                                  v->son_passed = 0;
                                                }
                                            else
                  /* L-%=L */               { v = v->father;
                                              if is_leftson(v)
                                                 v->father->son[0] = v->son[0];
                                              else
                                                 v->father->son[1] = v->son[0];
                                              v->son[0]->father = v->father;
                                              v = v->son[0];
                                              if (v->sons_no == 2)
                                                 v->son_passed = 1;
                                            }
                                           break;
                             case complch: ;
                             case headch: v = v->father;
                                          break;
                             default: /* Should not happen */;
                           }
                           break;
              case epsch: switch (v->father->op)
           /* @+=@ */      { case plusch: ;
           /* @*=@ */        case starch: v = v->father;
                                          v->op = epsch;
                                          v->expr[0] = epsch;
                                          v->sons_no = 0;
                                          break;
 
           /* @.L=L */       case concatch: son = is_leftson(v) ? 1 : 0;
                                            v = v->father;
                                            if is_leftson(v)
                                              v->father->son[0] = v->son[son];
                                            else
                                              v->father->son[1] = v->son[son];
                                            v->son[son]->father = v->father;
                                            v = v->son[son];
                                            if (son == 0)
                                            { if  (v->sons_no == 2)
                                              v->son_passed = 1;
                                            }
                                            else searchnoson(v);
                                            break;
                             case boolech: ;
                             case headch: v = v->father;
                                          break;
                             default: /* Should not happen */;
                           }
                           break;
              case alph: ;
              case regch: v = v->father;
                          break;
              default: /* Should not happen */;
            }
            break;
    default: /* Should not happen */;
  }
}

/***********************************************************
 *                                                         *
 * Mark nodes of tree (is_reg_ab)                          *
 *                                                         *
 ***********************************************************/

void mark_tree(head)
t_elem head;
 
{
  t_elem v = head->son[0];
 
  searchcil(v);
  v->is_reg_ab = TRUE;
  v = v->father;
 
  while (v != head)
  { switch (v->sons_no)
    { case 1:  v->is_reg_ab = TRUE;
               v = v->father;
               break;
      case 2: switch (v->son_passed)
              { case 0: v->son_passed = 1;
                        v = v->son[1];
                        searchcil(v);
                        v->is_reg_ab = TRUE;
                        v = v->father;
                        break;
                case 1: v->is_reg_ab = TRUE;
                        v->son_passed = 0;
                        v = v->father;
                        break;
                default: /* Should not happen */;
              }
              break;
      default: /* Should not happen */;
    }
  }
}


/***********************************************************
 *                                                         *
 * Functions for reorder_tree                              *
 *                                                         *
 ***********************************************************/

/* Reorder node of tree */
 
 
static void reorder_node(node)
t_elem node;
{
  t_elem v = node->son[0],
         father = node->father;
  string s;
  t_elem *son;
  posint son_count = 0,
         s_count = 0,
         i;
  char op = node->op;
 
  godown(v);
  while (v != father)
  { if (v->op != op)
    { ++son_count;
      ++s_count;
      v = v->father;
    }
    else
    { if (v->son_passed < (v->sons_no - 1))
      { v = v->son[++v->son_passed];
        godown(v);
      }
      else
      { v->son_passed = 0;
        ++s_count;
        v = v->father;
      }
    }  
  }  
  son = newt_ar(son_count);
  s = newstring(s_count);
  son_count = (s_count = 0);
 
  v = node->son[0];
  godown(v);
  while (v != father) 
  { if (v->op != op)
    { son[son_count++] = v;
      s[s_count++] = 'x';
      v = v->father;
    }
    else
    { if (v->son_passed < (v->sons_no - 1))
      { v = v->son[++v->son_passed];
        godown(v);
      }
      else
      { v->son_passed = 0;
        s[s_count++] = v->expr[0];
        v = v->father;
      }
    }
  }
  node ->expr = s;
  for (i = 0; i != son_count; ++ i)
    son[i]->father = node;
  node->son = son;
  node->sons_no = son_count;
  return;
}


/* Reorder tree with root 'head' */
 
static void reorder_tree(head)
t_elem head;
 
{
  t_elem v = head->son[0];
 
  searchcil(v);
  while (v != head)
  { switch (v->op)
    { case concatch:;
      case boolech:  if (strlen(v->expr) == (size_t) 1)
                     { if (!v->son[0]->is_reg_ab)
                       { reorder_node(v);
                         v = v->son[0];
                         passps(v);
                         break;
                       }
                     }
                     if (v->son_passed < (v->sons_no - 1))
                     { v = v->son[++v->son_passed];
                       if (strlen(v->father->expr) > (size_t) 1) passps(v);
                       else searchcil(v);
                     }
                     else
                     { v->son_passed = 0;
                       v = v->father;
                     }
                     break;
      case epsch: ;
      case langch: ;
      case alph: v->op = regch;
                 v = v->father;
                 break;
      case regch: ;
      case starch: ;
      case plusch: v = v->father;
                   break;
      default: /* Should not happen */;
    }
  }
  return;
}


/***********************************************************
 *                                                         *
 * Functions for rexnfa                                    *
 *                                                         *
 ***********************************************************/



/*                                          *
 * Convert "standard regular" node to eprex *
 *                                          */
 
static regex makerex(node)
t_elem node;
 
{ posint length; 
  regex outre = newrex();
  outre->exprex = node->expr;
  outre->erexl = strlen(node->expr);
  outre->rex=infix(outre,FALSE,&length);
  outre->rexl=length;
  outre->sno = S;
  return outre;
}

/*                  *
 * Build dfa for L+ *
 *                  */

static dfa plus_star_dfa(indfa,op)
dfa indfa;
char op;
{
  nfa hlp;
  posint qno = indfa->qno,
         sno = indfa->sno,
         q, l, newq;
  char h_op = (indfa->final[indfa->init]) ? plusch : op;
  dfa hlp1;
  
  /*tickon = FALSE;*/
  bufmark();
  hlp1 = dfamdfa(indfa, TRUE);
  buffree();/*tickon=TRUE;*/
  indfa = hlp1; 
  hlp = newnfa();
  newq = (h_op == starch) ? qno + 1 : qno;
  hlp->qno = newq;
  hlp->sno = sno;
  hlp->infin = newfinal(hlp->qno);
  hlp->delta = newndelta(hlp->sno, hlp->qno);
  for (q = 0; q <= qno; ++q)
  { if (indfa->final[q])
    { copyddelta(hlp->delta, q, indfa->delta, indfa->init);
      setfinalT(hlp->infin[q]);
    }
    copyddelta(hlp->delta, q, indfa->delta, q);
  }
  if (h_op == starch)
  { copyddelta(hlp->delta, newq, indfa->delta, indfa->init);
    setfinalT(hlp->infin[newq]);
    setinit(hlp->infin[newq]);
  }
  else
    setinit(hlp->infin[indfa->init]);
  hlp->is_eps = FALSE;
  hlp1 = nfa2dfa(hlp);
  buffree();/*tickon=TRUE;*/
  freenfa(hlp);
  return hlp1;
}


/*                          *
 * Build dfa for L1.L2...Ln *
 *                          */

static dfa conc_dfa(dfa_list)
d_array dfa_list;
{
  nfa hlp = newnfa();
  dfa *h_da = dfa_list->da;
  dfa outdfa, da_i;
  int i;
  posint n = dfa_list->no,
         qno = 0,
         sno = h_da[0]->sno,
         l, q, qno_i,
         init_i1,
         offset, lastoffset;
  mrkfin final_i;
  ddelta delta_i, delta_i1;
  boole init_is_final;
  /*tickon=FALSE;*/ bufmark();
  for (q = 0; q < n; ++q)
     dfa_list->da[q]=dfamdfa(dfa_list->da[q],FALSE);
  buffree();/*tickon=TRUE;*/
  for (i = 0; i != n; ++i) qno += (h_da[i]->qno + 1);
  hlp->qno = (--qno);
  hlp->sno = sno;
  hlp->infin = newfinal(hlp->qno);
  hlp->delta = newndelta(hlp->sno, hlp->qno);

  --i;
  da_i = h_da[i];
  delta_i = da_i->delta;
  final_i = da_i->final;
  qno_i = da_i->qno;

  for (q = 0; q <= qno_i; ++q)
  { copyddelta(hlp->delta, q, delta_i, q);
    if (final_i[q]) setfinalT(hlp->infin[q]);
  }
  lastoffset = 0;
  offset = qno_i + 1;
  --i;
  /* tick();*/
  do
  {
    init_i1 = da_i->init;
    delta_i1 = delta_i;
    da_i = h_da[i];
    delta_i = da_i->delta;
    final_i = da_i->final;
    qno_i = da_i->qno;

    init_is_final = isfinal(hlp->infin[init_i1 + lastoffset]);
    for (q = 0; q <= qno_i; ++q)
    { if (final_i[q])
      { for (l = 1; l <= sno; ++l)
          connect(hlp->delta, l, q + offset, delta_i1[l][init_i1] + lastoffset);  
        if (init_is_final) setfinalT(hlp->infin[q + offset]);
      }
      for (l = 1; l <= sno; ++l)
        connect(hlp->delta, l, q + offset, delta_i[l][q] + offset);
    }
    lastoffset = offset;
    offset += (qno_i + 1);
    --i;
    /*tick();*/
  } while (i >= 0);
  setinit(hlp->infin[da_i->init + lastoffset]);
  hlp->is_eps = FALSE;
  /*tickon=FALSE;*/bufmark();
  outdfa = nfa2dfa(hlp);
  buffree();/*tickon=TRUE;*/
  freenfa(hlp);
  return outdfa;
}

/* Functions for boole_dfa */

/* Find and insert element in binary tree */
static posint insert_elt(root, elem, n, max_no)
b_t_elem root;
array elem;
posint n, max_no;
{
  b_t_elem help, v = root;
  int c, i;

  for (;;)
  { for (i = 0; i != (n - 1); ++i) if (elem[i] != v->tupel[i]) break;
    c = v->tupel[i] - elem[i];
    if (c == 0) return v->no;
    if (c < 0)
    { if (v->rson == NULL)
      { help = newb_t_elem();
        help->tupel = elem;
        help->no = max_no + 1;
        v->rson = help;
        return help->no;
      }
      else v = v->rson;
    }
    else
    { if (v->lson == NULL)
      { help = newb_t_elem();
        help->tupel = elem;
        help->no = max_no + 1;
        v->lson = help;
        return help->no;
      }
      else v = v->lson;
    }
  }
}

/* Build dfa for boolean expressions */

static dfa boole_dfa(dfa_list, expr)
d_array dfa_list;
string expr;
{
  w_elem current = neww_elem(),
         first = current, last = current;
  b_t_elem b_head = newb_t_elem();
  b_t_elem b_root = newb_t_elem();
  dfa *h_da = dfa_list->da;
  array t = newarray(dfa_list->no);
  posint i, no,
         max_no = 0,
         n = dfa_list->no,
         s, sno = h_da[0]->sno,
         sc,
         length;
  b_array b_stack = newb_array(n);
  dfa outdfa = newdfa();
  b_head->rson = b_root;
  b_root->tupel = newarray(n);
  first->tupel = b_root->tupel;
  b_root->no = 0;
  first->delta_s = newarray(sno + 1);
  for (i = 0; i != n; ++i)
   {
    b_root->tupel[i] =  h_da[i]->init;
    /*tick();*/
   }

  while (current != NULL)
  { for (s = 1; s <= sno; ++s)
    { for (i = 0; i != n; ++i) t[i] = h_da[i]->delta[s][current->tupel[i]];
      no = insert_elt(b_root, t, n, max_no);
      current->delta_s[s] = no;
      if (no > max_no)
      { last = (last->next = neww_elem());
        last->tupel = t;
        last->delta_s = newarray(sno + 1);
        t = newarray(n);
        max_no++;
      }
    }
    current = current->next;
  }

  outdfa->delta = newddelta(sno, max_no);
  outdfa->final = newfinal(max_no);
  outdfa->qno = max_no;
  outdfa->sno = sno;
  outdfa->init = 0;

  current = first;
  length = strlen(expr);
  for (no = 0; no <= max_no; ++no)
  { for (s = 1; s <= sno; ++s)
      outdfa->delta[s][no] = current->delta_s[s];
    sc = 0;
    i = 0;
    for (s = 0; s < length ; ++s)
    { switch (expr[s])
      { case 'x' : b_stack[sc++] = h_da[i]->final[current->tupel[i]];
                   ++i;
                   break;
        case complch: b_stack[sc - 1] = !b_stack[sc - 1];
                      break;
        case unionch: --sc;
                      b_stack[sc - 1] = b_stack[sc - 1] || b_stack[sc];
                      break;
        case insecch: --sc;
                      b_stack[sc - 1] = b_stack[sc - 1] && b_stack[sc];
                      break;
        case minusch: --sc;                            
                      b_stack[sc - 1] = b_stack[sc - 1] && (!b_stack[sc]);
                      break;
        default: /* Should not happen */;
      }
    }
    outdfa->final[no] = b_stack[0];
    current = current->next;
  }
  return outdfa;
}


/***********************************************************
 *                                                         *
 * Functions for first_run                                 *
 *                                                         *
 ***********************************************************/

/*                                                              *
 * First run on tree: Handle standard rE's, boolean expr.'s and *
 *                    nodes below complch or insecch            *
 *                                                              */

static void first_run(head)
t_elem head;
{
  d_array dfa_list;
  nfa hlp_na;
  posint i;
  t_elem v = head->son[0];

  searchreg(v); 
  /*tickon=FALSE;*/bufmark();
  hlp_na = rex2nfa(makerex(v));
  buffree();/*tickon=TRUE;*/
  if(v->is_reg_ab)
    {v->na = hlp_na;
     v->q_no = hlp_na->qno + 1;
    }
  else 
    {/*tickon=FALSE;*/bufmark();
     v->da = nfa2dfa(hlp_na);
     buffree();/*tickon=TRUE;*/
     freenfa(hlp_na);
    }
  v = v->father;

  while (v != head)
    switch (v->op)
    { case plusch: if(!v->is_reg_ab)
                     v->da = plus_star_dfa(v->son[0]->da, v->op);
                   else v->q_no = v->son[0]->q_no;
                   v = v->father;
                   break; 
      case starch: if (!v->is_reg_ab)
                     v->da = plus_star_dfa(v->son[0]->da, v->op);
                   else v->q_no = v->son[0]->q_no + 1;
                   v = v->father; 
                   break;
      case concatch:;
      case boolech: if(v->son_passed < (v->sons_no - 1))
                      {v = v->son[++v->son_passed];
                       searchreg(v);
                       /*tickon=FALSE;*/bufmark();
                       hlp_na = rex2nfa(makerex(v));
                       buffree();/*tickon=TRUE;*/
                       if(v->is_reg_ab)
                         {v->na = hlp_na;
                          v->q_no = hlp_na->qno + 1;
                         }
                       else 
                         {/*tickon=FALSE;*/bufmark();
                          v->da = nfa2dfa(hlp_na);
                          buffree();/*tickon=TRUE;*/
                         }
                      }
                    else
                      {v->son_passed = 0;
                       if(((v->op == boolech) &&
                          (v->expr[strlen(v->expr) - (size_t) 1] != unionch)) ||
                          (! v->is_reg_ab))
                         {dfa_list = (d_array)newbuf(1, sizeof(struct dfa_array));
                          dfa_list->da = (dfa*)newbuf(v->sons_no, sizeof(dfa));
                          dfa_list->no = v->sons_no;
                          for(i = 0; i < v->sons_no; ++i)
                            dfa_list->da[i] = v->son[i]->da;
                          v->da = (v->op == concatch) ?
                             conc_dfa(dfa_list) : boole_dfa(dfa_list, v->expr);
                          v->q_no = v->da->qno + 1;
                         }
                       else
                         {v->q_no = v->son[0]->q_no + v->son[1]->q_no;
                          if (v->expr[strlen(v->expr) - (size_t) 1] == unionch) ++v->q_no;
                         }
                      }
                    v = v->father;
                    break;
      default: /* should not happen */;
    }
}

static void second_run(head, innfa)
t_elem head;
nfa innfa;
{ t_elem v = head->son[0];
  posint offset = 0,cut,init,lastoffset,p,q,s;
  ndelta delta = innfa->delta;
  mrkfin infin = innfa->infin;

  searchauto(v);
  if(v->na == NULL)
    {copyda(v->da);
     v->init = v->da->init;
    }
  else
    {copyna(v->na);
     v->init = 0;                    /* v->na->init =0 */
     /* v->init = v->na->init;          Problem */
    }
  lastoffset = 0;
  offset = v->q_no;

  v = v->father;
  while (v != head)
  { switch (v->op)
    { case plusch: init = v->son[0]->init;
                   for (q = lastoffset; q != offset; ++q)
                     if isfinal(infin[q])epstrans(q, init, lastoffset, offset);
                   v->init = init;
                   break;
      case starch: init = v->son[0]->init;
                   for (q = lastoffset; q != offset; ++q)
                     if isfinal(infin[q]) epstrans(q, init, lastoffset, offset);
                   epstrans(offset, init, lastoffset, offset);
                   setfinalT(infin[offset]);
                   v->init = offset;
                   ++offset;
                   break;
      case boolech: if(v->expr[strlen(v->expr) - (size_t) 1] == unionch)
                      if(v->son_passed == 0)
                        {v->son_passed = 1;
                         v = v->son[1];
                         searchauto(v);
                         if(v->na == NULL)
                           {copyda(v->da);
                            v->init = v->da->init + offset;
                           }
                         else
                           {copyna(v->na);
                            v->init = offset;                    /* v->na->init =0 */
                            /* v->init = v->na->init + offset;         Problem */
                           }
                         lastoffset = offset;
                         offset += v->q_no;
                        }
                      else
                        {v->son_passed = 0;
                         cut = lastoffset;
                         lastoffset -= v->son[0]->q_no;
                         epstrans(offset, v->son[0]->init, lastoffset, cut);
                         epstrans(offset, v->son[1]->init, cut, offset);
                         v->init = offset;
                         ++offset;
                        }
                     else /* Should not happen */ ;
                      break;
      case concatch: if(v->son_passed == 0)
                       {v->son_passed = 1;
                        v = v->son[1];
                        searchauto(v);
                        if(v->na == NULL)
                          {copyda(v->da);
                           v->init = v->da->init + offset;
                          }
                        else
                          {copyna(v->na);
                           v->init = offset;                    /* v->na->init =0 */
                           /* v->init = v->na->init + offset;         Problem */
                          }
                        lastoffset = offset;
                        offset += v->q_no;
                       }
                     else
                       {v->son_passed = 0;
                        cut = lastoffset;
                        lastoffset -= v->son[0]->q_no;
                        for(q = lastoffset; q != cut; ++q)
                          if(isfinal(infin[q]))
                            {setfinalF(infin[q]);
                             epstrans(q, v->son[1]->init, cut, offset);
                            }
                        v->init = v->son[0]->init;
                       }
                     break;
      default: /* Should not happen */;
    }
  /*tick();*/
    v = v->father;
  }
  setinit(infin[head->son[0]->init]);    
}

nfa genrex2nfa(regex re) {
  t_elem head;
  nfa outnfa;
 
  S = re->sno;
  head = postfix2tree(re);
  optimize_tree(head);
  mark_tree(head);
  reorder_tree(head);
  first_run(head);
 
  outnfa = newnfa();
  outnfa->sno = S;
  outnfa->qno = head->son[0]->q_no - 1;
  outnfa->delta = newndelta(outnfa->sno, outnfa->qno);
  outnfa->infin = newfinal(outnfa->qno);
  outnfa->minimal = FALSE;
  outnfa->is_eps =  FALSE;
  second_run(head, outnfa);
  freebuf();
  return outnfa;
}
