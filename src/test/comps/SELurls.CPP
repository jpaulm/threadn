  #include <stdlib.h>
  #include <string.h>
  #include <stdio.h>
  #include "compsvcs.h"

    #define TC(label,ch)  if (i_ptr >= es_ptr) goto eos;\
                     if (*i_ptr != ch) goto label; \
                     *o_ptr = *i_ptr; o_ptr++;  \
                     i_ptr++;

    #define TCO(label,ch)  if (i_ptr >= es_ptr) goto eos;\
                     if (*i_ptr != ch) goto label;  \
                     i_ptr++;

    #define TA(label)    if (i_ptr >= es_ptr) goto eos;\
                     if (!isalpha(*i_ptr)) goto label; \
                     *o_ptr = *i_ptr; o_ptr++;  \
                     i_ptr++;

    #define TAO(label)   if (i_ptr >= es_ptr) goto eos;\
                     if (!isalpha(*i_ptr)) goto label; \
                     i_ptr++;

    #define TN(label)    if (i_ptr >= es_ptr) goto eos;\
                     if (!isdigit(*i_ptr)) goto label; \
                     *o_ptr = *i_ptr; o_ptr++;  \
                     i_ptr++;

    #define TNO(label)   if (i_ptr >= es_ptr) goto eos;\
                     if (!isdigit(*i_ptr)) goto label; \
                     i_ptr++;

    #define CC       if (i_ptr >= es_ptr) goto eos;\
                     *o_ptr = *i_ptr; o_ptr++;  \
                     i_ptr++;

    #define SC       if (i_ptr >= es_ptr) goto eos;\
                     i_ptr++;

    #define TRUE 1
    #define FALSE 0
    
    
   void getWikiName (char * p, int sz);
   int isitint();

   char * WikiNameList[100];

   char WikiName [60];

   int kno = 0;


  THRCOMP SELURLS(anchor proc_anchor)
{
  void *ptr;
  void *ptrn;
  char * p2;
  
  int value;
  long size;
  char *type;
  port_ent port_tab[3];
  
  
  
  /* ports: INT - list of interesting WikiNames    */ 
  /*        IN  - output of RecentChanges          */ 
  /*        OUT - list of lines to be diaplayed    */ 

 
  value = dfsdfpt(proc_anchor, 3, port_tab, "INT", "IN","OUT");

/* build list of interesting URLs                */
  

/* process input entities */
  value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
  while (value == 0) {
      getWikiName((char *) ptr, size);
      printf(WikiName);
      
      value = dfscrep(proc_anchor, &ptrn, size + strlen("<b>  ***  </b>") + 1, "B");
      strncpy((char *) ptrn, (char *) ptr, size);
      p2 = (char *) ptrn + size;

      if  (isitint) 
	  strcpy(p2, "<b>  ***  </b>" );
      else
	  strcpy(p2, "              " );
      value = dfssend(proc_anchor, &ptrn, &port_tab[2], 0);
      value = dfsdrop(proc_anchor, &ptr);
        
      value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
      }
    return(0);
}

void getWikiName (char * p, int sz) {
char * i_ptr;
char * es_ptr;
char * o_ptr;

o_ptr = WikiName; 
i_ptr = p;
es_ptr = p + sz;
WikiName[0] = '\0';

for (;;) {
  TCO(nb1,' ');
 }
nb1: if (0!=strcmp(i_ptr,"<a href=wiki?"))
  return;
i_ptr += strlen("<a href=wiki?");

for (;;) {
   TCO(kwn,'>');
   break;
kwn: CC;
}
*o_ptr = '\0';

eos:exit;
}

int isitint() {
int i;

WikiNameList[0] = "WikiWikiSandbox";
kno = 1;

for (i = 0; i < kno; i++) {
   if (0 == strcmp(WikiName, WikiNameList[i]))
     return 1;
     }

return 0;

}


