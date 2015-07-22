; Assembler routine called by OS/2 version of THREADS.CPP
; It is passed a pointer which will become the new stack pointer
; It assumes the OS/2 _System calling interface 

        .386
        PUBLIC thzsstk

CODE32  SEGMENT
        ASSUME cs:CODE32,ds:FLAT 
thzsstk PROC,
            myptr:ptr dword 

 ;  assume DS = SS
 
        mov  eax,[esp+4]   ; get address of new stack area
        mov  ecx, [esp]    ; move return address into reg 
        sub  eax,8         ; reduce stack area addr by 8 
        mov  [eax],ecx     ; move return addr into new stack
        mov  esp,eax       ; make new area the official stack 
        xor  eax,eax       ; indicate routine successful
        ret                ; return to calling routine
                           ;  using new stack 
thzsstk ENDP
CODE32  ENDS
        END
