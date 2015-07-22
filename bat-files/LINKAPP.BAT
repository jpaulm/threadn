@ECHO OFF
ECHO Converting %1.NET file to DOS .EXE
rem Build application for DOS, using .NET file 
IF '%1' == '' GOTO NETMISSR
IF NOT EXIST %1.NET GOTO NETFILEMISSING
path c:\borlandc\bin
THXGEN %1
BCC  -2 -P -c -ml -Ic:\borlandc\include;c:\threadn %1.cpp
TLINK /Td /s /v /Lc:\borlandc\lib;c:\threadn C0l %1 , %1.exe,,  cl threads thcomps emu
TDSTRIP -s %1
GOTO DONE

:NETMISSR
ECHO THE COMMAND REQUIRES A NETWORK FILE NAME (netname.net)
GOTO DONE

:NETFILEMISSING
ECHO THE REQUESTED FILE %1 IS NOT FOUND: CHECK SPELLING AND PATH

:DONE