@ECHO OFF
rem compile THREADS subroutine for DOS
ECHO Compiling THREADS Subroutine %1 for DOS
IF '%1' == '' GOTO CMPMISSR
path c:\borlandc\bin
BCC  -2 -P -c -ml -v -Ic:\borlandc\include;c:\threadn;c:\threadn\thrdsrc thrdsrc\%1.cpp
IF '%1' == 'THXGEN' GOTO DONE
IF '%1' == 'THXBOOT' GOTO DONE
IF '%1' == 'thxgen' GOTO DONE
IF '%1' == 'thxboot' GOTO DONE
TLIB threads+-%1
erase %1.obj
GOTO DONE

:CMPMISSR ECHO SUBROUTINE NAME MISSING
:DONE