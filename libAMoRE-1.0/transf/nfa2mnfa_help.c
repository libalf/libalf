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

/* nfa2mnfa_help.c
 */

#include "nfa2mnfa_help.h"
 
nfa delsta(nfa inputnfa) {
 mrkfin infin=inputnfa->infin;   /* abbreviation */
  nfa result;                     /* Automaton to be returned.       */
  b_array reach1,reach2;          /* mark reachable and productive states */
  array stack;                     
  posint height=0;                /* height of stack */
  posint state1,state2,s1,s2,letter;
  posint count=0;                 /* count reachable and productive states */

  reach1=newb_array(inputnfa->qno+1);
  reach2=newb_array(inputnfa->qno+1);
  stack=newarray(inputnfa->qno+1);
  /* init stack with initial states */
  for(state1=0;state1<=inputnfa->qno;state1++)
    if(isinit(infin[state1]))
      {reach1[state1]=TRUE;
       stack[height++]=state1;
      }
  while(height) /* stack not empty */
    {state1=stack[--height];
     for(letter=1;letter<=inputnfa->sno;letter++)
       for(state2=0;state2<=inputnfa->qno;state2++)
         if(testcon(inputnfa->delta,letter,state1,state2)&&(!reach1[state2]))
           {reach1[state2]=TRUE;
            stack[height++]=state2;
           }
    } 
  /* all reachable states are marked in reach1
   * initialize stack with reachable final states 
   */
  for(state1=0;state1<=inputnfa->qno;state1++)
    if(isfinal(infin[state1]) && reach1[state1])
      {reach2[state1]=TRUE;
       stack[height++]=state1;
       count++;
      }
  while(height) /* stack not empty */
    {state1=stack[--height];
     for(state2=0;state2<=inputnfa->qno;state2++)
       if(reach1[state2])
         for(letter=1;letter<=inputnfa->sno;letter++)
           if(testcon(inputnfa->delta,letter,state2,state1)&&(!reach2[state2]))
             {reach2[state2]=TRUE;
              stack[height++]=state2;
              count++;
             }
    }  
  /* in reach2 all reachable and productive states are marked
   * there are exactly count many of these states
   */
  if(count==(inputnfa->qno+1))  /* No state has to be eliminated. */
    {freebuf();return inputnfa;}

  /* A new automaton has to be calculated. */
  result=newnfa();
  result->sno=inputnfa->sno;
  result->is_eps=FALSE;
  result->minimal=FALSE;
  if(count)result->qno=count-1;
  else     result->qno=0;
  result->delta=newndelta(result->sno,result->qno);  
  result->infin=newfinal(result->qno);    

  if(count==0) /* L(na)==empty */
    {setinit(result->infin[0]);
     setfinalF(result->infin[0]);
    }
  else
    {for(state1=0,count=0;state1<=inputnfa->qno;state1++)
       if(reach2[state1]) stack[count++]=state1;
     /* stack contains all old states, that are reachable and productive */
     for(state1=0;state1<count;state1++)
       {s1=stack[state1];             /* abbreviation for old state */
        setfinal(result->infin[state1],isfinal(infin[s1]));  
        if(isinit(infin[s1]))setinit(result->infin[state1]);  
        for(state2=0;state2<count; state2++)
          {s2=stack[state1];          /* abbreviation for old state */
           for (letter=1;letter<=result->sno;letter++)
             if(testcon(inputnfa->delta,letter,s1,s2))
               connect(result->delta,letter,state1,state2); 
          }   
       }
    }
  freebuf();
  return result;              
}

nfa invers_d(dfa inputdfa) {
    int i,j;
    nfa result;
    
    result=newnfa();
    result->is_eps=FALSE;
  result->minimal=FALSE;
  result->sno=inputdfa->sno;                      
  result->qno=inputdfa->qno;
  result->infin=newfinal(result->qno);
  result->delta=newndelta(result->sno,result->qno);
  /* Find initial and final states and compute delta. */
  for (i=0; i<=inputdfa->qno; i++)
   {/* Test if i is a new initial state. */
    if (isfinal(inputdfa->final[i]))
      setinit(result->infin[i]);
    /* Compute delta for state i. */
    for (j=1; j<=inputdfa->sno; j++)
      connect(result->delta,j,inputdfa->delta[j][i],i);
   }    
  /* Now find only final state. */               
  setfinalT(result->infin[inputdfa->init]);
  return result;
}
/***************************************************************************/
