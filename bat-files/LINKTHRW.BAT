@ECHO OFF
rem build THREADS Driver for Windows, called THREADSW.EXE
ECHO Build THREADS Driver for Windows
rem
rem - Issue command     c:\threadn\threadsw revs
rem  -and the net will be executed dynamically, 
rem - loading components at run time
rem
path c:\borlandc\bin
BCC -W -3 -P -c -ml -v -N -Ic:\borlandc\include;c:\threadn;c:\threadn\thrdsrc thrdsrc\threads.cpp
TLINK /Tw /s /v /Lc:\borlandc\lib;c:\threadn C0wl thxboot, threadsw.exe,, import cwl threadsw  ,threadsw.def
TDSTRIP -s threadsw
