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

/* win_bin.c 3.0 (KIEL) 3.95 */

#include "binary.h"

/* functions included in win_bin.c
 *
 *	nfa	unionfa()
 *	nfa	concatfa()
 *	dfa	insecfa()
 *	nfa	shuffle()
 *	nfa	leftquot()
 *	dfa	rightquot()
 */

/******************************************************************/
/******************************************************************/

typedef struct pairlst{posint info1,info2;
                       struct pairlst *next;
                      }* pairlist;

#define newpair()	(pairlist)newbuf((posint)1,(posint)sizeof(struct pairlst))

static void pairinit(ptfirst,ptlast,info1,info2)
pairlist *ptfirst,*ptlast;
posint info1,info2;
{*ptfirst=newpair();
(*ptfirst)->info1=info1;
(*ptfirst)->info2=info2;
(*ptfirst)->next=NULL;
 *ptlast= *ptfirst;
}

static void pairinsert(ptlast,info1,info2)
pairlist* ptlast;
posint info1,info2;
{(*ptlast)->next=newpair();
  *ptlast=(*ptlast)->next;
 (*ptlast)->next=NULL;
 (*ptlast)->info1=info1;
 (*ptlast)->info2=info2;
}

/* computes the union L(na1)+L(na2)
 * RETURN a nfa 
 */
nfa unionfa(nfa na1,nfa na2)

{ nfa result;
  posint i,j,k,ri,rj,newbeg2;
  result = newnfa();
    result->qno = na1->qno + na2->qno + 1;
  /* state 0 is the new initial state
   * q -> q                 q in na1
   * q -> q + na1->qno +1   q in na2
   */     
  result->sno = na1->sno;
  result->minimal = FALSE;
  result->is_eps = FALSE;
  result->infin = newfinal(result->qno);
  result->delta = newndelta(result->sno,result->qno);
  /* copy na1->delta */
  for(i=0; i <=na1->qno; i++) 
    {setfinal(result->infin[i],isfinal(na1->infin[i]));
     if(isinit(na1->infin[i]))
       setinit(result->infin[i]);
     for(j=0; j <= na1->qno; j++)
	for(k=1; k<=na1->sno; k++)
          cpdelta(result->delta,k,i,j,na1->delta,k,i,j);
    }
  /* copy na2->delta */
  newbeg2 = na1->qno+1; 
  for(i=0,ri=newbeg2; i <=na2->qno; i++, ri++) 
    {setfinal(result->infin[ri],isfinal(na2->infin[i]));
     if(isinit(na2->infin[i]))setinit(result->infin[ri]);
     for(j=0,rj=newbeg2; j <= na2->qno; j++, rj++)
       for(k=1; k<=na2->sno; k++)
         cpdelta(result->delta,k,ri,rj,na2->delta,k,i,j);
    }
  return result;
} /* unionfa */



/* computes the concatenation L(na1).L(na2)
 * RETURN a nfa describing the new language
 */
nfa concatfa(nfa na1,nfa na2)
{ nfa result;
  posint i,j,k,letter,ri,rj,rk,newbeg2;
  boole epsilon=FALSE; /* test epsilon in L(na2) */
  result = newnfa();
  result->qno = na1->qno + na2->qno + 1;
  /* q -> q                  q in na1
   * q -> q + na1->qno + 1   q in na2
   *     
   * copy transitions from final states of na1 to initial states of na2  
   * initial states = initial states of na1
   * final states   = final states of na2 
   * if(epsilon in L(na2)) then
   *    final states = final states + final states of na1
   */
  result->sno = na1->sno;
  result->minimal = FALSE;
  result->is_eps = FALSE;
  result->infin = newfinal(result->qno);
  result->delta = newndelta(result->sno,result->qno);
  /* copy na1->delta + initial states */
  for(i=0; i <=na1->qno; i++)
    {if(isinit(na1->infin[i]))setinit(result->infin[i]);
     for(j=0; j <= na1->qno; j++)
       for(letter=1; letter<=na1->sno; letter++)
         cpdelta(result->delta,letter,i,j,na1->delta,letter,i,j);
    }
  /* copy na2->delta + final states */
  newbeg2 = na1->qno+1;
  for(i=0,ri=newbeg2; i <=na2->qno; i++, ri++)
    {if(isfinal(na2->infin[i]))
       {setfinalT(result->infin[ri]);
        if(isinit(na2->infin[i]))epsilon=TRUE; 
       }
     for(j=0,rj=newbeg2; j <= na2->qno; j++, rj++)
       for(letter=1; letter<=na2->sno; letter++)
         cpdelta(result->delta,letter,ri,rj,na2->delta,letter,i,j);
    }
  /* copy trans. from initial states of na1 to final states of na2 
   * if epsilon in L(na2) then make final states of na1 final states in result
   */ 
  for(i=0;i<=na1->qno;i++)
    if(isfinal(na1->infin[i]))
      {if(epsilon) setfinalT(result->infin[i]);
       for(j=0,ri=newbeg2;j<=na2->qno;j++,ri++)
         if(isinit(na2->infin[j]))
           for(k=0,rk=newbeg2; k <= na2->qno; k++, rk++)
             for(letter=1; letter<=na2->sno; letter++)
               cpdelta(result->delta,letter,i,rk,na2->delta,letter,j,k);
      }
   return result;
} /* concatfa */

static posint offset;

#define pair(A,B) ((A*offset)+B)

/* if minus  compute L(da1) \ L(da2) else L(da1) & L(da2) */

dfa insecfa(dfa da1,dfa da2, boole minus)
{ dfa result;
  posint q1,q2,let;
  posint help;
  offset = da2->qno+1;	

  result = newdfa();
  result->sno = da1->sno;
  /* this would make sense also...                            */
  /* result->sno = da1->sno > da2->sno ? da2->sno : da1->sno; */

  result->qno = pair(da1->qno,da2->qno);
  result->init = pair(da1->init,da2->init);
  result->minimal = FALSE;
  result->final = newfinal(result->qno);
  result->delta = newddelta(result->sno,result->qno);

  for(q1=0,help=0; q1<=da1->qno; q1++) {
        for(q2=0; q2 <=da2->qno; q2++,help++) { /* help = pair(q1,q2) */
              if (minus) {
                result->final[help] = da1->final[q1] && (!da2->final[q2]);
              } else {
                result->final[help] = da1->final[q1] && da2->final[q2];
              }
              for(let=1; let<=result->sno; let++) {
                    result->delta[let][help] = pair(da1->delta[let][q1],da2->delta[let][q2]);
                    }
              }
        }
  return result;
} /* insecfa */


/* compute the shuffle of na1 and na2 
 * compute a nfa for the shuffle 
 */ 
nfa shuffle(nfa na1,nfa na2)

{ nfa result;
  posint q1,q2,letter;
  posint help,help1,q3,s;
  offset = na2->qno+1;	
  result = newnfa();
  result->sno = na1->sno;
  result->qno = pair(na1->qno,na2->qno);
  result->minimal = FALSE;
  result->is_eps = FALSE;
  result->infin = newfinal(result->qno);
  result->delta = newndelta(result->sno,result->qno);
  /* compute new transitions 
   * (p,q),a,(p',q) <=> in na1 p,a,p'
   * (p,q),a,(p,q') <=> in na2 q,a,q'
   * (p,q) is final <=> p and q is final
   * (p,q) is initial <=> p and q is initial
   * all states in result are reachable
   */
  for(q1=0,help=0,help1=0;q1<=na1->qno;q1++,help1+=offset) /* help1=q1*offset */
    for(q2=0;q2<=na2->qno;q2++,help++)                     /* help=pair(q1,q2) */
      {setfinal(result->infin[help],isfinal(na1->infin[q1])&&isfinal(na2->infin[q2]));
       if(isinit(na1->infin[q1])&&isinit(na2->infin[q2]))
          setinit(result->infin[help]);
       for(letter=1;letter<=na1->sno;letter++)
         {for(q3=0,s=q2;q3<=na1->qno;q3++,s+=offset)    /* s=pair(q3,q2)=q3*offset+q2 */
	    if(testcon(na1->delta,letter,q1,q3))
              connect(result->delta,letter,help,s);
	  for(q3=0,s=help1;q3<=na2->qno;q3++,s++)       /* s=pair(q1,q3)=q1*offset+q3 */
	    if(testcon(na2->delta,letter,q2,q3))
              connect(result->delta,letter,help,s);
	 }
      }
  return result;
} /* shuffle */


/* L(leftquot(dfa1,dfa2)= { w | ex. v in L(dfa2) vw in L(dfa1)
 * output is a nfa with usualy more than one initial state 
 */
nfa leftquot(dfa indfa1, dfa indfa2)
{
 nfa outnfa;
 posint state1,state2;
 posint help1,help2;
 pairlist first,last,run;
 posint letter;
 arrayofb_array mark; /* mark accessible states */
 mark=newarrayofb_array(indfa1->qno+1);
 for(state1=0;state1<=indfa1->qno;state1++)
   mark[state1]=newb_array(indfa2->qno+1);
 /* init outnfa */
 outnfa=newnfa();
 outnfa->is_eps=FALSE;
 outnfa->qno=indfa1->qno;
 outnfa->sno=indfa1->sno;
 outnfa->infin=newfinal(outnfa->qno);
 outnfa->delta=newndelta(outnfa->sno,outnfa->qno);
   /* copy delta and final */
 for(state1=0; state1<=outnfa->qno; state1++)
    setfinal(outnfa->infin[state1],indfa1->final[state1]);
 for(letter=1; letter<=outnfa->sno; letter++)
    for(state1=0; state1<=outnfa->qno; state1++) 
	connect(outnfa->delta,letter,state1,indfa1->delta[letter][state1]);
 /* compute all pairs p,q which are accessible from the pair p0,q0 */
 pairinit(&first,&last,indfa1->init,indfa2->init);
 mark[indfa1->init][indfa2->init]=TRUE;
 run=first;
 while(run!=NULL)
  {state1=run->info1;
   state2=run->info2;
   for(letter=1;letter<=indfa1->sno;letter++)
      {help1=indfa1->delta[letter][state1];
       help2=indfa2->delta[letter][state2];
       if(!mark[help1][help2]) /* new pair */
	 {pairinsert(&last,help1,help2);
          mark[help1][help2]=TRUE;
         }
      }
   run=run->next;
  }
/* search for new initial states */
while(first!=NULL)
  {state1=first->info1;
   state2=first->info2;
   first=first->next;
   if(indfa2->final[state2])   /* mark state1 initial */ 
           setinit(outnfa->infin[state1]);
  }
 freebuf();
 return(outnfa);
}


/* L(rightquot(dfa1,dfa2)) = { w | ex. v in L(dfa2) wv in L(dfa1) */
dfa rightquot(dfa indfa1, dfa indfa2)
{
 posint state,state1,state2;
 posint help1,help2;
 posint letter;
 boole test;
 dfa outdfa;
 pairlist first,last,run;
 arrayofb_array mark; /* mark accessible states */
 /* mark[p][q] == TRUE       if p,q is reachable from the current state in the for loop 
  *                           and indfa2->init
  *            == UN_KNOWN   if no pair of final states is reachable from p,q
  *            == FALSE      it is not known whether a pair of final stes is reachable
  */
 mark=newarrayofb_array(indfa1->qno+1);
 for(state1=0;state1<=indfa1->qno;state1++)
   mark[state1]=newb_array(indfa2->qno+1);
 /* init outdfa */
 outdfa=newdfa();
 outdfa->qno=indfa1->qno;
 outdfa->init=indfa1->init;
 outdfa->sno=indfa1->sno;
 outdfa->minimal=FALSE;
 outdfa->final=newfinal(outdfa->qno);
 outdfa->delta=newddelta(outdfa->sno,outdfa->qno);
   /* copy delta */
 for(letter=1; letter <= indfa1->sno; letter++)
   for(state1=0; state1 <= indfa1->qno; state1++)
     outdfa->delta[letter][state1] = indfa1->delta[letter][state1];
 /* search for all states q in indfa1 such that:
  * a pair of final states is accessible from the pair q and indfa2->init
  */ 
for(state=0;state<=indfa1->qno;state++)
   {if(indfa1->final[state]&&indfa2->final[indfa2->init])
        {outdfa->final[state]=TRUE;continue;}
    else 
      if(mark[state][indfa2->init]) continue; 
    pairinit(&first,&last,state,indfa2->init);
    mark[state][indfa2->init]=TRUE;
    run=first;
    test=TRUE;
    while((run!=NULL)&&test)
     {state1=run->info1;
      state2=run->info2;
      for(letter=1;letter<=indfa1->sno;letter++)
         {help1=indfa1->delta[letter][state1];
          help2=indfa2->delta[letter][state2];
	  if(indfa1->final[help1]&&indfa2->final[help2])
	     {test=FALSE;break;}
          if(!mark[help1][help2]) /* mark[help1,help2] != TRUE,UN_KNOWN */
	    {pairinsert(&last,help1,help2);
             mark[help1][help2]=TRUE;
            }
         }
      run=run->next;
     }
   if(!test) /* new final state, delete mark */
      outdfa->final[state]=TRUE;
   if(test) test=UN_KNOWN; 
      /* if test==TRUE no pair of final states is reachable from all pairs in first */
   while(first!=NULL)
      {mark[first->info1][first->info2]=test; 
       first=first->next;
      }
  } /* end for */
 freebuf();
 return(outdfa);
}
