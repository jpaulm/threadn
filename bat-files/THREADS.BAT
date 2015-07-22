@ECHO OFF
IF '%1' == ''     GOTO DISHELP
IF '%1' == 'RUN'  GOTO RUNPARM
IF '%1' == 'run'  GOTO RUNPARM
IF '%1' == 'Run'  GOTO RUNPARM
IF '%1' == 'rUN'  GOTO RUNPARM
IF '%1' == 'R'    GOTO RUNPARM
IF '%1' == 'r'    GOTO RUNPARM
IF '%1' == 'PROD' GOTO PRODPARM
IF '%1' == 'prod' GOTO PRODPARM
IF '%1' == 'Prod' GOTO PRODPARM
IF '%1' == 'pROD' GOTO PRODPARM
IF '%1' == 'P'    GOTO PRODPARM
IF '%1' == 'p'    GOTO PRODPARM
IF '%1' == 'HELP' GOTO HELPPARM
IF '%1' == 'help' GOTO HELPPARM
IF '%1' == 'Help' GOTO HELPPARM
IF '%1' == 'hELP' GOTO HELPPARM
IF '%1' == 'H'    GOTO HELPPARM
IF '%1' == 'h'    GOTO HELPPARM
ECHO FIRST PARAMETER IS INVALID, MUST BE RUN/PROD/HELP
GOTO DISHELP

:RUNPARM
IF '%2' == '' GOTO NETMISSR
IF NOT EXIST %2.NET GOTO NETFILEMISSING
ECHO THIS OPTION NOT CURRENTLY SUPPORTED
GOTO DONE

:NETMISSR
ECHO THE %1 COMMAND REQUIRES AN NETWORK FILE NAME (netname.net)
GOTO DONE

:NETFILEMISSING
ECHO THE REQUESTED FILE %2 IS NOT FOUND, CHECK SPELLING AND PATH
GOTO DONE

:PRODPARM
IF '%2' == '' GOTO NETMISSR
IF NOT EXIST %2.NET GOTO NETFILEMISSING
path c:\borlandc\bin
THXGEN %2
BCC -W -2 -P -c -ml -Ic:\borlandc\include;c:\threadw %2.cpp
TLINK /Tw /s /Lc:\borlandc\lib;c:\threadw C0wl %2 @threads.lnk @%2.lnk Cwl, %2.exe,, import ,threads.def
GOTO DONE

:HELPPARM
IF '%2 == ''      GOTO DISHELP
IF '%2' == 'RUN'  GOTO RUNHELP
IF '%2' == 'run'  GOTO RUNHELP
IF '%2' == 'Run'  GOTO RUNHELP
IF '%2' == 'rUN'  GOTO RUNHELP
IF '%2' == 'R'    GOTO RUNHELP
IF '%2' == 'r'    GOTO RUNHELP
IF '%2' == 'PROD' GOTO PRODHELP
IF '%2' == 'prod' GOTO PRODHELP
IF '%2' == 'Prod' GOTO PRODHELP
IF '%2' == 'pROD' GOTO PRODHELP
IF '%2' == 'P'    GOTO PRODHELP
IF '%2' == 'p'    GOTO PRODHELP
GOTO DISHELP

:RUNHELP
ECHO *** This option is currently unsupported. ***
ECHO The RUN subcommand is used to interpret a network file and run it.  The
ECHO network file must be in a file with an extension NET (ie. netname.NET)
ECHO Using the RUN option, changes may be made to the network, which can
ECHO then immediately be retried.  
GOTO DONE

:PRODHELP
ECHO The PROD subcommand produces an executable (netname.EXE) file from the
ECHO network file (netname.NET).  This allows a network to be run without
ECHO the THREADS RUN subcommand.  The network can now be run from the DOS
ECHO command prompt. 
ECHO This function requires the Borland Turbo C++ compiler, V 3.0 or later. 
GOTO DONE

:DISHELP
ECHO .
ECHO WELCOME TO THREADS, HOMEDATA's Flow-Based Programming tool!
ECHO .
ECHO The calling format is:
ECHO   THREADS RUN  netname         - to run a TEST network
ECHO   THREADS PROD netname         - to produce an EXEcutable for a network
ECHO   THREADS HELP [RUN/PROD]      - to get help on a subcommand
ECHO .
ECHO RUNning a test network is currently unsupported. 
ECHO .
ECHO When testing is complete for a network, you can produce an EXE file for
ECHO that network using the PROD command.  This produces an EXE file for your 
ECHO application, which can then simply be run just like other EXEs.
ECHO This function requires the Borland Turbo C++ compiler, V 3.0 or later.
:DONE