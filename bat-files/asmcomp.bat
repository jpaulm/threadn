@ECHO OFF
rem assemble code
ECHO Assembling %1
IF '%1' == '' GOTO CMPMISSR
path c:\borlandc\bin
tasm /la c:\threadn\%1.asm, c:\threadn\%1.obj, c:\threadn\%1.lst
TLIB threadsw+-%1
erase %1.obj
GOTO DONE
:CMPMISSR ECHO SUBROUTINE NAME MISSING
:DONE
