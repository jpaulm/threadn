#include "thxiip.h"
#include "thxanch.h"
#include "thxscan.h"
#include <stdio.h>
void FAR threads(label_ent * label_tab, int dynam,  FILE * fp, int timereq);
THRCOMP THGENIPS(anchor anch);
THRCOMP THENCBRK(anchor anch);
THRCOMP THREVERS(anchor anch);
THRCOMP THREMBRK(anchor anch);
THRCOMP THVIEW(anchor anch);
proc_ent P0 = {"generator\0", "THGENIPS\0", (int (FAR PASCAL*)(anchor anch))THGENIPS, 0, 0,  0, 0, 0};
proc_ent P1 = {"Enclose_in_brackets\0", "THENCBRK\0", (int (FAR PASCAL*)(anchor anch))THENCBRK, 0, 0,  0, 0, 0};
proc_ent P2 = {"Reverse_within_brackets\0", "THREVERS\0", (int (FAR PASCAL*)(anchor anch))THREVERS, 0, 0,  0, 0, 0};
proc_ent P3 = {"Remove_brackets\0", "THREMBRK\0", (int (FAR PASCAL*)(anchor anch))THREMBRK, 0, 0,  0, 0, 0};
proc_ent P4 = {"viewer\0", "L1\0",  0, 0, 2,   0, 1, 0};
proc_ent P5 = {"\0", "\0", 0, 0, 0, 0, 0, 0};
IIP I4 = {1, "H"};
IIP I5 = {2, "20"};
IIP I6 = {10, "Newtest..."};
cnxt_ent C0 = {"generator\0", "OUT\0", 0, "Enclose_in_brackets\0", "IN\0", 0, 0, 6};
cnxt_ent C1 = {"Enclose_in_brackets\0", "OUT\0", 0, "Reverse_within_brackets\0", "IN\0", 0, 0, 6};
cnxt_ent C2 = {"Reverse_within_brackets\0", "OUT\0", 0, "Remove_brackets\0", "IN\0", 0, 0, 6};
cnxt_ent C3 = {"Remove_brackets\0", "OUT\0", 0, "viewer\0", "IN\0", 0, 0, 6};
cnxt_ent C4 = {"!\0", "\0", 0, "viewer\0", "OPT\0", 0, &I4, 0};
cnxt_ent C5 = {"!\0", "\0", 0, "generator\0", "OPT\0", 0, &I5, 0};
cnxt_ent C6 = {"!\0", "\0", 0, "generator\0", "PREFIX\0", 0, &I6, 0};
cnxt_ent C7 = {"\0", " ",0, " ", " ", 0, 0, 0};
proc_ent P6 = {"viewer\0", "THVIEW\0", (int (FAR PASCAL*)(anchor anch))THVIEW, 0, 0,  0, 0, 0};
proc_ent P7 = {"\0", "\0", 0, 0, 0, 0, 0, 0};
cnxt_ent C8 = {"*\0", "OPT\0", 0, "viewer\0", "OPT\0", 0, 0, 0};
cnxt_ent C9 = {"*\0", "IN\0", 0, "viewer\0", "IN\0", 0, 0, 0};
cnxt_ent C10 = {"viewer\0", "OUT\0", 0, "*\0", "OUT\0", 0, 0, 6};
cnxt_ent C11 = {"\0", " ",0, " ", " ", 0, 0, 0};
label_ent LABELTAB = {" ", "\0", &C0, &P0, 'L', 0, 0};
label_ent L1 = {"\0", "\0", 0, 0, ' ', 0, 0};
label_ent L2 = {"L1\0", "\0", &C8, &P6, 'L', 0, 0};
label_ent L3 = {"\0", "\0", 0, 0, ' ', 0, 0};
label_ent LX = {"\0", "\0", 0, 0, ' ', 0, 0};
int comptab = 0;
void main() {
threads(&LABELTAB, 0, 0, 0);
}
