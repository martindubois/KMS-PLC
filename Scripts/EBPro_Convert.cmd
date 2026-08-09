@echo off

rem Author    KMS - Martin Dubois, P. Eng
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Scripts/EBPro_Convert.cmd

echo Executing  EBPro_Convert.cmd  ...

rem ===== Initialisation ====================================================

set KMS_PLC_FOLDER=%~dp0\..

set EBPRO_CONFIG_TXT=EBPro_Config.txt

set EBPRO_CONVERT_ADDRESS_EXE="%KMS_PLC_FOLDER%\EBPro_Convert_Address.exe"

set EBPRO_CONVERT_LABEL_EXE="%KMS_PLC_FOLDER%\EBPro_Convert_Label.exe"

set EBPRO_EXPORTED_LABEL_LBL=EBPro_Exported_Label.lbl

rem ===== Verification ======================================================

if not exist %EBPRO_CONFIG_TXT% (
    echo FATAL ERROR  %EBPRO_CONFIG_TXT%  does not exist
    pause
    exit /B 10
)

if not exist %EBPRO_CONVERT_ADDRESS_EXE% (
    echo FATAL ERROR  %EBPRO_CONVERT_ADDRESS_EXE%  does not exist
    pause
    exit /B 30
)

if not exist %EBPRO_CONVERT_LABEL_EXE% (
    echo FATAL ERROR  %EBPRO_CONVERT_LABEL_EXE%  does not exist
    pause
    exit /B 30
)

rem ===== Execution =========================================================

echo INSTRUCTION  Export addresses to EBPro_Exported_Address.csv

pause

echo INSTRUCTION  Export labels to EBPro_Exported_Label.lbl

pause

%EBPRO_CONVERT_ADDRESS_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_CONVERT_ADDRESS_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 40
)

%EBPRO_CONVERT_LABEL_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_CONVERT_LABEL_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

rem ===== End ===============================================================

echo OK
