#include "thxiip.h"
#include "thxanch.h"
#include "thxscan.h"
#include <stdio.h>
void FAR threads(label_ent * label_tab, int dynam,  FILE * fp, int timereq);
THRCOMP THFILERD(anchor anch);
THRCOMP SELPOMP(anchor anch);
THRCOMP THFILEWT(anchor anch);
proc_ent P0 = {"Lire\0", "THFILERD\0", (int (FAR PASCAL*)(anchor anch))THFILERD, 0, 0,  0, 0, 0};
proc_ent P1 = {"Select\0", "SELPOMP\0", (int (FAR PASCAL*)(anchor anch))SELPOMP, 0, 0,  0, 0, 0};
proc_ent P2 = {"Ecrire\0", "THFILEWT\0", (int (FAR PASCAL*)(anchor anch))THFILEWT, 0, 0,  0, 0, 0};
proc_ent P3 = {"\0", "\0", 0, 0, 0, 0, 0, 0};
IIP I0 = {23, "c:\\threadn\\pompiers.fil"};
IIP I3 = {8, "100000  "};
IIP I4 = {21, "c:\\threadn\\riches.fil"};
cnxt_ent C0 = {"!\0", "\0", 0, "Lire\0", "OPT\0", 0, &I0, 0};
cnxt_ent C1 = {"Lire\0", "OUT\0", 0, "Select\0", "IN\0", 0, 0, 6};
cnxt_ent C2 = {"Select\0", "OUT\0", 0, "Ecrire\0", "IN\0", 0, 0, 6};
cnxt_ent C3 = {"!\0", "\0", 0, "Select\0", "SAL\0", 0, &I3, 0};
cnxt_ent C4 = {"!\0", "\0", 0, "Ecrire\0", "OPT\0", 0, &I4, 0};
cnxt_ent C5 = {"\0", " ",0, " ", " ", 0, 0, 0};
label_ent LABELTAB = {" ", "\0", &C0, &P0, 'L', 0, 0};
label_ent L1 = {"\0", "\0", 0, 0, ' ', 0, 0};
label_ent LX = {"\0", "\0", 0, 0, ' ', 0, 0};
int comptab = 0;
void main() {
threads(&LABELTAB, 0, 0, 0);
}
