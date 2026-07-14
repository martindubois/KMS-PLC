@echo off

rem This file has been genereted using KMS-Framework


rem ===== Verify =====


if not exist C:\_VC\KMS-PLC\_DocUser\KMS-PLC.ReadMe.txt (
    echo "ERROR  C:\_VC\KMS-PLC\_DocUser\KMS-PLC.ReadMe.txt  does not exist"
    pause
    exit /B 10
)

if not exist C:\_VC\KMS-PLC\Import\KMS.ReadMe.txt (
    echo "ERROR  C:\_VC\KMS-PLC\Import\KMS.ReadMe.txt  does not exist"
    pause
    exit /B 20
)

if not exist "C:\Program Files (x86)\Inno Setup 6\Compil32.exe" (
    echo "ERROR  C:\Program Files (x86)\Inno Setup 6\Compil32.exe  does not exist"
    pause
    exit /B 30
)

if not exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Msbuild\Current\Bin\amd64\MSBuild.exe" (
    echo "ERROR  C:\Program Files\Microsoft Visual Studio\2022\Professional\Msbuild\Current\Bin\amd64\MSBuild.exe  does not exist"
    pause
    exit /B 40
)


rem ===== Pre-Build =====



rem ===== Compile =====


"C:\Program Files\Microsoft Visual Studio\2022\Professional\Msbuild\Current\Bin\amd64\MSBuild.exe" Solution.sln /target:rebuild /Property:Configuration=Debug /property:Platform=x64
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Msbuild\Current\Bin\amd64\MSBuild.exe" Solution.sln /target:rebuild /Property:Configuration=Release /property:Platform=x64
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Msbuild\Current\Bin\amd64\MSBuild.exe" Solution.sln /target:rebuild /Property:Configuration=Release_Static /property:Platform=x64

rem ===== Test =====


if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Address.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Address.exe  does not exist"
    pause
    exit /B 50
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Address.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Address.pdb  does not exist"
    pause
    exit /B 60
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Address.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Address.exe  does not exist"
    pause
    exit /B 70
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Address.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Address.pdb  does not exist"
    pause
    exit /B 80
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Address.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Address.exe  does not exist"
    pause
    exit /B 90
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Address.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Address.pdb  does not exist"
    pause
    exit /B 100
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Label.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Label.exe  does not exist"
    pause
    exit /B 110
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Label.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Build_Label.pdb  does not exist"
    pause
    exit /B 120
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Label.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Label.exe  does not exist"
    pause
    exit /B 130
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Label.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Build_Label.pdb  does not exist"
    pause
    exit /B 140
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Label.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Label.exe  does not exist"
    pause
    exit /B 150
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Label.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Build_Label.pdb  does not exist"
    pause
    exit /B 160
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Address.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Address.exe  does not exist"
    pause
    exit /B 170
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Address.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Address.pdb  does not exist"
    pause
    exit /B 180
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Address.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Address.exe  does not exist"
    pause
    exit /B 190
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Address.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Address.pdb  does not exist"
    pause
    exit /B 200
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Address.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Address.exe  does not exist"
    pause
    exit /B 210
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Address.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Address.pdb  does not exist"
    pause
    exit /B 220
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Label.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Label.exe  does not exist"
    pause
    exit /B 230
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Label.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro_Convert_Label.pdb  does not exist"
    pause
    exit /B 240
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Label.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Label.exe  does not exist"
    pause
    exit /B 250
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Label.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro_Convert_Label.pdb  does not exist"
    pause
    exit /B 260
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Label.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Label.exe  does not exist"
    pause
    exit /B 270
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Label.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro_Convert_Label.pdb  does not exist"
    pause
    exit /B 280
)

if not exist C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Build.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Build.exe  does not exist"
    pause
    exit /B 290
)

if not exist C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Build.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Build.pdb  does not exist"
    pause
    exit /B 300
)

if not exist C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Build.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Build.exe  does not exist"
    pause
    exit /B 310
)

if not exist C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Build.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Build.pdb  does not exist"
    pause
    exit /B 320
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Build.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Build.exe  does not exist"
    pause
    exit /B 330
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Build.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Build.pdb  does not exist"
    pause
    exit /B 340
)

if not exist C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Convert.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Convert.exe  does not exist"
    pause
    exit /B 350
)

if not exist C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Convert.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\TRiLOGI_Convert.pdb  does not exist"
    pause
    exit /B 360
)

if not exist C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Convert.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Convert.exe  does not exist"
    pause
    exit /B 370
)

if not exist C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Convert.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\TRiLOGI_Convert.pdb  does not exist"
    pause
    exit /B 380
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Convert.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Convert.exe  does not exist"
    pause
    exit /B 390
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Convert.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\TRiLOGI_Convert.pdb  does not exist"
    pause
    exit /B 400
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro.lib  does not exist"
    pause
    exit /B 410
)

if not exist C:\_VC\KMS-PLC\x64\Debug\EBPro.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\EBPro.pdb  does not exist"
    pause
    exit /B 420
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro.lib  does not exist"
    pause
    exit /B 430
)

if not exist C:\_VC\KMS-PLC\x64\Release\EBPro.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\EBPro.pdb  does not exist"
    pause
    exit /B 440
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro.lib  does not exist"
    pause
    exit /B 450
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\EBPro.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\EBPro.pdb  does not exist"
    pause
    exit /B 460
)

if not exist C:\_VC\KMS-PLC\x64\Debug\HMI.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\HMI.lib  does not exist"
    pause
    exit /B 470
)

if not exist C:\_VC\KMS-PLC\x64\Debug\HMI.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\HMI.pdb  does not exist"
    pause
    exit /B 480
)

if not exist C:\_VC\KMS-PLC\x64\Release\HMI.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\HMI.lib  does not exist"
    pause
    exit /B 490
)

if not exist C:\_VC\KMS-PLC\x64\Release\HMI.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\HMI.pdb  does not exist"
    pause
    exit /B 500
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\HMI.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\HMI.lib  does not exist"
    pause
    exit /B 510
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\HMI.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\HMI.pdb  does not exist"
    pause
    exit /B 520
)

if not exist C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A.lib  does not exist"
    pause
    exit /B 530
)

if not exist C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A.pdb  does not exist"
    pause
    exit /B 540
)

if not exist C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A.lib  does not exist"
    pause
    exit /B 550
)

if not exist C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A.pdb  does not exist"
    pause
    exit /B 560
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A.lib  does not exist"
    pause
    exit /B 570
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A.pdb  does not exist"
    pause
    exit /B 580
)

if not exist C:\_VC\KMS-PLC\x64\Debug\PLC.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\PLC.lib  does not exist"
    pause
    exit /B 590
)

if not exist C:\_VC\KMS-PLC\x64\Debug\PLC.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\PLC.pdb  does not exist"
    pause
    exit /B 600
)

if not exist C:\_VC\KMS-PLC\x64\Release\PLC.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\PLC.lib  does not exist"
    pause
    exit /B 610
)

if not exist C:\_VC\KMS-PLC\x64\Release\PLC.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\PLC.pdb  does not exist"
    pause
    exit /B 620
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\PLC.lib (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\PLC.lib  does not exist"
    pause
    exit /B 630
)

if not exist C:\_VC\KMS-PLC\x64\Release_Static\PLC.pdb (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\PLC.pdb  does not exist"
    pause
    exit /B 640
)

if not exist C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A-Test.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A-Test.exe  does not exist"
    pause
    exit /B 650
)

"C:\_VC\KMS-PLC\x64\Debug\KMS-PLC-A-Test.exe" Groups+=Auto
if not exist C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A-Test.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A-Test.exe  does not exist"
    pause
    exit /B 660
)

"C:\_VC\KMS-PLC\x64\Release\KMS-PLC-A-Test.exe" Groups+=Auto
if not exist C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A-Test.exe (
    echo "ERROR  C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A-Test.exe  does not exist"
    pause
    exit /B 670
)

"C:\_VC\KMS-PLC\x64\Release_Static\KMS-PLC-A-Test.exe" Groups+=Auto

rem ===== Package =====


"C:\Program Files (x86)\Inno Setup 6\Compil32.exe" /cc Product_x64.iss

rem ===== Sign =====




rem ===== End =====

echo OK
exit /B 0
