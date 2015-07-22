@ECHO OFF
ECHO Converting %1.NET file to Windows .EXE
rem Build application for Windows, using .NET file 
IF '%1' == '' GOTO NETMISSR
IF NOT EXIST %1.NET GOTO NETFILEMISSING
path c:\borlandc\bin
THXGEN %1
BCC -W -3 -P -c -ml -N -Ic:\borlandc\include;c:\threadn %1.cpp
TLINK /Tw /s /v /Lc:\borlandc\lib;c:\threadn C0wl %1 , %1.exe,, import cwl threadsw thcompsw ,threadsw.def
TDSTRIP -s %1
GOTO DONE

:NETMISSR
ECHO THE COMMAND REQUIRES A NETWORK FILE NAME (netname.net)
GOTO DONE

:NETFILEMISSING
ECHO THE REQUESTED FILE %1 IS NOT FOUND: CHECK SPELLING AND PATH

:DONE