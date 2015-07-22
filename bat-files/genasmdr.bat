@ECHO OFF
rem generate assembler listing

IF '%1' == '' GOTO CMPMISSR
path c:\borlandc\bin
BCC -W -3 -P -c -S -ml -v -f287 -ff- -N -Ic:\borlandc\include;c:\threadn;c:\threadn\thrdsrc %1.cpp
GOTO DONE
:CMPMISSR ECHO SUBROUTINE NAME MISSING
:DONE