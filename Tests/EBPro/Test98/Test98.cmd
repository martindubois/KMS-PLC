@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2026 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Tests/EBPro/Test99/Test98.cmd

echo Executing  Tests/EBPro/Test98/Test98.cmd  ...

rem ===== Configuration =====================================================

set CONFIG=Debug

rem ===== Initialisation ====================================================

set BINARY_FOLDER=..\..\..\x64\%CONFIG%

set EBPRO_BUILD_LABEL_EXE="%BINARY_FOLDER%\EBPro_Build_Label.exe"

set EBPRO_CONVERT_LABEL_EXE="%BINARY_FOLDER%\EBPro_Convert_Label.exe"

set EBPRO_LABELS_TO_IMPORT_LBL="EBPro_Labels_ToImport.lbl"

set FIRST_LABELS_TO_IMPORT_LBL="First_Labels_ToImport.lbl"

set FIRST_LABELS_TXT="First_Labels.txt"

set HMI_LABELS_TXT="HMI_Labels.txt"

set KDIFF3_EXE="C:\Program Files\KDiff3\kdiff3.exe"

set SECOND_LABELS_TXT="Second_Labels.txt"

rem ===== Verification ======================================================

if not exist %EBPRO_BUILD_LABEL_EXE% (
    echo FATAL ERROR  %EBPRO_BUILD_LABEL_EXE%  does not exist
    pause
    exit /B 10
)

if not exist %EBPRO_CONVERT_LABEL_EXE% (
    echo FATAL ERROR  %EBPRO_CONVERT_LABEL_EXE%  does not exist
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

%EBPRO_CONVERT_LABEL_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_CONVERT_LABEL_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 30
)

copy %HMI_LABELS_TXT% %FIRST_LABELS_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %HMI_LABELS_TXT% %FIRST_LABELS_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 40
)

%EBPRO_BUILD_LABEL_EXE%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_BUILD_LABEL_EXE%  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

copy %EBPRO_LABELS_TO_IMPORT_LBL% %FIRST_LABELS_TO_IMPORT_LBL%
if ERRORLEVEL 1 (
    echo ERROR  copy %EBPRO_LABELS_TO_IMPORT_LBL% %FIRST_LABELS_TO_IMPORT_LBL%  failed - %ERRORLEVEL%
    pause
    exit /B 60
)

%EBPRO_CONVERT_LABEL_EXE% %EBPRO_LABELS_TO_IMPORT_LBL%
if ERRORLEVEL 1 (
    echo ERROR  %EBPRO_CONVERT_LABEL_EXE% %EBPRO_LABELS_TO_IMPORT_LBL%  failed - %ERRORLEVEL%
    pause
    exit /B 70
)

copy %HMI_LABELS_TXT% %SECOND_LABELS_TXT%
if ERRORLEVEL 1 (
    echo ERROR  copy %HMI_LABELS_TXT% %SECOND_LABELS_TXT%  failed - %ERRORLEVEL%
    pause
    exit /B 80
)

echo INSTRUCTION  File must be equal except for the header line

%KDiff3_EXE% %FIRST_LABELS_TXT% %SECOND_LABELS_TXT%

rem ===== End ===============================================================

echo OK
