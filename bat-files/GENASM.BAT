@ECHO OFF
rem generate assembler listing

IF 'drawflow' == '' GOTO CMPMISSR
path c:\borlandc\bin
BCC -W -3 -P -c -S -ml -v -N -Ic:\borlandc\include;c:\threadn;c:\threadn\thrdsrc drawflow.cpp
GOTO DONE
:CMPMISSR ECHO SUBROUTINE NAME MISSING
:DONE