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

/* nfa2mnfa.c
 */

#include "nfa2mnfa.h"
#include "nfa2dfa.h" /* modnfa2dfa */
#include "dfa2nfa.h" /* dfa2nfa */
#include "unaryB.h" /* revnfa*/

/***************************************************************************/
/* Procedures included:                                                    */
/* nfa nfa2mnfa (to be called for minimization)                            */
/***************************************************************************/
static boole row_is_covered();
static boole col_is_covered();
static void compute_compact_rows();
static void compute_compact_cols();
static nfa compute_irreducible_automaton();
static void calculate_necessary_grids();
static void calculate_grids_r();
static void calculate_grids_c();
static void add_to_list();
static void fundamental_table_calculation();
static void fun_tab_delta_calculation();
static boole is_initial();
static boole is_final();

static posint ai,am;          /* needed for add & setelem */    
static posint ZEILEN,SPALTEN,GITTER,nbs,nbz,nbg;     
static posint MZEILEN,MSPALTEN,NECESSARY,REST; 
    /* ZEILEN is the number of rows of the RAM - 1;          */
    /* SPALTEN is the number of columns of the RAM - 1;      */
    /* GITTER is the number of grids to be looked at;        */
    /* nb.=number of bytes to form a set with . elements;    */  
    /* MZEILEN is the number of rows of the MM - 1;          */     
    /* MSPALTEN is the number of columns of the MM - 1;      */
    /* NECESSARY is the number of essential grids;           */
    /* REST=GITTER-NECESSARY;                                */
static posint rowsink,colsink;        
    /* Rowsink is the number of the empty state of b or      */
    /* ZEILEN+1 if b has no empty state. Colsink is the      */
    /* number of the empty state of c or SPALTEN+1 if c has  */
    /* no empty state.                                       */
static array rowweight,colweight; 
    /* Rowweight[i]=number of 1's in row i of the RAM;       */
    /* colweight[i]=number of 1's in column i of the RAM.    */
static cover grids;   
    /* Grids->sizeofglist=GITTER. Grids->glist points to the */
    /* list of grids with grids->glist->info being the first */
    /* grid. The list starts with the essential (necessary)  */
    /* grids and continues with the other grids.             */ 
static gridlist firstI,firstII;      
    /* FirstI->info is the first grid in the list of grids.  */
    /* FirstII->info is the first non-essential grid.        */
static posint total;
    /* Total is the number of 1's in the RAM.                */
static ptset compact_row,compact_col;                          
    /* Testelem(i,compact_row->grset) iff row i is compact   */ 
    /* or rowweight[i]==0.                                   */
    /* Testelem(i,compact_col->grset) iff column i is        */
    /* compact or colweight[i]==0.                           */    
    /* A row (column) is compact if it is the union of some  */
    /* other rows (columns).                                 */
static ptset ones;                                         
    /* Testelem(i,ones->grset) iff the row i resp. the       */
    /* column i contains a 1 standing alone in both its row  */
    /* and its column.                                       */
static ptset initial_grids,final_grids; 
    /* Testelem(i,initial_grids) iff grid i - considered as  */
    /* a state of the fundamental table - is an initial      */
    /* state.                                                */
    /* Testelem(i,final_grids) iff grid i - considered as a  */    
    /* state of the fundamental table - is a final state.    */
static ptset** delta;
    /* Transition function of fundamental table.             */
static bitmap* tra;  
    /* Tra is to be seen as the RAM. It is:                  */
    /* Testelem(i,tra[j]) iff the (i,j)-element of the       */
    /* matrix is 1.                                          */
static ptset* covered_row,*covered_col;                        
    /* Testelem(i,covered_row[j]) iff row j covers row i and */ 
    /* row i contains at least one 1.                        */
    /* Testelem(i,covered_col[j]) iff column j covers column */
    /* i and column i contains at least one 1.               */
static boole V; 
    /* V iff ZEILEN<SPALTEN. Thus, it is possible to         */
    /* calculate either "based on rows" or "based on         */
    /* columns", depending on which one is less.             */ 

nfa nfa2mnfa(nfa na, dfa minda) { 
    posint i,j,MAXI;   
    /* MAXI=number of states of the best automaton so far - 1. */
    dfa b,c;
    nfa rvs,na1,f,g,h,d,current_best_automaton;
    if (na->qno<1)return na;
    na1=delsta(na);   /* Delete superfluous states. */      
    if (na1->qno<1) return na1;

    b=minda;      
    rvs=invers_d(b);
    c=modnfa2dfa(rvs,&tra);  
    /* c is MD(dual automaton to na). */
    /* The RAM is not explicitly stored, but it is: The        */
    /* (i,j)-element is 1 iff testelem(i,tra[j]).              */
    if((na1->qno<=b->qno)&&(na1->qno<=c->qno))
	current_best_automaton=na1; 
    else if(b->qno<=c->qno)current_best_automaton=dfa2nfa(b); /* FIXME */
    else current_best_automaton=invers_d(c);
    MAXI=current_best_automaton->qno;  

    ZEILEN=b->qno;
    SPALTEN=c->qno; 
    nbz=(ZEILEN+1)/SIZEOFBYTE + 1;   
    nbs=(SPALTEN+1)/SIZEOFBYTE + 1; 
    V=(ZEILEN<SPALTEN);                     
    total=0;
    rowweight=newarray(ZEILEN+1);
    colweight=newarray(SPALTEN+1);                    
    for(i=0; i<=ZEILEN; i++)
	{for(j=0; j<=SPALTEN; j++)
	    if(testelem(i,tra[j]))
		{rowweight[i]++;colweight[j]++;}
	total+=rowweight[i];
	} 
    /* Rowweight[i] is the number of 1's in row i of the RAM;  */
    /* colweight[j] is the number of 1's in column j of the    */
    /* RAM; total is the number of 1's in the RAM.             */
    /* The compact rows and columns have to be found. Their    */
    /* numbers are stored in compact_row->grset and            */
    /* compact_col->grset. The numbers of the rows that are    */
    /* covered by row i are stored in covered_row[i]->grset.   */
    /* The numbers of the columns that are covered by column i */
    /* are stored in covered_col[i]->grset.                    */
    compute_compact_rows();
    compute_compact_cols();        
    /* Now: Row i is compact iff testelem(i,compact_row        */
    /* ->grset); column i is compact iff testelem(i,           */
    /* compact_col). The number of compact rows is compact_row */
    /* ->ngrset. The number of compact columns is compact_col  */
    /* ->ngrset. The row/column for the empty state is         */
    /* considered compact. Row i covers row j (with            */
    /* rowweight[j]>0) iff testelem(j,covered_row[i]); column  */
    /* i covers column j (with colweight[j]>0) iff testelem(j, */
    /* covered_col[i]). The number of rows with weight>0       */
    /* covered by row i is covered_row[i]->ngrset. The number  */
    /* of columns with weight>0 covered by column i is         */
    /* covered_col[i]->ngrset.                                 */
    MZEILEN=ZEILEN-compact_row->ngrset;
    MSPALTEN=SPALTEN-compact_col->ngrset;
    if(V)
	{/* The calculation is based on the rows and on the       */
	    /* automaton b, because the number of rows of the RAM is */
	    /* less than the number of columns.                      */      
	    f=dfa2nfa(b);                 
	    for (i=0; i<=ZEILEN; i++)
		if(testelem(i,covered_row[b->init]->grset))
		    setinit(f->infin[i]);      /* Another initial state. */
    nbs=(ZEILEN+1)/SIZEOFBYTE + 1;      
	}
    else
	{/* The calculation is based on the columns and on the    */
	    /* automaton c, because the number of columns of the RAM */
	    /* is not bigger than the number of rows.                */
	    f=dfa2nfa(c);    
	    for(i=0; i<=SPALTEN; i++)
       if (testelem(i,covered_col[c->init]->grset))
	   setinit(f->infin[i]);      /* Another initial state. */
	    nbs=(SPALTEN+1)/SIZEOFBYTE + 1; 
	}                                                            
    /* Now compute the irreducible automaton with the smallest */
    /* number of states if this number is smaller than the     */
    /* number of states of the best automaton known so far.    */
    if(MZEILEN<MSPALTEN)
	{if(MAXI>=MZEILEN)
	    {h=compute_irreducible_automaton(b,compact_row,covered_row,ZEILEN,MZEILEN);  
	    current_best_automaton=h;
	    MAXI=h->qno;
	    }
	}
  else
      if (MAXI>=MSPALTEN)
	  {g=compute_irreducible_automaton(c,compact_col,covered_col,SPALTEN,MSPALTEN); 
	  h=revnfa(g);  /* Otherwise dual language. */
	  current_best_automaton=h; 
	  MAXI=h->qno;
	  }
    /* The next step is to find the 1's that stand alone in    */
    /* both their row and their column and to define           */
    /* (necessary) grids that cover these 1's.                 */
    calculate_necessary_grids();                                 
    if (NECESSARY>MAXI)
	/* It is not possible to find an automaton with less     */
	/* states than h or na1 that accepts L(na) or its dual   */
	/* language.                                             */
	{freebuf();return(current_best_automaton);} 
    /* Now calculate the other grids. */
    if (V) calculate_grids_r();
    else   calculate_grids_c(); 
    /* Now the first NECESSARY grids of the list of grids are  */
    /* the essential grids and the other REST grids are the    */
    /* non-essential grids.                                    */
    
    fundamental_table_calculation(f);      
    /* Compute the initial grids (states), final grids         */
    /* (states) and delta of the fundamental table.            */
    
    d=calc_sol(delta,V,firstI,firstII,GITTER,NECESSARY,REST,MAXI,f,initial_grids,final_grids);
    freebuf();
    if(d)  return(d);
    else  return(current_best_automaton);
} 
/***************************************************************************/

/***************************************************************************/
/* row_is_covered                                                          */
/***************************************************************************/
/* Row_is_covered(j,k) returns TRUE iff for all columns i (if the (j,i)-   */
/* element of the RAM is 1 then the (k,i)-element also is 1) and row j     */
/* contains at least one 1. The first condition means that row k covers    */
/* row j.                                                                  */
/***************************************************************************/
static boole row_is_covered(posint j,posint k) { 
    posint i;
    if (rowweight[j]==0) return FALSE; /* Empty state. */
    if (rowweight[j]>=rowweight[k]) return FALSE;
    for (i=0; i<=SPALTEN; i++)
	if(testelem(j,tra[i]) && (!testelem(k,tra[i]))) return FALSE;
    return TRUE;
}

/***************************************************************************/
/* col_is_covered                                                          */
/***************************************************************************/
/* Col_is_covered(j,k) returns TRUE iff for all rows i (if the (i,j)-      */
/* element of the RAM is 1 then the (i,k)-element also is 1) and column j  */
/* contains at least one 1. The first condition means that column k covers */
/* column j.                                                               */
/***************************************************************************/
static boole col_is_covered(posint j,posint k) { 
    posint i;
    if (colweight[j]==0) return FALSE; /* Empty state. */
    if (colweight[j]>=colweight[k]) return FALSE;
    for (i=0; i<=ZEILEN; i++)
	if (testelem(i,tra[j]) && (!testelem(i,tra[k]))) return FALSE;
    return TRUE;
}

/***************************************************************************/
/* compute_compact_rows                                                    */
/***************************************************************************/
/* The procedure finds all compact rows and adds their numbers to the set  */
/* compact_row->grset, that is row i is compact iff testelem(i,compact_row */
/* ->grset). In covered_row[i]->grset the numbers of all the rows that are */
/* covered by row i and contain at least one 1 are stored. This is useful  */
/* for the computation of the irreducible automaton.                       */
/***************************************************************************/
static void compute_compact_rows()
{ posint i,j,k;          
  ptset pattern; /* Pattern saves the 1's in row i that are  */
                 /* not yet covered.                         */
  rowsink=ZEILEN+1;
  covered_row=newgsarray(ZEILEN+1);
  for (i=0; i<=ZEILEN; i++)
    newpattern(covered_row[i],nbz);
  newpattern(compact_row,nbz);
  newpattern(pattern,nbs);  
  for (i=0; i<=ZEILEN; i++)
  { if (rowweight[i]<2)  
    { if (rowweight[i]==0)
      { /* If there is no 1 in the row, the row can be       */
        /* eliminated and is best considered compact.        */
        add(i,compact_row->grset);
        compact_row->ngrset++;      
        rowsink=i;   /* Store the number of the empty state. */
      }
    } 
    else         
    { for (j=0; j<=SPALTEN; j++)
        if (testelem(i,tra[j])){add(j,pattern->grset);}
        else {sub(j,pattern->grset);}
      pattern->ngrset=rowweight[i];                          
      /* Now testelem(j,pattern) iff testelem(i,tra[j]) for  */
      /* every column j.                                     */
      for(j=0; j<=ZEILEN; j++)
        if(j!=i)
          if(row_is_covered(j,i))
            {/* Found a row that contains at least one 1 and  */
             /* is covered by row i.                          */
             add(j,covered_row[i]->grset); 
             covered_row[i]->ngrset++;     
             for(k=0; (k<=SPALTEN) && (pattern->ngrset!=0); k++)
               if(testelem(j,tra[k]) && testelem(k,pattern->grset))
                 {sub(k,pattern->grset);
                  pattern->ngrset--;      
                  /* Since a row is compact iff it is the      */ 
                  /* union of some other rows this can be      */
                  /* checked by trying to eliminate all        */
                  /* elements of pattern->grset. This time k   */
                  /* has been found to be covered by row i.    */
                  /* Then k can be deleted from pattern->grset.*/
                 }                      
            }
      if (pattern->ngrset==0)   
      { /* Found a row that is the union of some other rows. */
        add(i,compact_row->grset);
        compact_row->ngrset++;
      }                       
    } 
  }
}
/***************************************************************************/

/***************************************************************************/
/* compute_compact_cols                                                    */
/***************************************************************************/
/* The procedure finds all compact columns and adds their numbers to the   */
/* set compact_col->grset, that is column i is compact iff testelem(i,     */
/* compact_row->grset). In covered_col[i]->grset the numbers of all the    */
/* columns that are covered by column i and contain at least one 1 are     */
/* stored. This is good for the computation of the irreducible automaton.  */
/***************************************************************************/
static void compute_compact_cols()
{ posint i,j,k;          
  ptset pattern; /* Pattern saves the 1's in column i that   */
                 /* are not yet covered.                     */
  
  colsink=SPALTEN+1;
  covered_col=newgsarray(SPALTEN+1);
  for (i=0; i<=SPALTEN; i++)
    newpattern(covered_col[i],nbs);
  newpattern(compact_col,nbs);
  newpattern(pattern,nbz);
  for (i=0; i<=SPALTEN; i++)
  { if (colweight[i]<2)    
    { if (colweight[i]==0)
      { /* If there is no 1 in the column, the column can be */
        /* eliminated and is best seen as compact.           */
        add(i,compact_col->grset);
        compact_col->ngrset++;                                 
        colsink=i;   /* Store the number of the empty state. */
      }
    }   
    else       
    { for (j=0; j<nbz; j++)
        pattern->grset[j]=tra[i][j];
      pattern->ngrset=colweight[i];
      /* Now testelem(j,pattern) iff testelem(j,tra[i]) for  */
      /* every row j.                                        */
      for (j=0; j<=SPALTEN; j++)
      { if (j!=i)
        { if (col_is_covered(j,i))
          { /* Found a column that contains at least one 1   */
            /* and is covered by column i.                   */  
            add(j,covered_col[i]->grset); 
            covered_col[i]->ngrset++;        
            for (k=0; (k<=ZEILEN) && (pattern->ngrset!=0); k++)
            { if (testelem(k,tra[j]) && testelem(k,pattern->grset))
              { sub(k,pattern->grset);
                pattern->ngrset--; 
                /* Since a column is compact iff it is the   */ 
                /* union of some other columns this can be   */
                /* checked by trying to eliminate all        */
                /* elements of pattern->grset. This time k   */
                /* has been found to be covered by column i. */      
                /* Then k can be deleted from pattern->grset.*/
              } 
            }
          }
        }
      }
      if (pattern->ngrset==0)  
      { /* Found a column that is the union of some other    */
        /* columns.                                          */
        add(i,compact_col->grset);
        compact_col->ngrset++;
      } 
    }                        
  }
}
/***************************************************************************/

/***************************************************************************/
/* compute_irreducible_automaton                                           */
/***************************************************************************/
/* This procedure computes the irreducible automaton to da by eliminating  */
/* the compact states of da.                                               */
/***************************************************************************/
static nfa compute_irreducible_automaton(da,compact_states,covered_states,dastates,nastates)
dfa da;
ptset compact_states;
ptset* covered_states;
posint dastates,nastates; /* Number of states - 1 of given   */
                          /* and of irreducible automaton.   */
{ posint i,j,k,l,m;
  nfa na;
  array old2new;

  if (dastates==nastates)return dfa2nfa(da);   /* Nothing to do. */
  old2new=newarray(dastates+1);
  i=j=0;
  while (i<=dastates)
  { while (testelem(i,compact_states->grset))i++;
    old2new[i++]=j++;                                        
  }          
  /* Old2new[i] is the number of the new state if i is not   */
  /* compact and undefined (0) otherwise.                    */
  na=newnfa();
  na->sno=da->sno;
  na->qno=nastates;
  na->delta=newndelta(na->sno,na->qno);
  na->infin=newfinal(na->qno);  
  na->is_eps=na->minimal=FALSE;
  /* Computation of final states. A state that is not        */
  /* compact is final in na iff it is final in da.           */
  for (i=0;i<=dastates;i++)
    if (!testelem(i,compact_states->grset))
      if (isfinal(da->final[i]))
        setfinalT(na->infin[old2new[i]]);
  /* Computation of initial states. A state that is not      */
  /* compact is initial in na iff it is the initial state in */
  /* da or if its rows/columns are covered by the initial    */
  /* states' ones.                                           */
  i=da->init;    /* Initial state of da. */
  if (!testelem(i,compact_states->grset))
  { setinit(na->infin[old2new[i]]);
  }                                 
  /* Now for covered states. */
  for  (j=0,k=covered_states[i]->ngrset; k!=0; j++)
  { if (testelem(j,covered_states[i]->grset))
    { k--;
      if (!testelem(j,compact_states->grset))
      { /* Found another initial state. */
        setinit(na->infin[old2new[j]]);
      }
    }
  }
  /* Computation of na->delta. */
  for (i=0;i<=dastates;i++)
  { if (!testelem(i,compact_states->grset))
    { /* Only for states that are not compact. */
      for (j=1;j<=da->sno;j++)
      { k=da->delta[j][i];
        if (testelem(k,compact_states->grset))
        { /* K is compact. Add m to delta[j][i] iff m is     */
          /* covered by k and is not compact.                */
          for (m=0,l=covered_states[k]->ngrset; l!=0; m++)
          { if (testelem(m,covered_states[k]->grset))
            { l--;
              if (!testelem(m,compact_states->grset))
              { connect(na->delta,j,old2new[i],old2new[m]);
              }
            }
          }
        }
        else
        { /* K is not compact. Simply join old2new[k] to     */
          /* delta[j][i].                                    */
          connect(na->delta,j,old2new[i],old2new[k]);
        }          
      }
    }
  }
  return na;
}
/***************************************************************************/

/***************************************************************************/
/* calculate_necessary_grids                                               */
/***************************************************************************/
/* Calculate_necessary_grids finds all grids that cover a 1 that is the    */
/* only one in both a row and a column. These grids are essential          */
/* (necessary). The rows resp. columns containing such a 1 are not needed  */
/* when calculating the other grids. Their numbers are stored in           */
/* ones->grset and their number is ones->ngrset.                           */
/***************************************************************************/
static void calculate_necessary_grids()
{ posint i,j;
  boole test;

  newpattern(ones,nbs);
  newcover(grids,nbs); /* List of grids. The memory for the  */
                       /* first grid has already been        */
                       /* fetched.                           */
  firstI=grids->glist; /* FirstI->info is the first grid.    */ 
  if (V)
  { /* Calculate with rows. First try to find a row with     */
    /* rowweight==1.                                         */
    for (i=0; i<=ZEILEN; i++)
    { if (rowweight[i]==1)
      { /* Found such a row. Now look for the column. */
        test=FALSE;
        j=0;    
        while ((!test) && (j<=SPALTEN))
        { if (testelem(i,tra[j]))
            test=TRUE;
          else 
            j++;
        }  
        if ((test) && (colweight[j]==1))
        { /* The 1 stands alone in row i and column j. Now   */
          /* define a new grid.                              */
          ones->ngrset++;
          add(i,ones->grset); /* Store the number of the row.*/
          if (grids->sizeofglist!=0)
          { /* A new gridlist is needed. */
            newgridlist(grids->glist->next,nbs);
            grids->glist=grids->glist->next;
          }
          grids->sizeofglist++;
          grids->glist->info->ncol=1;    
          add(i,grids->glist->info->col);       
        }
      }
    }
  }
  else
  { /* Calculate with columns. First try to find a column    */
    /* with colweight==1.                                    */
    for (i=0; i<=SPALTEN; i++)
    { if (colweight[i]==1)
      { /* Found such a column. Now look for the row. */
        test=FALSE;
        j=0;    
        while ((!test) && (j<=ZEILEN))
        { if (testelem(j,tra[i]))
            test=TRUE;
          else 
            j++;
        }
        if ((test) && (rowweight[j]==1))
        { /* The 1 stands alone in row j and column i. Now   */
          /* define a new grid.                              */
          ones->ngrset++;
          add(i,ones->grset); /* Store the number */
                              /* of the column.   */
          if (grids->sizeofglist!=0)
          { /* A new gridlist is needed. */
            newgridlist(grids->glist->next,nbs);
            grids->glist=grids->glist->next;
          }
          grids->sizeofglist++;
          grids->glist->info->ncol=1;    
          add(i,grids->glist->info->col);       
        }
      }
    }
  }    
  NECESSARY=grids->sizeofglist;                    
  /* NECESSARY is the number of the essential grids.  */
}
/***************************************************************************/

/***************************************************************************/
/* calculate_grids_r                                                       */
/***************************************************************************/
/* This procedure calculates the other grids to be looked at. This is done */
/* by testing all possible patterns of the rows whose numbers are not      */
/* element of one->grset and contain at least one 1.                       */
/***************************************************************************/
static void calculate_grids_r()
{ posint i,j,k,n,m,Z,nbm;
  array new2old; /* New2old assigns to all rows of the       */
                 /* "smaller" RAM the corresponding row of   */
                 /* the original RAM.                        */
  ptset pattern; /* Pattern of rows. */
  boole testI,testII,easy;
                         
  easy=FALSE;
  Z=ZEILEN-ones->ngrset;     
  if (rowsink!=ZEILEN+1)
    Z--;
  REST=0; /* No non-essential grid so far. */
  new2old=newarray(Z+1);
  nbm=(Z+1)/SIZEOFBYTE + 1;
  j=0;
  for (i=0; i<=Z; i++)
  { while (testelem(j,ones->grset) && (j<=ZEILEN))
      j++; /* There is no need to look at row j. */
    if (j==rowsink)
      j++; /* No need to look at the row for the empty state.*/
    new2old[i]=j++;
  }                     
  i=0;
  while ((i<=SPALTEN) && (!easy))
  { if (colweight[i]==Z+1)
      easy=TRUE; /* There is a column in the smaller RAM     */
                 /* containing Z+1 1's. Then all sets of     */
                 /* rows of the smaller RAM are used for the */
                 /* grids.                                   */
    else
      i++;
  }
  newpattern(pattern,nbm);
  pattern->ngrset=1;
  k=0;
  add(k,pattern->grset);
  if (easy)
  { /* Make a grid out of every pattern. */
    while (k<=Z)
    { add_to_list(pattern,new2old);             
      k=0;
      /* Calculate next pattern. */
      while (testelem(k,pattern->grset) && (k<=Z))
      { sub(k,pattern->grset);
        pattern->ngrset--;   
        k++;
      }                            
      /* If k==Z all patterns have been checked. */
      if (k<=Z)
      { add(k,pattern->grset);
        pattern->ngrset++;
      }
    }                                                        
  }
  else
  { while (k<=Z)
    { /* Test if there is a column defining a grid together  */
      /* with the set of rows.                               */
      testI=FALSE;
      i=0;
      while ((i<=SPALTEN) && (!testI))
      { if (colweight[i]>=pattern->ngrset)
        { n=0;
          testII=TRUE;
          m=pattern->ngrset;
          while ((m!=0) && testII)
          { if (testelem(n,pattern->grset))
            { m--;
              if (!testelem(new2old[n],tra[i]))
                testII=FALSE;
            }
            n++;
          }
        }        
        else
          testII=FALSE;       
        if (testII)
          testI=TRUE; /* Found such a column. */                   
        else
          i++;
      }  
      if (testI)
      { add_to_list(pattern,new2old);             
        k=0;
      }
      else
      { /* If no such column was found not all of the        */
        /* remaining patterns (sets of rows) have to be      */
        /* looked at.                                        */
        k=0; 
        while (!testelem(k,pattern->grset))
          k++;       
        /* K is the minimal i that is an element of          */
        /* pattern->grset.                                   */
      }                                                       
      /* Calculate next pattern. */
      while (testelem(k,pattern->grset) && (k<=Z))
      { sub(k,pattern->grset);
        pattern->ngrset--;   
        k++;
      }                            
      /* If k==Z all patterns have been checked. */
      if (k<=Z)
      { add(k,pattern->grset);
        pattern->ngrset++;
      }
    }                                                        
  }
  GITTER=grids->sizeofglist; /* GITTER=number of grids. */
  nbg=(GITTER)/SIZEOFBYTE + 1; 
}
/***************************************************************************/

/***************************************************************************/
/* calculate_grids_c                                                       */
/***************************************************************************/
/* This procedure calculates the other grids to be looked at. This is done */
/* by testing all possible patterns of the columns whose numbers are not   */
/* element of one->grset and contain at least one 1.                       */
/***************************************************************************/
static void calculate_grids_c()
{ posint i,j,k,n,m,S,nbm;
  array new2old; /* New2old assigns to all columns of the    */
                 /* "smaller" RAM the corresponding column   */
                 /* of the original RAM.                     */
  ptset pattern; /* Pattern of columns. */
  boole testI,testII,easy;
                         
  easy=FALSE;
  S=SPALTEN-ones->ngrset;
  if (colsink!=SPALTEN+1)
    S--;
  REST=0; /* No non-essential grid so far. */
  new2old=newarray(S+1);
  nbm=(S+1)/SIZEOFBYTE + 1;
  j=0;
  for (i=0; i<=S; i++)
  { while (testelem(j,ones->grset) && (j<=SPALTEN))
      j++; /* There is no need to look at column j. */
    if (j==colsink)
      j++; /* No need to look at the column for the */
           /* empty state.                          */
    new2old[i]=j++;
  }
  i=0;
  while ((i<=ZEILEN) && (!easy))
  { if (rowweight[i]==S+1)
      easy=TRUE; /* There is a row in the smaller RAM        */
                 /* containing S+1 1's. Then all sets of     */
                 /* columns of the smaller RAM are used for  */
                 /* the grids.                               */
    else
      i++;
  }
  newpattern(pattern,nbm);
  pattern->ngrset=1;
  k=0;
  add(k,pattern->grset);
  if (easy)
  { /* Make a grid out of every pattern. */
    while (k<=S)
    { add_to_list(pattern,new2old);             
      k=0;                         
      /* Calculate next pattern. */
      while (testelem(k,pattern->grset) && (k<=S))
      { sub(k,pattern->grset);
        pattern->ngrset--;   
        k++;
      }                            
      /* If k==S all patterns have been checked. */
      if (k<=S)
      { add(k,pattern->grset);
        pattern->ngrset++;
      }          
    }
  }
  else
  { while (k<=S)
    { /* Test if there is a row defining a grid together     */
      /* with the set of columns.                            */
      testI=FALSE;
      i=0;
      while ((i<=ZEILEN) && (!testI))
      { if (rowweight[i]>=pattern->ngrset)
        { n=0;
          testII=TRUE;
          m=pattern->ngrset;
          while ((m!=0) && testII)
          { if (testelem(n,pattern->grset))
            { m--;
              if (!testelem(i,tra[new2old[n]]))
                testII=FALSE;
            }
            n++;
          }
        } 
        else
          testII=FALSE;       
        if (testII)
          testI=TRUE; /* Found such a row. */                   
        else
          i++;
      }  
      if (testI)
      { add_to_list(pattern,new2old);             
        k=0;
      }
      else
      { /* If no such row was found not all of the remaining */
        /* patterns (sets of columns) have to be looked at.  */
        k=0; 
        while (!testelem(k,pattern->grset))
          k++;       
        /* K is the minimal i that is an element of          */
        /* pattern->grset.                                   */
      }                                                       
      /* Calculate next pattern. */
      while (testelem(k,pattern->grset) && (k<=S))
      { sub(k,pattern->grset);
        pattern->ngrset--;   
        k++;
      }                            
      /* If k==S all patterns have been checked. */
      if (k<=S)
      { add(k,pattern->grset);
        pattern->ngrset++;
      }          
    }
  }                                                        
  GITTER=grids->sizeofglist; /* GITTER=number of grids. */
  nbg=(GITTER)/SIZEOFBYTE + 1; 
}
/***************************************************************************/

/***************************************************************************/
/* add_to_list                                                             */
/***************************************************************************/
/* Add_to_list adds a new grid as the last element to the list of grids.   */
/***************************************************************************/
static void add_to_list(pattern,cols)
ptset pattern; /* Pattern is a ptset used for storing the    */
array cols;    /* numbers of the columns resp. rows of a     */
               /* grid over the RAM without the columns resp.*/
               /* rows that contain a 1 standing alone in    */
               /* both its row and its column and without    */
               /* the empty state. Cols assigns each element */
               /* of pattern the number of the column (row)  */
               /* of the original RAM.                       */
{ posint i,j;

  if (grids->sizeofglist!=0)
  { /* A new gridlist has to be defined. */
    newgridlist(grids->glist->next,nbs);
    grids->glist=grids->glist->next;
  }             
  if (REST==0)
    firstII=grids->glist; /* FirstII->info is the first */
                          /* non-essential grid.        */
  grids->sizeofglist++;   /* One more grid. */  
  REST++;                 /* One more non-essential grid. */  
  /* Copy information about columns. */
  grids->glist->info->ncol=i=pattern->ngrset;
  for (j=0; i!=0; j++)
  { if (testelem(j,pattern->grset))
    { /* Found a column. */
      i--;
      add(cols[j],grids->glist->info->col);
    }
  }                                            
}
/***************************************************************************/    

/***************************************************************************/
/* fundamental_table_calculation                                           */
/***************************************************************************/
/* Fundamental_table_calculation computes the initial and final states     */
/* (grids) and delta of the fundamental table.                             */
/***************************************************************************/
static void fundamental_table_calculation(f)
nfa f;
{ posint i,j;
  gridlist start;

  newpattern(initial_grids,nbg); /* For saving if a grid     */
  newpattern(final_grids,nbg);   /* corresponds to an        */
                                 /* initial or a final state */
                                 /* of the fundamental table.*/
  delta=newmdelta(f->sno);
  for (i=0; i<f->sno; i++)
  { delta[i]=newgsarray(GITTER);
    for (j=0; j<GITTER; j++)  
      newpattern(delta[i][j],nbs);
  }                                     
  /* Thus, delta[i][j] for i a letter and j a grid is a      */
  /* ptset with the set containing at most ZEILEN resp.      */
  /* SPALTEN elements.                                       */
  /* Now calculate initial grids and final grids.            */
  for (i=0,start=firstI; i<GITTER; i++,start=start->next)
  { if (is_initial(start->info,f))
    { add(i,initial_grids->grset);
      initial_grids->ngrset++;
    }
    if (is_final(start->info,f))
    { add(i,final_grids->grset);
      final_grids->ngrset++;
    }
  }
  /* Finally calculation of delta. */
  fun_tab_delta_calculation(f);      
}

/***************************************************************************/
/* fun_tab_delta_calculation                                               */
/***************************************************************************/
/* This procedure calculates delta of the fundamental table.               */
/***************************************************************************/
static void fun_tab_delta_calculation(f)
nfa f;
{ posint i,j,k,l,m,S;
  gridlist start;                       
  if (V)  S=ZEILEN;
  else    S=SPALTEN;
  for (i=0,start=firstI; i!=GITTER; i++,start=start->next)
    for(j=0; j<f->sno; j++)
      for(l=0,k=start->info->ncol; k!=0; l++)
        if(testelem(l,start->info->col))
          {k--;
           for(m=0; m<=S; m++)
             if(testcon(f->delta,(j+1),m,l) && (!testelem(m,delta[j][i]->grset)))
               {add(m,delta[j][i]->grset);
                delta[j][i]->ngrset++;
                /* Grid i contains column l and the dual       */
                /* automaton to f has an edge between the      */
                /* states l and m marked with letter j+1. Then */
                /* m has to be added to the set of delta[j][i].*/
               }
          }
}    

/***************************************************************************/
/* is_initial                                                              */
/***************************************************************************/
/* The procedure returns TRUE iff the grid help - seen as a state of the   */
/* fundamental table - is an initial state. This is so iff all of help's   */
/* columns are final states in the irreducible automaton f.                */
/***************************************************************************/
static boole is_initial(help,f)
grid help;          
nfa f;
{ posint i,j;
  for(j=0,i=help->ncol; i!=0; j++)
    if(testelem(j,help->col))
      {i--;
       if (!isfinal(f->infin[j])) return FALSE; 
       /* Help is not an initial state. */
      }
  return TRUE;
}

/***************************************************************************/
/* is_final                                                                */
/***************************************************************************/
/* The procedure returns TRUE iff the grid help - seen as a state of the   */
/* fundamental table - is a final state. This is so iff there exists a     */
/* column in the grid so that the corresponding state in the irreducible   */
/* automaton f is an initial state.                                        */
/***************************************************************************/
static boole is_final(help,f)
grid help;        
nfa f;
{ posint i,j;
  for(j=0,i=help->ncol; i!=0; j++)
    if(testelem(j,help->col))
      {i--;
       if (isinit(f->infin[j])) return TRUE; 
       /* Help is a final state. */
      }
  return FALSE;
}
