@ECHO OFF
rem Compiling an Application component as a Windows DLL
ECHO Compiling %1 as a Windows DLL 
IF '%1' == '' GOTO CMPMISSR
cd thrdsrc
IF NOT EXIST %1.cpp GOTO COMPMISSING
cd ..
path c:\borlandc\bin
BCC -WDE -3 -P -c -ml -v -N -Ic:\borlandc\include;c:\threadn thrdsrc\%1.cpp
TLIB thcompsw +- %1
if exist %1.dll erase %1.dll
TLINK /Twd /s /v /Lc:\borlandc\lib;c:\threadn C0dl %1 LibMain, %1 ,,import  mathwl cwl threadsw
erase %1.obj
GOTO DONE

:CMPMISSR
ECHO THE COMMAND REQUIRES A COMPONENT NAME (cmpname.cpp)
GOTO DONE

:COMPMISSING
ECHO THE REQUESTED FILE %1.cpp IS NOT FOUND: CHECK SPELLING AND PATH

:DONE
pause