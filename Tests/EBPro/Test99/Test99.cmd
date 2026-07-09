@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Tests/EBPro/Test99/Test99.cmd

echo Executing  Tests/EBPro/Test99/Test99.cmd  ...

rem ===== Configuration =====================================================

set CONFIG=Debug

rem ===== Initialisation ====================================================

set BINARY_FOLDER=..\..\..\x64\%CONFIG%

set EBPRO_ADDRESSES_TO_IMPORT_CSV="EBPro_Addresses_ToImport.csv"

set EBPRO_BUILD_ADDRESS_EXE="%BINARY_FOLDER%\EBPro_Build_Address.exe"

set EBPRO_CONVERT_ADDRESS_EXE="%BINARY_FOLDER%\EBPro_Convert_Address.exe"

set FIRST_ADDRESSES_TO_IMPORT_CSV="First_Addresses_ToImport.csv"

set FIRST_ADDRESSES_TXT="First_Addresses.txt"

set HMI_ADDRESSES_TXT="HMI_Addresses.txt"

set KDIFF3_EXE="C:\Program Files\KDiff3\kdiff3.exe"

set SECOND_ADDRESSES_TXT="Second_Addresses.txt"

rem ===== Verification ======================================================

if not exist %EBPRO_BUILD_ADDRESS_EXE% (
    echo FATAL ERROR  %EBPRO_BUILD_ADDRESS_EXE%  does not exist
    pause
    exit /B 10
)

if not exist %EBPRO_CONVERT_ADDRESS_EXE% (
    echo FATAL ERROR  %EBPRO_CONVERT_ADDRESS_EXE%  does not exist
    pause
    exit /B 20
)

if not exist %KDIFF3_EXE% (
    echo FATAL ERROR  %KDIFF3_EXE%  does not exist
    echo Install KDiff3
    pause
    exit /B 30
)

rem ===== Execution =========================================================

%EBPRO_CONVERT_ADDRESS_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_CONVERT_ADDRESS_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 40
)

copy %HMI_ADDRESSES_TXT% %FIRST_ADDRESSES_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %HMI_ADDRESSES_TXT% %FIRST_ADDRESSES_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

%EBPRO_BUILD_ADDRESS_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_BUILD_ADDRESS_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 60
)

copy %EBPRO_ADDRESSES_TO_IMPORT_CSV% %FIRST_ADDRESSES_TO_IMPORT_CSV%
if ERRORLEVEL 1 (
    echo ERROR  copy %EBPRO_ADDRESSES_TO_IMPORT_CSV% %FIRST_ADDRESSES_TO_IMPORT_CSV%  failed - %ERRORLEVEL%
    pause
    exit /B 70
)

%EBPRO_CONVERT_ADDRESS_EXE% %EBPRO_ADDRESSES_TO_IMPORT_CSV%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_CONVERT_ADDRESS_EXE% %EBPRO_ADDRESSES_TO_IMPORT_CSV%  failed - %ERRORLEVEL%
    pause
    exit /B 80
)

copy %HMI_ADDRESSES_TXT% %SECOND_ADDRESSES_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %HMI_ADDRESSES_TXT% %SECOND_ADDRESSES_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 90
)

echo INSTRUCTION  File must be binary equal

%KDiff3_EXE% %FIRST_ADDRESSES_TXT% %SECOND_ADDRESSES_TXT%

rem ===== End ===============================================================

echo OK
