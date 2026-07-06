@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Templates/TRiLOGI_Build.cmd

echo Excuting  TRiLOGI_Build.cmd  ...

rem ===== Configuration =====================================================

set KMS_PLC_FOLDER=C:\Program Files\KMS-PLC

rem ===== Initialisation ====================================================

set PLC_CIRCUITS_TXT=PLC_Circuits.txt

set PLC_MAIN_TXT=PLC_Main.txt

set PLC_TO_COMPILE_PC6=PLC_ToCompile.PC6

set TRILOGI_BUILD_EXE="%KMS_PLC_FOLDER%\TRiLOGI_Build.exe"

set TRILOGI_START_AND_CONVERT_CMD="%KMS_PLC_FOLDER%\Scripts\TRiLOGI_StartAndConvert.cmd"

rem ===== Verification ======================================================

if not exist %TRILOGI_BUILD_EXE% (
    echo FATAL ERROR  %TRILOGI_BUILD_EXE%  does not exist
    pause
    exit /B 20
)

if not exist %TRILOGI_START_AND_CONVERT_CMD% (
    echo FATAL ERROR  %TRILOGI_START_AND_CONVERT_CMD%  does not exist
    pause
    exit /B 30
)

if not exist %PLC_CIRCUITS_TXT% (
    echo FATAL ERROR  %PLC_CIRCUITS_TXT%  does not exist
    pause
    exit /B 40
)

if not exist %PLC_MAIN_TXT% (
    echo FATAL ERROR  %PLC_MAIN_TXT%  does not exist
    pause
    exit /B 50
)

rem ===== Excecution ========================================================

rem Add additional source file names to the next command line.
rem PLC_Circuits.txt and PLC_Main.txt are implicite.
%TRILOGI_BUILD_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %TRILOGI_BUILD_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 60
)

call %TRILOGI_START_AND_CONVERT_CMD%
if ERRORLEVEL 1 (
    echo ERROR  call %TRILOGI_START_AND_CONVERT_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 70
)

rem ===== End ===============================================================

echo OK
