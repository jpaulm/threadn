@ECHO OFF
rem Compiling an Application component for DOS
ECHO Compiling Component %1 for DOS 
IF '%1' == '' GOTO CMPMISSR
cd thrdsrc
IF NOT EXIST %1.cpp GOTO COMPMISSING
cd ..
path c:\borlandc\bin
BCC  -2 -P -c -ml -v -Ic:\borlandc\include;c:\threadn thrdsrc\%1.cpp
TLIB thcomps +- %1
TLIB thcompsw +- %1
erase %1.obj
GOTO DONE

:CMPMISSR
ECHO THE COMMAND REQUIRES A COMPONENT NAME (cmpname.cpp)
GOTO DONE

:COMPMISSING
ECHO THE REQUESTED FILE %1.cpp IS NOT FOUND: CHECK SPELLING AND PATH

:DONE