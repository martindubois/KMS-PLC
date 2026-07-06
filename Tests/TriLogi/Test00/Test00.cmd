@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Tests/TRiLOGI/Test00/Test00.cmd

echo Executing  Tests/TRiLOGI/Test99/Test00.cmd  ...

rem ===== Configuration =====================================================

set CONFIG=Debug

rem ===== Initialisation ====================================================

set BINARY_FOLDER=..\..\..\x64\%CONFIG%

set TRILOGI_BUILD_EXE="%BINARY_FOLDER%\TRiLOGI_Build.exe"

set TRILOGI_CONVERT_EXE="%BINARY_FOLDER%\TRiLOGI_Convert.exe"

rem ===== Verification ======================================================

if not exist %TRILOGI_BUILD_EXE% (
    echo FATAL ERROR  %TRILOGI_BUILD_EXE%  does not exist
    pause
    exit /B 10
)

if not exist %TRILOGI_CONVERT_EXE% (
    echo FATAL ERROR  %TRILOGI_CONVERT_EXE%  does not exist
    pause
    exit /B 20
)

rem ===== Execution =========================================================

%TRILOGI_BUILD_EXE%
if 0 == %ERRORLEVEL% (
    echo ERROR  %TRILOGI_BUILD_EXE%  did not fail
    pause
    exit /B 30
)

%TRILOGI_CONVERT_EXE%
if 0 == %ERRORLEVEL% (
    echo ERROR  %TRILOGI_CONVERT_EXE%  did not fail
    pause
    exit /B 40
)

%TRILOGI_CONVERT_EXE% DoesNotExist.PC6 Initial OneMore
if 0 == %ERRORLEVEL% (
    echo ERROR  %TRILOGI_CONVERT_EXE% DoesNotExist.PC6 Initial OneMore  did not fail
    pause
    exit /B 50
)

%TRILOGI_CONVERT_EXE% DoesNotExist.PC6 Invalid
if 0 == %ERRORLEVEL% (
    echo ERROR  %TRILOGI_CONVERT_EXE% DoesNotExist.PC6 Invalid  did not fail
    pause
    exit /B 60
)

%TRILOGI_CONVERT_EXE% DoesNotExist.PC6
if 0 == %ERRORLEVEL% (
    echo ERROR  %TRILOGI_CONVERT_EXE% DoesNotExist.PC6  did not fail
    pause
    exit /B 70
)

%TRILOGI_CONVERT_EXE% DoesNotExist.PC6 Initial
if 0 == %ERRORLEVEL% (
    echo ERROR  %TRILOGI_CONVERT_EXE% DoesNotExist.PC6 Initial  did not fail
    pause
    exit /B 80
)

rem ===== End ===============================================================

echo OK
