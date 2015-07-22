@ECHO OFF
rem compile THREADS subroutine for Windows
ECHO Compiling THREADS subroutine %1 for Windows
IF '%1' == '' GOTO CMPMISSR
path c:\borlandc\bin
BCC -W -3 -P -c -ml -v -N -Ic:\borlandc\include;c:\threadn;c:\threadn\thrdsrc thrdsrc\%1.cpp
IF '%1' == 'THXGEN' GOTO DONE
IF '%1' == 'THXBOOT' GOTO DONE
IF '%1' == 'thxgen' GOTO DONE
IF '%1' == 'thxboot' GOTO DONE
TLIB threadsw+-%1
IF '%1' == 'libmain' GOTO DONE
IF '%1' == 'LIBMAIN' GOTO DONE
erase %1.obj
GOTO DONE
:CMPMISSR ECHO SUBROUTINE NAME MISSING
:DONE
pause