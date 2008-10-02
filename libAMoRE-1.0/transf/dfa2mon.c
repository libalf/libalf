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

/* dfa2mon.c 3.0 (KIEL) 11.94 */

#include "dfa2mon.h"

/* static melement searchtrans()
 *        monoid   dfa2mon()
 */
/******************************************************************/
typedef struct m1 
   {posint    number;       /* number */
    posint    lastletter;   /* last letter of shortest representative */
    array     transf;       /* transformation */
    array     gensucc;      /* gensucc[0] number of predecessor 
                             * gensucc[i] result of mult. with the i-th
                             * generator to the right */
    struct m1 *rsucc,*lsucc;/* right and left son in the tree structure */
    struct m1 *next;        /* next element in list structure */ 
   }*melement;
#define newmelement()		(melement)newbuf((posint)1,(posint)sizeof(struct m1))
/******************************************************************/
static melement root;  /* root of sorted tree of transitions */
static posint nextno;  /* actuel number of monoidelements */
static posint qno;     /* abbreviation: number of states of the underlying dfa */
/******************************************************************/
static melement searchtrans(atrans)
 /*  return pointer to melement where transition atrans is found
  *  if number of this element is nextno then this element is new
  */
array atrans;
{ 
  register posint j;
  register array btrans;
  melement run=root;
  for(;;)
    { btrans=run->transf;                                /* abbreviation */
      for(j=0;((j<=qno)&&(atrans[j] == btrans[j]));j++); /* compare transformations */
      if (j > qno)return(run);                           /* transformation found */
      else if(atrans[j] < btrans[j])                     /* search at left successor  */
	     if (run->lsucc!=NULL) run=run->lsucc; 
	     else                                        /* new transformation */
               {run->lsucc=newmelement();
	        run=run->lsucc; 
                run->number=nextno;
	        return(run);        
	       }
	   else                                           /* search at right successor */
	     if (run->rsucc!=NULL) run=run->rsucc;  
	     else                                         /* new transformation */
                   {run->rsucc=newmelement();
		    run=run->rsucc; 
                    run->number=nextno;
		    return(run);        
		   }
   }
}


/******************************************************************/
monoid dfa2mon(dfa indfa) { 
 register posint state;
 register array map;              /* used to compute a transformation */
 register array dd,tr;            /* abbreviation */
 monoid mon;
 posint gen; 
 melement  actuel,last,intree;
 boole nozero=TRUE,test;
  /* the successors of actuel are not yet computed
   * last points to the last element of the list
   * root points to the first element of the list 
   *         and to the root of the searchtree 
   * the elements are ordered with respect to the 
   *         lexicographical order of the transformation
   */
 qno=indfa->qno;  /* abbreviation */
 mon=newmon();
 mon->qno=qno;
 mon->sno=indfa->sno;
 root=newmelement();
 root->lastletter=0;
 map=newar(qno+1);
 for(state=0;state<=qno;state++)map[state]=state;
 root->transf=map;
 root->number=0;
 last=root;
 map=newar(qno+1);
 /* compute  the generators of the monoid: 1 ... stamon->gno
  * let2gen[j]=i   iff the jth letter has the transformation of the ith generator
  * generator[i]=j iff the jth letter is the first letter with the transformation 
  *                    of the ith generator
  * generator[0] is the identity
  */
 mon->let2gen=newar(mon->sno+1);
 mon->generator=newar(mon->sno+1);
 mon->gno=0;
 mon->let2gen[0]=0;
 mon->generator[0]=0;
 nextno=1;
 for(gen=1;gen<=mon->sno;gen++)       /* for all letters */
     {
      dd=indfa->delta[gen];           /* abbreviation */
      for(state=0;state<=qno;state++)map[state]=dd[state];
      intree=searchtrans(map);
      mon->let2gen[gen]=intree->number;
      if(intree->number==nextno)      /* new generator */
	{mon->generator[nextno]=gen;
         intree->lastletter=nextno; 
         intree->transf=map;
         last->next=intree;           /* append new element */
         last=last->next;
         map=newar(qno+1);
         nextno++;/* tick(); */
        }
      else
        if(intree->number==0)mon->mequals=TRUE; /* synt. monoid = syntactic semigroup */
     } /* end for */
 mon->gno=nextno-1;
 root->gensucc=newar(mon->gno+1);
 root->gensucc[0]=0;
 actuel=root;
 for(gen=1;gen<=mon->gno;gen++) /* for all generators ... */
   {root->gensucc[gen]=gen;
    actuel=actuel->next;
    actuel->gensucc=newar(mon->gno+1);
   }
 /* monoid construction */
 actuel=root->next;                        /* successors of root are already computed */
 while(actuel!=NULL)		           /* while list is not empty */
  {tr=actuel->transf;                      /* abbreviation */
   for(gen=1;gen<=mon->gno;gen++)          /* for all generators .. */
    {dd=indfa->delta[mon->generator[gen]]; /* abbreviation */
     for(state=0;state<=qno;state++)       /* compute transformation */
        map[state]=dd[tr[state]];
     intree=searchtrans(map);
     actuel->gensucc[gen]=intree->number;
     if(intree->number==nextno)            /* new transformation */
	{intree->lastletter=gen;
         intree->transf=map;
         map=newar(qno+1);
	 intree->gensucc=newar(mon->gno+1);
	 intree->gensucc[0]=actuel->number;
         last->next=intree;                /* append new element to list */
         last=last->next;
         nextno++;/*tick();*/
	}
      else /* old element, test for identity */
	if(intree->number==0)mon->mequals=TRUE;
    }  /* end for */
   actuel=actuel->next;
  }    /* end while */
/* store the results in arrays */
  mon->mno=nextno;
  mon->gensucc=newarray1(nextno);
  mon->no2trans=newarray1(nextno);
  mon->no2length=newar(nextno);
  mon->lastletter=newar(nextno);
/* identity */
  mon->no2length[0]=0;
  mon->lastletter[0]=0;
  mon->no2trans[0]=root->transf;
  mon->gensucc[0]=root->gensucc;
/* rest of monoid */
  for(state=1;state<nextno;state++)
    {root=root->next;
     mon->lastletter[state]=root->lastletter;
     mon->no2length[state]=mon->no2length[root->gensucc[0]]+1;
     mon->no2trans[state]=root->transf;
     mon->gensucc[state]=root->gensucc;
    }
  mon->word=newar(mon->no2length[mon->mno-1]);
  gen=mon->no2length[mon->mno-1]+1;  /* number of letters of longest element +1 */
  gen *= (mon->sno<=27)? 1 : strlen(pi2a(mon->sno))+1;
  mon->repr=(string)calloc(gen,sizeof(char));
  for(state=0;state<gen;)mon->repr[state++]='\0';
  freebuf(); 
  dispose(map);
  /* search for zero */
  for(state=0;(state<mon->mno)&&nozero;state++)
    {test=TRUE;
     for(gen=1;(gen<=mon->gno)&&test;gen++)
       if(mon->gensucc[state][gen]!=state) test=FALSE;
     for(gen=1;(gen<=mon->gno)&&test;gen++)
       if(mult(mon,gen,state)!=state) test=FALSE;
     if(test)nozero=FALSE;
     mon->zero=state;
    }
  if(nozero) mon->zero=mon->mno;
  return(mon);
}
