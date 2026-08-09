@echo off

rem Author    KMS - Martin Dubois, P. Eng
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Scripts/TRiLOGI_StartAndConvert.cmd

echo Executing  TRiLOGI_StartAndConvert.cmd  ...

rem ===== Configuration =====================================================

set KMS_PLC_FOLDER=%~dp0\..

rem ===== Initialisation ====================================================

set PLC_TO_COMPILE_PC6=PLC_ToCompile.PC6

set TRILOGI_CONVERT_EXE="%KMS_PLC_FOLDER%\TRiLOGI_Convert.exe"

rem ===== Verification ======================================================

if not exist %TRILOGI_CONVERT_EXE% (
    echo FATAL ERROR  %TRILOGI_CONVERT_EXE%  does not exist
    pause
    exit /B 10
)

rem ===== Excecution ========================================================

if not exist %PLC_TO_COMPILE_PC6% (
    echo ERROR  %PLC_TO_COMPILE_PC6%  does not exist
    pause
    exit /B 20
)

start "" /max /wait %PLC_TO_COMPILE_PC6%
if ERRORLEVEL 1 (
    echo ERROR  start "" /max /wait %PLC_TO_COMPILE_PC6%  failed - %ERRORLEVEL%
    pause
    exit /B 30
)

%TRILOGI_CONVERT_EXE% %PLC_TO_COMPILE_PC6%
if ERRORLEVEL 1 (
    echo ERROR  %TRILOGI_CONVERT_EXE% %PLC_TO_COMPILE_PC6%  failed - %ERRORLEVEL%
    pause
    exit /B 70
)

del %PLC_TO_COMPILE_PC6%
if ERRORLEVEL 1 (
    echo ERROR  del %PLC_TO_COMPILE_PC6%  failed - %ERRORLEVEL%
    pause
    exit /B 80
)

rem ===== End ===============================================================

echo OK
