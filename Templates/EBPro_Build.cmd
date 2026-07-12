@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Templates/EBPro_Build.cmd

echo Excuting  TRiLOGI_Build.cmd $1  ...

rem ===== Configuration =====================================================

set KMS_PLC_FOLDER=C:\Program Files\KMS-PLC

rem ===== Initialisation ====================================================

set EBPRO_BUILD_ADDRESS_EXE="%KMS_PLC_FOLDER%\EBPro_Build_Address.exe"

set EBPRO_BUILD_LABEL_EXE="%KMS_PLC_FOLDER%\EBPro_Build_Label.exe"

rem ===== Verification ======================================================

if not exist %EBPRO_BUILD_ADDRESS_EXE% (
    echo FATAL ERROR  %EBPRO_BUILD_ADDRESS_EXE%  does not exist
    pause
    exit /B 10
)

if not exist %EBPRO_BUILD_LABEL_EXE% (
    echo FATAL ERROR  %EBPRO_BUILD_LABEL_EXE%  does not exist
    pause
    exit /B 20
)

rem ===== Excecution ========================================================

if "Address" == "$1" then goto Addresses

if "Label" == "$1" then goto Labels

echo USER ERROR  Invalid command line
echo Usage: EBPro_Build.cmd Address|Label
pause
exit /B 30

:Addresses

rem Add additional source file name to the next command line.
rem HMI_Addresses.txt is implicite.
%EBPRO_BUILD_ADDRESS_EXE%

if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_BUILD_ADDRESS_EXE%  failed - %ERRORLEVEL%
    pause 40
    exit /B 
)

echo INSTRUCTION  Import EBPro_Addresses_ToImport.csv

pause

goto End

:Labels

rem Add additional source file name to the next line.
rem HMI_Labels.txt is implicite.
%EBPRO_BUILD_LABEL_EXE%

if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_BUILD_LABEL_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

echo INSTRUCTION  Import EBPro_Labels_ToImport.lbl

pause

goto End

rem ===== End ===============================================================

:End

echo OK
