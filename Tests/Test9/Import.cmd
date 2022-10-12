@echo off

rem Author    KMS - Martin Dubois, P. Eng.
rem Copyright (C) 2022 KMS
rem License   http://www.apache.org/licenses/LICENSE-2.0
rem Product   KMS-PLC
rem File      Tests/Test9/Import.cmd

echo Executing Tests/Test9/Import.cmd %1 ...

rem ===== Initialisation ====================================================

set SRC_FOLDER=%1

rem ===== Verification ======================================================

if "" == "%1" (
    echo USER ERROR  The command line is not valid
    echo Usage  .\Import.cmd {SourceFolder}
    pause
    exit /B 10
)

if not exist %SRC_FOLDER% (
    echo USER ERROR  %SRC_FOLDER%  does not exist
    pause
    exit /B 20
)

rem ===== Execution =========================================================

if not exist PLC mkdir PLC
if not exist HMI mkdir HMI

copy %SRC_FOLDER%\KMS-PLC.cfg KMS-PLC.cfg
copy %SRC_FOLDER%\PLC\*.PC6 PLC
copy %SRC_FOLDER%\HMI\*.csv HMI
copy %SRC_FOLDER%\HMI\*.lbl.in0 HMI

rem ===== End ===============================================================

echo OK
