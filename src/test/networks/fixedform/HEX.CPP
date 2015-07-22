#include "thxiip.h"
#include "thxanch.h"
#include "thxscan.h"
#include <stdio.h>
void FAR threads(label_ent * label_tab, int dynam,  FILE * fp, int timereq);
THRCOMP THFILERT(anchor anch);
THRCOMP THVIEW(anchor anch);
proc_ent P0 = {"Read\0", "THFILERT\0", (int (FAR PASCAL*)(anchor anch))THFILERT, 0, 0,  1, 0, 0};
proc_ent P1 = {"Show\0", "THVIEW\0", (int (FAR PASCAL*)(anchor anch))THVIEW, 0, 0,  1, 0, 0};
proc_ent P2 = {"\0", "\0", 0, 0, 0, 0, 0, 0};
IIP I0 = {29, "c:\\threadn\\thrdsrc\\thview.cpp"};
IIP I2 = {1, "H"};
cnxt_ent C0 = {"!\0", "\0", 0, "Read\0", "OPT\0", 0, &I0, 0};
cnxt_ent C1 = {"Read\0", "OUT\0", 0, "Show\0", "IN\0", 0, 0, 6};
cnxt_ent C2 = {"!\0", "\0", 0, "Show\0", "OPT\0", 0, &I2, 0};
cnxt_ent C3 = {"\0", " ",0, " ", " ", 0, 0, 0};
label_ent LABELTAB = {" ", "\0", &C0, &P0, 'L', 0, 0};
label_ent L1 = {"\0", "\0", 0, 0, ' ', 0, 0};
label_ent LX = {"\0", "\0", 0, 0, ' ', 0, 0};
int comptab = 0;
void main() {
threads(&LABELTAB, 0, 0, 0);
}
