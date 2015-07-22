#include "thxiip.h"
#include "thxanch.h"
#include "thxscan.h"
#include <stdio.h>
void FAR threads(label_ent * label_tab, int dynam,  FILE * fp, int timereq);
THRCOMP THFILERD(anchor anch);
THRCOMP SELURLS(anchor anch);
THRCOMP THFILEWT(anchor anch);
proc_ent P0 = {"Lire\0", "THFILERD\0", (int (FAR PASCAL*)(anchor anch))THFILERD, 0, 0,  0, 0, 0};
proc_ent P1 = {"Select\0", "SELURLS\0", (int (FAR PASCAL*)(anchor anch))SELURLS, 0, 0,  0, 0, 0};
proc_ent P2 = {"Ecrire\0", "THFILEWT\0", (int (FAR PASCAL*)(anchor anch))THFILEWT, 0, 0,  0, 0, 0};
proc_ent P3 = {"\0", "\0", 0, 0, 0, 0, 0, 0};
IIP I0 = {33, "c:\\Business\\UseMod~1\\recent~1.htm"};
IIP I3 = {32, "c:\\Business\\UseMod~1\\modchgs.htm"};
cnxt_ent C0 = {"!\0", "\0", 0, "Lire\0", "OPT\0", 0, &I0, 0};
cnxt_ent C1 = {"Lire\0", "OUT\0", 0, "Select\0", "IN\0", 0, 0, 6};
cnxt_ent C2 = {"Select\0", "OUT\0", 0, "Ecrire\0", "IN\0", 0, 0, 6};
cnxt_ent C3 = {"!\0", "\0", 0, "Ecrire\0", "OPT\0", 0, &I3, 0};
cnxt_ent C4 = {"\0", " ",0, " ", " ", 0, 0, 0};
label_ent LABELTAB = {" ", "\0", &C0, &P0, 'L', 0, 0};
label_ent L1 = {"\0", "\0", 0, 0, ' ', 0, 0};
label_ent LX = {"\0", "\0", 0, 0, ' ', 0, 0};
int comptab = 0;
void main() {
threads(&LABELTAB, 0, 0, 0);
}
