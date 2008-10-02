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

/** \file
 *  \brief contains test
 */
#include "testUnary.h"

#include "dfa2mon.h" 
#include "mon2dcl.h" 

/* functions included :
 * 	void deadsearch()
 *	boole folutest()
 *	static boole test_htrivial()
 *	static boole test_groupfree()
 *	boole sftest()
 *	static boole testleftzero()
 *	static boole testrightzero()
 *	static void idemcompute()
 *	static boolx eSetest(e)
 *	void proptest()   
 *	boole testpwt()
 *	boole testdd1()
 */
/******************************************************************/
static monoid stamon;
/* arrays used in eSetest */
static array eSeplace;
static b_array eSemark;
/* variables used in idemcompute */
static b_array testidem;
static array idem;
static posint idemcount;
/* variables used in minlocaltest */
static posint maxrank;             /* least n such that S^n == S^{n+1} */
static array sort;
static array startrank;
/******************************************************************/
/******************************************************************/

static void S_to_i()
/* compute for each s in S the maximum i such that s in S^i
 * O(( |S^1| + |S^2| .. + |S^maxn|) * number of generators) in  O(|S|^2 * number of generators)
 */
{
 posint s,pos,element,gen;
 array old,new,change;
 array s_to_i;
 posint number,oldnumber; /* size of S^{i+1} , S^i */
 s_to_i=newarray(stamon->mno);
 old=newarray(stamon->mno);
 new=newarray(stamon->mno+1);
 for(s=1,pos=0;s<stamon->mno;s++,pos++)
   {
    new[pos]=s;
    s_to_i[s]=1;
   }
 oldnumber=stamon->mno;
 number=stamon->mno-1;
 maxrank=1;
 while(number<oldnumber)
   {
    maxrank++;
    oldnumber=number;
    number=0;
    change=old;  /* change new and old */
    old=new;
    new=change;
    for(pos=0;pos<oldnumber;pos++) /* for all elements in old */
      for(gen=1;gen<=stamon->gno;gen++) /* for all generators */
         {
          element=stamon->gensucc[old[pos]][gen];
          if(s_to_i[element]<maxrank)
            {
             new[number]=element;
             number++;
             s_to_i[element]=maxrank;
            }
          }
   } 
 /* maxrank == n+1 for the least n such that S^n == S^{n+1} */
 maxrank--;
 for(pos=0;pos<number;pos++)
    s_to_i[old[pos]]=maxrank;
 sort=old;                                     /* use old space */
 startrank=new;
 for(number=0;number<=maxrank+1;number++)      /* set zero startrank */ 
    startrank[number]=0;
 for(element=1;element<stamon->mno;element++)  /* count elements of rank i=1 to maxrank */ 
    startrank[s_to_i[element]]++;
 for(number=1;number<maxrank;number++)         /* compute last position of elements of rank i in sort */
    startrank[number+1]+=startrank[number];
 startrank[maxrank+1]=stamon->mno-1;
 for(element=1;element<stamon->mno;element++)  /* fill sort */
    {
     number=s_to_i[element];
     startrank[number]--;
     pos=startrank[number];
     sort[pos]=element;
    }
}

/******************************************************************/

static posint s2i(element,power)
posint element,power;
{
 posint help,result;
 help=element;
 result=0;
 while(power)
     {
      if(power%2)  /* power is odd */
           result=mult(stamon,result,help);
      help=mult(stamon,help,help);
      power=power/2;
     }
  return result;
}

static posint minlocaltest()
{
 boole holds;
 posint n;
 posint s,y,z,sy,sz,t,st,sts;
 posint sysz,szsy;
 posint m,r,p,k;
 posint i,j; /* positions in sort */
 /* (DISABLED be) help(FALSE,"computing degree of locally testable"); */
 S_to_i();
 /* compute minimal n such that the first equation holds */
 holds=FALSE;
 n=0;
 while(!holds) 
   {
    n++;
    holds=TRUE;
    for(i=startrank[n];(i<startrank[n+1])&&(holds);i++) /* for all elements with rank n .. */
      {
       s=sort[i];
       for(y=1;(y<stamon->mno)&&(holds);y++)
          for(z=1;(z<stamon->mno)&&(holds);z++)
             {
              sy=mult(stamon,s,y);
              sz=mult(stamon,s,z);
              sysz=mult(stamon,sy,sz);
              szsy=mult(stamon,sz,sy);
              holds=(mult(stamon,sysz,s)==mult(stamon,szsy,s));
             } 
      }
   }
 n--;
 /* n is the last rank where the first equation not holds */
 holds=FALSE;
 while(!holds) 
   {
    n++;
    holds=TRUE;
    for(r=1;(r<=n+1)&&(holds);r++)
      {
       m = n / r;
       p = n % r;
       if(p>maxrank)p=maxrank;
       k = r-p;
       if(k>maxrank)k=maxrank;
       if(p!=0)
        {
         for(i=startrank[p];(i<(stamon->mno-1))&&(holds);i++)
           {
            s=sort[i];
            for(j=startrank[k];(j<(stamon->mno-1))&&(holds);j++)
              {
               t=sort[j];
               st=mult(stamon,s,t);
               sts=mult(stamon,s2i(st,m+1),s);
               holds=(mult(stamon,st,sts)==sts);
              }
            }
         }
       else
         for(j=startrank[k];(j<(stamon->mno-1))&&(holds);j++)
           {
            t=sort[j];
            st=s2i(t,m+1);
            holds=(st==mult(stamon,st,t));
           }
      }
    }    
 /* help(FALSE,"Please enter only one of the uppercase letters from the window headlines"); */
 return(n);
} 


void deadsearch(dfa indfa,posint *deadpos, posint *deadneg, 
                boole *pos, boole *neg) {
  posint state,l;
  *pos=FALSE;*neg=FALSE;
  for (state=0;state<=indfa->qno;state++)
    {for(l=1;(l<=indfa->sno)&&(state==indfa->delta[l][state]);l++);
    if(l>indfa->sno)  {               /* deadstate found */
      if(indfa->final[state]) 
        {*deadpos=state;*pos=TRUE;}
      else                    
        {*deadneg=state;*neg=TRUE;}
    }
    }
}


boole folutest(dfa indfa)
{posint l,l1,state,state1,deadstate;
 dfa testdfa;
 monoid mon;
 boole test,test1; /* test <=> indfa has a nonfinal sink state */
 deadsearch(indfa,&state1,&state,&test1,&test);
 deadstate=(test)? state:indfa->qno+1;

 /* define new automaton, 
  * alphabet A x {0,1}     stateset Q + {deadstate}
  * coding of alphabet:   (i,0) -> i  and (i,1) -> indfa->sno+i
  * (q,a,q') and  q notin F => (q, (a,0) , q')  
  * (q,a,q') and  q    in F => (q, (a,0) , deadstate)  
  * (q,a,q') and  q    in F => (q, (a,1) , q')  
  * (q,a,q') and  q notin F => (q, (a,1) , deadstate)  
  *
  * L(indfa) in FOL_U iff new automaton permutationfree
  */
 testdfa=newdfa();
 testdfa->sno=2*indfa->sno;
 testdfa->init=indfa->init;
 testdfa->qno=(test)? indfa->qno : indfa->qno+1;
 testdfa->delta=newddelta(testdfa->sno,testdfa->qno);
 testdfa->final=newfinal(testdfa->qno);
 if(!test)
   {
    for(l=1;l<=testdfa->sno;l++)testdfa->delta[l][deadstate]=deadstate;
    testdfa->final[deadstate]=FALSE;
   }
 for(state=0;state<=indfa->qno;state++)
  {testdfa->final[state]=indfa->final[state];
   for(l=1,l1=indfa->sno+1;l<=indfa->sno;l++,l1++)
     {
      state1=indfa->delta[l][state]; /* abbreviation */
      if(indfa->final[state1])
         {testdfa->delta[l][state]=deadstate;
          testdfa->delta[l1][state]=state1;}
      else
         {testdfa->delta[l1][state]=deadstate;
          testdfa->delta[l][state]=state1;} 
     }
  }
 mon=dfa2mon(testdfa);
 test1=sftest(mon);  
 freedfa(testdfa);
 dispose(testdfa);
 freemon(mon);
 dispose(mon);
 return(test1);
}


static boole test_htrivial(mon)
/* a monoid is H-trivial iff every hclass consists of only one element */
monoid mon;
{posint run=0;
 while(run<mon->ds->dno)
  if(mon->ds->dclassarray[run++]->hsize!=1)return(FALSE);
 return(TRUE);
}

static boole test_groupfree(mon)
/*   O(|Q|.|M|)
 *    an element x is member of a group
 * <=>image(x) is a transversale of kernel(x)
 * <=>two different elements of the image have different images
 */
monoid mon;
{
 boole group;
 b_array image; /* mark elements in the image */
 array help;/* contains the elements of the image */
 posint i,j,rang;
 arrayofarray a=mon->no2trans; /* abbreviation */
 image=newb_array(mon->qno+1); 
 help=newarray(mon->qno+1); 
 for(i=0;i<mon->mno;i++) /* for all m in M .. */
   {rang=0;
    group=TRUE;
    /* compute image and rang */
    for(j=0;j<=mon->qno;j++)
      if(!image[a[i][j]])      /* new element in image */
       {help[rang++]=a[i][j];
	image[a[i][j]]=TRUE;
       }
    /* help[0] .. help[rang-1] is the image
     * test transversale
     * compute the images of all images and delete this elements in image
     * if an image is yet deleted then two images have the same image
     */
    for(j=0;j<rang;j++)
       if(!image[a[i][help[j]]])    /* two images have the same image */
	 {group=FALSE; break;}
       else
	 image[a[i][help[j]]]=FALSE;
    if(group&&(!idempotent(i,mon))) /* i is an element of a nontrivial group */
       return(FALSE);
    for(j=0;j<rang;j++)             /* set zero image and help */
       {image[help[j]]=FALSE;help[j]=0;}
   }
 return(TRUE);
}

/*     a monoid M is aperiodic
 * <=> M is H-trivial
 * <=> M is groupfree (contains only trivial groups)
 *
 *     L is starfree
 * <=> the syntactical monoid of L is aperiodic
 */
boole sftest(monoid mon) {
 if(mon->dclassiscomputed)return(test_htrivial(mon));
 else return(test_groupfree(mon));
}


static boole testleftzero(no)
posint no;
/* O(|A|)
 * testleftzero(no) <=> no*a=no for all generators a
 *                  <=> no*S=no
 */
{
 posint i;
 for(i=1;i<=stamon->gno;i++)
   if(stamon->gensucc[no][i]!=no)return(FALSE);
 return(TRUE);
}


static boole testrightzero(no)
posint no;
/* O(|A|*||no||)
 * testrightzero(no) <=> a*no=no for all generators a
 *                   <=> S*no=no
 */
{
 posint i;
 for(i=1;i<=stamon->gno;i++)
   if(mult(stamon,i,no)!=no)return(FALSE);
 return(TRUE);
}


static void idemcompute()
/* O(|M|*|Q|)
 * computes all idempotent elements of the syntactic semigroup
 * results in idemcount  number of idempotent elements
 *            idem       array which contains all idempotents
 *            testidem   testidem[i] <=> i is idempotent
 */
{
 posint elem;
 posint begin;
 idemcount=0;
 begin=(stamon->mequals)? 0 : 1;
 testidem=newb_array(stamon->mno);
 idem=newarray(stamon->mno);
 for(elem=begin;elem<stamon->mno;elem++)
    if(idempotent(elem,stamon))
	 {
	  testidem[elem]=TRUE;
	  idem[idemcount++]=elem;
	 }
}

static boolx eSetest(e)
/* O(|eS|*MULT)
 * 
 * esetest(e)  == 0  <=> eSe == e
 *             == 1  <=> eSe != e  , eSe is commutative and idempotent
 *             == 2  <=> eSe != e  , ! eSe is commutative and idempotent
 */
posint e;
{register posint i;
 register posint count=1; /* counts the number of elements in eSe */
 posint run=0,j;
 eSemark[e]=TRUE;
 eSeplace[0]=e;
 /* compute eS */
 while(run!=count)
   {for(i=1;i<=stamon->gno;i++)
      {j=stamon->gensucc[eSeplace[run]][i];
      if(!eSemark[j])
	 {eSeplace[count++]=j;
	  eSemark[j]=TRUE;
	  }
      }
    run++;
   }
 /* eSeplace[0] .. eSeplace[count-1] = eSe */
 /* setzero eSemark */
 for(i=0;i<count;i++) eSemark[eSeplace[i]]=FALSE;
  /* compute eSe |eSe|<=|eS| */
 run=0;
 for(i=0;i<count;i++)
    {j=mult(stamon,eSeplace[i],e);
     if(!eSemark[j])
	{
	 eSeplace[run++]=j;
	 eSemark[j]=TRUE;
	}
     }
 /* eSeplace[0] . . eSeplace[run-1] contains eSe */
 /* setzero eSemark */
 for(i=0;i<run;i++) eSemark[eSeplace[i]]=FALSE;
 if(run==1) return(0); /* eSe=e*/
 else
  /* test eSe idempotent and commutative */
   {
    for(j=0;j<run;j++)
     /* for all elements in eSe test idempotent */
      if(!testidem[eSeplace[j]])return(2);
     /* test commutative */
    for(j=0;j<run;j++)
      for(i=j+1;i<run;i++)
	if(!(mult(stamon,eSeplace[i],eSeplace[j])==mult(stamon,eSeplace[j],eSeplace[i])))return(2);
    return(1); /* eSe is commutative and idempotent */
   }
}


/* test membership of the syntactic semigroup in several +-Varieties
 *
 * L(S) is finite or cofinite   <=>  eS=Se=e for all idempotent e
 *         definite             <=>  Se  = e   for all idempotent e
 *         reverse definite     <=>  eS  = e   for all idempotent e
 *         generalized definite <=>  eSe = e   for all idempotent e
 *         local testable       <=>  eSe is idempotent and commutative
 *                                          for all idempotent e
 *
 *  nilpotent => definite,revdefinite,gendefinite,localtest
 *  definite or revdefinite => gendefinite => localtest
 *
 * if mon->mequals and mon->starfree then
 *   there is a letter whose transformation is the identity
 *   in this case L can`t be nilpotent,definite,revdefinite,gendefinite
 *
 * if stamon->mno==1 then L = A* or L = 0 
 */

void proptest(language lan, monoid mon)   
{posint i,j;
 boole test;
 b_array halfzero;
 stamon=mon;
 test=((stamon->mno==1) || (!stamon->mequals));
 lan->nilpotent=test;
 lan->definite=test;
 lan->revdefinite=test;
 lan->gendefinite=test;
 lan->localtest=test;
 if(stamon->mno==1)return; /* all flags are TRUE */
 if(stamon->mequals) /* all flags are FALSE */
   /*     L local testable
    * iff S is idempotent and commutative
    * iff the set of generators is idempotent and commutative
    */
   {
    for(i=1;i<=stamon->gno;i++)
       if(stamon->gensucc[i][i]!=i) return;
    for(i=1;i<=stamon->gno;i++)
      for(j=i+1;j<=stamon->gno;j++)
	if(stamon->gensucc[i][j]!=stamon->gensucc[j][i]) return;
    lan->localtest=TRUE;
    return;
   }
 /* all flags are TRUE */
 idemcompute();
 if(idemcount>=2)
     lan->nilpotent=FALSE;
 else 
     if(!(testrightzero(idem[0])&&testleftzero(idem[0])))
          lan->nilpotent=FALSE;
 if(!lan->nilpotent)
   {
    halfzero=newb_array(idemcount);
     /* halfzero[i] <=> idem[i] is a right or left zero */
    for(i=0;i<idemcount;i++)
      {if(testrightzero(idem[i])) halfzero[i]=TRUE;
       else lan->definite=FALSE;
       if(testleftzero(idem[i])) halfzero[i]=TRUE;
       else lan->revdefinite=FALSE;
      }
    if(!(lan->definite||lan->revdefinite))
      {
       /* get free memory for eSetest */
       eSemark=newb_array(stamon->mno);
       eSeplace=newarray(stamon->mno); /* free memory for eSetest */
       for(i=0;i<idemcount;i++)
	  if(!halfzero[i]) /* e is a halfzero => eSe = e */
	    {
	     test=eSetest(idem[i]);
	     if(test)   /* eSe != e */
		lan->gendefinite=FALSE;
	     if(test==2) /* eSe not commutative or not idempotent */
		{lan->localtest=FALSE;
                 break;
                }
                   
	    }
      } 
   } /* end if !lan->nilpotent */
 if(lan->localtest)
   { lan->localdegree=lan->lmon->mno; /* means localdegree unknown */
     lan->localdegree=minlocaltest();
    }
 freebuf();
}


/*     * - Varietie
 *      L is piecewiese testable
 *   <=>M(L) is D trivial (size of each dclass is 1)
 */
boole testpwt(monoid mon)
{if(!mon->dclassiscomputed)mon2dcl(mon);
 return(mon->mno==mon->ds->dno);
}

/* + - V 
 *       L has dot depth <=1
 *  <=>
 *       for all idempotent elements e and id2
 *       and all elements i,j,k,l in S=S(L)
 *         (e*i*f*j)^g * e*i*f*l*e * (k*f*l*e)^g
 *       =
 *         (e*i*f*j)^g * e * (k*f*l*e)^g
 */
boole testdd1(monoid mon) {
 posint begin;
 posint i,j;
 posint k,l,id1,id2;
 posint help1,help2,help3,help4,help5,help6,help7,help8,help9,help10,help11;
 array mnopower,help;
 posint faktor;
 stamon=mon;
 if(stamon->mno==1)return(TRUE);
 begin=(stamon->mequals)? 0 : 1;
 faktor=stamon->mno-begin;
    /* compute idempotent elements */
 idemcompute();
    /* computation of the faktor-power of each element */
 mnopower=newarray(stamon->mno);
 help=newarray(stamon->mno);
 /* help[i]=i^1  mnopower[i]=i^0 */
 for(i=0;i<stamon->mno;i++)
   {help[i]=i;
    mnopower[i]=0;
   }
 while(faktor)
     {
      if(faktor!=((faktor/2)*2))  /* faktor is odd */
	  for(i=0;i<stamon->mno;i++)
		 mnopower[i]=mult(stamon,mnopower[i],help[i]);
      for(i=0;i<stamon->mno;i++)help[i]=mult(stamon,help[i],help[i]);
      faktor=faktor/2;
     }
 /* test condition 
 *         (e*i*f*j)^g * e*i*f*l*e * (k*f*l*e)^g
 *       =
 *         (e*i*f*j)^g * e * (k*f*l*e)^g
 *   e,f idempotent i,j,k,l halfgroupelements 
 */
 for(id1=0;id1<idemcount;id1++)
   for(i=begin;i<stamon->mno;i++)
     { help1=mult(stamon,idem[id1],i);                 /* help1=e*i */
       for(l=begin;l<stamon->mno;l++)
         { help2=mult(stamon,l,idem[id1]);             /* help2=l*e */
           for(id2=0;id2<idemcount;id2++)
             { help3=mult(stamon,idem[id2],help2);     /* help3=f*l*e */
               help4=mult(stamon,help1,idem[id2]);     /* help4=e*i*f */
               help5=mult(stamon,help1,help3);         /* help5=e*i*f*l*e */
               for(j=begin;j<stamon->mno;j++)
                  { help6=mult(stamon,help4,j);        /* help6=e*i*f*j */
                    help6=mnopower[help6];             /* help6=(e*i*f*j)^g */
                    help7=mult(stamon,help6,help5);    /* help7=(e*i*f*j)^g * e*i*f*l*e */
                    help8=mult(stamon,help6,idem[id1]);/* help8=(e*i*f*j)^g * e */
                    for(k=begin;k<stamon->mno;k++)
	              { help9=mult(stamon,k,help3);    /* help3=k*f*l*e */
                        help9=mnopower[help9];         /* help3=(k*f*l*e)^g */
                        help10=mult(stamon,help7,help9);
                        help11=mult(stamon,help8,help9);
	                if(help10!=help11)
	                   {freebuf();return(FALSE);}
                       }
                  }
             }
         }
      }
freebuf();
return(TRUE);
}
