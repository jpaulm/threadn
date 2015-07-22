
    #include <stdio.h>
    #include <malloc.h>
    #include <stdio.h>
    #include <ctype.h>
    #include <stdlib.h>
    #include <string.h>
    #include <setjmp.h>
    #include "threads.h"
    #include "thxiip.h"
    #include "thxscan.h"
    #define TRUE 1
    #define FALSE 0
    
    #if __OS2__ == 1
      #define getc fgetc
      #undef  EOF
      #define EOF 0xff
    #endif 
    
    #define TC(label,ch)  if (curr_char != ch) goto label; \
                     *o_ptr = curr_char; o_ptr++;  \
                     curr_char = getc(fp);

    #define TCO(label,ch)  if (curr_char != ch) goto label;  \
                     curr_char = getc(fp);

    #define TA(label)    if (!isalpha(curr_char)) goto label; \
                     *o_ptr = curr_char; o_ptr++;  \
                     curr_char = getc(fp);

    #define TAO(label)   if (!isalpha(curr_char)) goto label; \
                     curr_char = getc(fp);

    #define TN(label)    if (!isdigit(curr_char)) goto label; \
                     *o_ptr = curr_char; o_ptr++;  \
                     curr_char = getc(fp);

    #define TNO(label)   if (!isdigit(curr_char)) goto label; \
                     curr_char = getc(fp);

    #define CC       *o_ptr = curr_char; o_ptr++;  \
                     curr_char = getc(fp);

    #define SC       curr_char = getc(fp);



   void scan_blanks(FILE *fp);
   int scan_sym(FILE *fp, char *o_ptr);
   int find_label(label_ent *label_tab, char name[32], char file[10],
      int label_count);
   int thxgatrs(char * comp);


   char curr_char;

   int thxscan(FILE *fp, cnxt_ent *cnxt_tab, proc_ent *proc_tab,
		       label_ent *label_tab, char file_name[10],
                        int *label_cp, int* proc_cp, int* cnxt_cp)
    {
       char *o_ptr;
       char out_str[255];
       char fname[256];
       char out_num[5];
       int i, IIPlen, ret_code;
       int proc_cnt;
       int proc_start = 0;
       int proc_hold, proc_last;
       int label_count, proc_count, cnxt_count;
       proc_ent *curr_proc;
       cnxt_ent *curr_cnxt;
       int label_ct;
       unsigned cnxt_open, eq_arrow;
       IIP *IIP_ptr;
       FILE *fp2;


       label_count = *label_cp;
       proc_count = *proc_cp;
       cnxt_count = *cnxt_cp;
       proc_hold = proc_count;
       ret_code = 0;
       do {curr_char = getc(fp);} while (curr_char == '\n');

 netloop:
       cnxt_open = FALSE;

       o_ptr = out_str;
       scan_blanks(fp);
       TCO(nEOF,EOF);  
       printf("End of Network Definition\n");
       goto exit;
 nEOF: TCO(NQ,'\'');
       strcpy(label_tab[label_count].label," ");
       strcpy(label_tab[label_count].file, file_name);
       label_tab[label_count].cnxt_ptr = &cnxt_tab[cnxt_count];
       label_tab[label_count].proc_ptr = &proc_tab[proc_count];
       label_tab[label_count].ent_type = 'L';
       printf("Scanning Main Network\n");
       proc_start = proc_count;
       proc_tab[proc_count].proc_name[0] = '\0';
       label_count++;
       label_tab[label_count].label[0] = '\0';
       goto TQ;
 NQ:   if (scan_sym(fp, o_ptr) != 0) {
	  printf("Name error\n");
	  ret_code = 4;
          goto exit;  }
       scan_blanks(fp);
       TCO(ncol,':');
       strcpy(label_tab[label_count].label,out_str);
       strcpy(label_tab[label_count].file, file_name);
       label_tab[label_count].cnxt_ptr = &cnxt_tab[cnxt_count];
       label_tab[label_count].proc_ptr = &proc_tab[proc_count];
       label_tab[label_count].ent_type = 'L';
       printf("Scanning Network: %s\n",out_str);
       proc_start = proc_count;
       label_count++;
       label_tab[label_count].label[0] = '\0';

       scan_blanks(fp);

loop:
       o_ptr = out_str;
       scan_blanks(fp);
       TCO(X1,'\'');
TQ:    
       TCO(tbsl,EOF);  
       printf("EOF encountered within quoted string\n");
       ret_code = 4;
       goto exit;
tbsl:  TCO(copy,'\'');
       TC(NQ2,'\'');
       goto TQ;
copy:  CC;
       goto TQ;
NQ2:   IIPlen = o_ptr - out_str;
       strcpy(cnxt_tab[cnxt_count].upstream_name,"!");
       IIP_ptr = (IIP *)malloc(IIPlen + sizeof(IIP_ptr -> IIP_len));
       IIP_ptr -> IIP_len = IIPlen;
       memcpy(IIP_ptr -> datapart, out_str, IIPlen);
       cnxt_tab[cnxt_count].gen.IIPptr = IIP_ptr;
       scan_blanks(fp);
       goto tArrow;

X1:    if (scan_sym(fp, o_ptr) != 0) {
	  printf("Name error\n");
	  ret_code = 4;
          goto exit;  }
       scan_blanks(fp);
       goto topen;

ncol:   strcpy(label_tab[label_count].label," ");
       strcpy(label_tab[label_count].file, file_name);
       label_tab[label_count].cnxt_ptr = &cnxt_tab[cnxt_count];
       label_tab[label_count].proc_ptr = &proc_tab[proc_count];
       label_tab[label_count].ent_type = 'L';
       proc_start = proc_count;
       label_count++;
       label_tab[label_count].label[0] = '\0';

topen:  if (cnxt_open) {
	  strcpy(cnxt_tab[cnxt_count].downstream_name,out_str);
	  cnxt_open = FALSE;
	  curr_cnxt = &cnxt_tab[cnxt_count];
	  if (curr_cnxt -> upstream_name[0] != '!')
             printf(" Connection: %s %s[%d] -> %s[%d] %s\n",
	     curr_cnxt -> upstream_name,
	     curr_cnxt -> upstream_port_name,
	     curr_cnxt -> upstream_elem_no,
	     curr_cnxt -> downstream_port_name,
	     curr_cnxt -> downstream_elem_no,
	     curr_cnxt -> downstream_name);
	   else {
             printf(" IIP: -> %s[%d] %s\n",
	     curr_cnxt -> downstream_port_name,
	     curr_cnxt -> downstream_elem_no,
	     curr_cnxt -> downstream_name);
             IIP_ptr = curr_cnxt -> gen.IIPptr;
             printf("    \'");
             for (i = 0; i < IIP_ptr -> IIP_len; i++)
	          printf("%c",IIP_ptr -> datapart[i]);
             printf("\'\n");
             }
	  cnxt_count++;
	  }
        else {
	TCO(NIP,'=');
	TCO(NIP,'>');
	scan_blanks(fp);

        strcpy(cnxt_tab[cnxt_count].upstream_name, "*");
	strcpy(cnxt_tab[cnxt_count].upstream_port_name,out_str);
        cnxt_tab[cnxt_count].upstream_elem_no = 0;
        goto ncap;
       }
	/* look up in proc list for subnet */
 NIP:  i = proc_start;
       while (proc_tab[i].proc_name[0] != '\0') {
         if (strcmp(proc_tab[i].proc_name,out_str) == 0) break;
	 i++;
         }
       proc_cnt = i;
       if (proc_tab[proc_cnt].proc_name[0] == '\0')  {
	 strcpy(proc_tab[proc_cnt].proc_name,out_str);
	 proc_tab[proc_cnt].trace = 0;
	 proc_tab[proc_cnt + 1].proc_name[0] = '\0';
	 proc_count = proc_cnt;
	 }

       TCO(NB1,'(');
       scan_blanks(fp);
       o_ptr = proc_tab[proc_cnt].comp_name;
Y1:    TA(NA2);
       goto Y1;
NA2:   TN(NN2);
       goto Y1;
NN2:   *o_ptr = '\0';
       scan_blanks(fp);
       TCO(NB1,')');
       curr_proc = &proc_tab[proc_cnt];
       printf(" Process: %s (%s)\n",curr_proc -> proc_name,
	  curr_proc -> comp_name);
NB1:   scan_blanks(fp);
       TCO(NQ1,'?');
       proc_tab[proc_cnt].trace = 1;

NQ1:   scan_blanks(fp);
       TCO(NSC1,';');
       cnxt_tab[cnxt_count].upstream_name[0] = '\0';
       cnxt_count++;
       proc_count++;
       proc_count++;
       label_count++;
       goto netloop;
NSC1:  TCO(NC1,',');
       goto loop;

NC1:   strcpy(cnxt_tab[cnxt_count].upstream_name,out_str);
       o_ptr = out_str;
X2:    TC(X2a,'*');     /* automatic port */
       *o_ptr = '\0';
       goto is_inport;
X2a:   if (scan_sym(fp, o_ptr) != 0) {
	  printf("Upstream port name error for %s\n",
	    cnxt_tab[cnxt_count].upstream_name);
	  ret_code = 4;
          goto exit;  }
is_inport:
       strcpy(cnxt_tab[cnxt_count].upstream_port_name,out_str);


       scan_blanks(fp);
       cnxt_tab[cnxt_count].upstream_elem_no = 0;
       TCO(tArrow,'[');
       o_ptr = out_num;
GNx:   TN(NNx);
       goto GNx;
NNx:   TCO(elemerr,']');
       *o_ptr = '\0';
       cnxt_tab[cnxt_count].upstream_elem_no = atoi(out_num);
       scan_blanks(fp);

tArrow: eq_arrow = FALSE;
       TCO(tEq,'-');
       goto tGr;
tEq:   TCO(nArrow,'=');
       eq_arrow = TRUE;
tGr:   TCO(nArrow,'>');

       cnxt_tab[cnxt_count].capacity = -1;
       scan_blanks(fp);
       TCO(ncap,'(');
       o_ptr = out_num;
GNz:   TN(NNz);
       goto GNz;
NNz:   TCO(caperr,')');
       *o_ptr = '\0';
       cnxt_tab[cnxt_count].capacity = atoi(out_num);
       scan_blanks(fp);
       goto ncap;
caperr: printf("Capacity error\n");
       ret_code = 4;
       goto exit;
ncap:  cnxt_tab[cnxt_count].downstream_elem_no = 0;

/* Scan off downstream port name */
       o_ptr = out_str;
       TC(Y2a,'*');       /* automatic port */
       *o_ptr = '\0';
       goto is_outport;
Y2a:   if (scan_sym(fp, o_ptr) != 0) {
	  printf("Downstream port name error for %s %s\n",
	  cnxt_tab[cnxt_count].upstream_name,
	  cnxt_tab[cnxt_count].upstream_port_name);
	  ret_code = 4;
          goto exit;  }
is_outport:
       strcpy(cnxt_tab[cnxt_count].downstream_port_name,out_str);
       strcpy(cnxt_tab[cnxt_count].downstream_name, "*");  /* ext. conn */
       cnxt_open = TRUE;

       scan_blanks(fp);
       TCO(CSC2,'[');
       o_ptr = out_num;
GNy:   TN(NNy);
       goto GNy;
NNy:   TCO(elemerr,']');
       *o_ptr = '\0';
       cnxt_tab[cnxt_count].downstream_elem_no = atoi(out_num);

CSC2:  scan_blanks(fp);
       TCO(NSC2,';');
       if (!eq_arrow)
	  printf("Port name %s not an external port\n", out_str);
       curr_cnxt = &cnxt_tab[cnxt_count];
       if (curr_cnxt -> upstream_name[0] != '!')
             printf(" Connection: %s %s[%d] -> %s[%d] %s\n",
	     curr_cnxt -> upstream_name,
	     curr_cnxt -> upstream_port_name,
	     curr_cnxt -> upstream_elem_no,
	     curr_cnxt -> downstream_port_name,
	     curr_cnxt -> downstream_elem_no,
	     curr_cnxt -> downstream_name);
	   else {
             printf(" IIP: -> %s[%d] %s\n",
	     curr_cnxt -> downstream_port_name,
	     curr_cnxt -> downstream_elem_no,
	     curr_cnxt -> downstream_name);
             IIP_ptr = curr_cnxt -> gen.IIPptr;
             printf("    \'");
             for (i = 0; i < IIP_ptr -> IIP_len; i++)
	          printf("%c",IIP_ptr -> datapart[i]);
             printf("\'\n");
             }


       cnxt_count++;
       cnxt_tab[cnxt_count].upstream_name[0] = '\0';
       cnxt_count++;
       proc_count++;
       proc_count++;
       label_count++;
       goto netloop;

NSC2:  TCO(loop,',');

       if (!eq_arrow)
	  printf("Port name %s not an external port\n", out_str);
       curr_cnxt = &cnxt_tab[cnxt_count];
       if (curr_cnxt -> upstream_name[0] != '!')
             printf(" Connection: %s %s[%d] -> %s[%d] %s\n",
	     curr_cnxt -> upstream_name,
	     curr_cnxt -> upstream_port_name,
	     curr_cnxt -> upstream_elem_no,
	     curr_cnxt -> downstream_port_name,
	     curr_cnxt -> downstream_elem_no,
	     curr_cnxt -> downstream_name);
	   else {
             printf(" IIP: -> %s[%d] %s\n",
	     curr_cnxt -> downstream_port_name,
	     curr_cnxt -> downstream_elem_no,
	     curr_cnxt -> downstream_name);
             IIP_ptr = curr_cnxt -> gen.IIPptr;
             printf("    \'");
             for (i = 0; i < IIP_ptr -> IIP_len; i++)
	          printf("%c",IIP_ptr -> datapart[i]);
             printf("\'\n");
             }


       cnxt_count++;
       cnxt_open = FALSE;
       goto loop;

elemerr: printf("Port element error\n");
       ret_code = 4;
       goto exit;

nArrow: printf("No arrow found\n");
       ret_code = 4;
exit:
   if (fclose(fp) != 0) {
         printf("Close error\n");
         if (ret_code == 0)
            ret_code = 2;
	 }
       if (ret_code > 0) {
          printf("Scan error\n");
          return(ret_code);
          }

         proc_last = proc_count;
         for (i = proc_hold; i < proc_last; i++) {
	  curr_proc = &proc_tab[i];
	  if (curr_proc -> proc_name[0] == '\0') continue;
         label_ct = find_label(label_tab, curr_proc -> comp_name, file_name, label_count);
          curr_proc -> composite = (label_ct > 0);
          if (curr_proc -> composite)
             curr_proc -> label_count = label_ct;
           else {
            strcpy(fname, curr_proc -> comp_name);
            if ((fp2 = fopen(strcat(fname, ".net"),"r"))
                   != NULL) {
                 label_ct = label_count;
                 strcpy(fname, curr_proc -> comp_name);
	         thxscan(fp2, cnxt_tab, proc_tab, label_tab, fname,
                    &label_count, &proc_count, &cnxt_count);
                 fclose(fp2);
                 curr_proc -> composite = TRUE;
                 curr_proc -> label_count = label_ct;
                 }
                else {
                   curr_proc -> must_run =
                        (thxgatrs(curr_proc -> comp_name) > 0);
                   curr_proc -> composite = FALSE;
		   curr_proc -> faddr = 0;
                 }
	    }
          }
       *label_cp = label_count;
       *proc_cp = proc_count;
       *cnxt_cp = cnxt_count;
       printf("Scan finished\n");
       return (ret_code);
    }

 void scan_blanks(FILE *fp)
  {
     for (;;) {
	TCO(not_blank,' ');
        continue;
 not_blank: TCO(nsl,'/');
        TCO(nsl,'*');
        for (;;) {
          TCO(tast,EOF);  
          printf("EOF encountered within comment\n");
          goto exit;
 tast:    TCO(nast,'*');
	  TCO(nsl2,'/');
	  break;
 nast:    SC;
 nsl2:    continue;
          }
 nsl:   TCO(exit,'\n');
        }
exit: return;
}

int scan_sym(FILE *fp, char *o_ptr)
  {

       int i;

X3:    TA(NA4);
       goto X4;
NA4:   TN(NN4);
       goto X4;
NN4:   TC(ES4,'_');
 /* loop back to here */  
X4:    TA(NA5);
       goto X4;
NA5:   TN(NN5);
       goto X4;
NN5:   TC(ES5,'_');
       goto X4;
ES4:   printf("Invalid name: ");
       for (i = 0; i < 7; i++) {
          printf("%c",curr_char);
          curr_char = getc(fp);
          } 
       printf("\n");       
       return(4);

ES5:   *o_ptr = '\0';
       return(0);
  }

   int find_label(label_ent *label_tab, char name[32], char file[10],  int label_count )
{
   int i;
   for (i = 0; i < label_count; i++) {

        if (label_tab[i].label[0] == '\0') 
             continue;  
        if (label_tab[i].ent_type != 'L') 
             continue;  
        if (strcmp(label_tab[i].label, name) == 0  &&
                  (strcmp(label_tab[i].file, "\0") == 0 ||
                 strcmp(label_tab[i].file, file) == 0))
           break;
       }
   if (i < label_count)
       return (i);
     else return(0);
}


