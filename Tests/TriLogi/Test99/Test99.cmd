@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Tests/TriLogi/Test99/Test99.cmd

echo Executing  Tests/TriLogi/Test99/Test99.cmd  ...

rem ===== Configuration =====================================================

set CONFIG=Debug

set INPUT_PC6=PLC_C3.PC6

rem ===== Initialisation ====================================================

set BINARY_FOLDER=..\..\..\x64\%CONFIG%

set FIRST_CIRCUITS_TXT=First_Circuits.txt

set FIRST_MAIN_TXT=First_Main.txt

set FIRST_TO_COMPILE_PC6=First_ToCompile.PC6

set KDIFF3_EXE="C:\Program Files\KDiff3\kdiff3.exe"

set PLC_CIRCUITS_TXT=PLC_Circuits.txt

set PLC_MAIN_TXT=PLC_Main.txt

set PLC_TO_COMPILE_PC6=PLC_ToCompile.PC6

set SECOND_CIRCUITS_TXT=Second_Circuits.txt

set SECOND_MAIN_TXT=Second_Main.txt

set TRILOGI_BUILD_EXE="%BINARY_FOLDER%\TriLogi_Build.exe"

set TRILOGI_CONVERT_EXE="%BINARY_FOLDER%\TriLogi_Convert.exe"

rem ===== Verification ======================================================

if not exist %INPUT_PC6% (
    echo FATAL ERROR  %INPUT_PC6%  does not exist
    pause
    exit /B 10
)

if not exist %TRILOGI_BUILD_EXE% (
    echo FATAL ERROR  %TRILOGI_BUILD_EXE%  does not exist
    pause
    exit /B 20
)

if not exist %TRILOGI_CONVERT_EXE% (
    echo FATAL ERROR  %TRILOGI_CONVERT_EXE%  does not exist
    pause
    exit /B 30
)

rem ===== Execution =========================================================

%TRILOGI_CONVERT_EXE% %INPUT_PC6% Initial
if ERRORLEVEL 1 (
    echo ERROR  %TRILOGI_CONVERT_EXE% %INPUT_PC6% Initial  failed - %ERRORLEVEL%
    pause
    exit /B 40
)

copy %PLC_CIRCUITS_TXT% %FIRST_CIRCUITS_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %PLC_CIRCUITS_TXT% %FIRST_CIRCUITS_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

copy %PLC_MAIN_TXT% %FIRST_MAIN_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %PLC_MAIN_TXT% %FIRST_MAIN_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 60
)

%TRILOGI_BUILD_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %TRILOGI_BUILD_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 70
)

copy %PLC_TO_COMPILE_PC6% %FIRST_TO_COMPILE_PC6%
if ERRORLEVEL 1 (
    echo ERROR  copy %PLC_TO_COMPILE_PC6% %FIRST_TO_COMPILE_PC6%  failed - %ERRORLEVEL%
    pause
    exit /B 80
)

%TRILOGI_CONVERT_EXE% %PLC_TO_COMPILE_PC6% Initial
if ERRORLEVEL 1 (
    echo ERROR  %TRILOGI_CONVERT_EXE% %COMPILED_PC6% Initial  failed - %ERRORLEVEL%
    pause
    exit /B 90
)

copy %PLC_CIRCUITS_TXT% %SECOND_CIRCUITS_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %PLC_CIRCUITS_TXT% %SECOND_CIRCUITS_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 100
)

copy %PLC_MAIN_TXT% %SECOND_MAIN_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %PLC_MAIN_TXT% %SECOND_MAIN_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 110
)

%KDiff3_EXE% %FIRST_CIRCUITS_TXT% %SECOND_CIRCUITS_TXT%

%KDiff3_EXE% %FIRST_MAIN_TXT% %SECOND_MAIN_TXT%

rem ===== End ===============================================================

echo OK
