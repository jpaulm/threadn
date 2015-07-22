@ECHO OFF
rem Creating THREADS ZIP file
rem First parameter is path of pkzip.exe
ECHO Building ZIP File 
erase threads.zip
erase *.bak
erase *.map
erase *.lst
copy thrdsrc\thzsstk.asm thrdsrc\thzsstk.as~
erase *.asm
copy thrdsrc\thzsstk.as~ thrdsrc\thzsstk.asm
erase *.as~
erase libtmp*.*
%1\pkzip threads *.*
erase thrdsrc\*.bak
%1\pkzip -a threads thrdsrc\*.*