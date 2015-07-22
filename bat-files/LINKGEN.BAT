@ECHO OFF
rem Link thxgen.exe
rem this program scans off a .net definition and converts it to a .cpp program
ECHO Linking the THXGEN Program as a DOS .EXE
path c:\borlandc\bin
tlink /Td /Lc:\borlandc\lib;c:\threadn C0l thxgen ,thxgen.exe,,cl threads emu