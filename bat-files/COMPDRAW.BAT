@ECHO OFF
rem compile subroutine for Windows
ECHO Compiling subroutine %1 for Windows
IF '%1' == '' GOTO CMPMISSR
path c:\borlandc\bin
BCC -W -2 -P -c -ml -v -Ic:\borlandc\include;c:\threadn;c:\threadn\thrdsrc thrdsrc\%1.cpp
GOTO DONE
:CMPMISSR ECHO SUBROUTINE NAME MISSING
:DONE
