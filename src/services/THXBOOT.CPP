
#include <time.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include "thxiip.h"
#include "thxanch.h"
#include "thxscan.h"
#define FALSE 0
#define TRUE 1

   
   void FAR threads(label_ent * label_tab, int dynam,  
      FILE * fp,
      int timereq);

   main(int argc, char *argv[])
   
   {
    int value;
    int timereq;
    char fname[20];
    FILE *fp;
    label_ent * label_tab;

    timereq = FALSE;
    
    
    if (argc < 2) {
	    printf("You forgot to enter the file name\n");
            return(8);
            }  

    strcpy(fname,argv[1]);
    if ((fp = fopen(strcat(fname,".net"),"r")) == NULL) {
	    printf("Cannot open Network\n");
	    return(8);
	    }
    if (argc == 2 && strcmp(argv[1],"/t") == 0)
      timereq = TRUE;
     
    threads(label_tab, 1, fp, timereq);

    return(0);
    }




