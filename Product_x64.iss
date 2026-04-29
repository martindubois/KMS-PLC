
; Author    KMS - Martin Dubois, P. Eng.
; Copyright (C) 2024-2026 KMS
; License   http://www.apache.org/licenses/LICENSE-2.0
; Product   KMS-PLC
; File      Product_x64.iss

[Setup]
AppName = KMS-PLC
AppPublisher = KMS
AppPublisherURL = https://www.kms-quebec.com
AppSupportURL = https://www.kms-quebec.com
AppVersion = 2.0.0-dev
ArchitecturesInstallIn64BitMode = x64
DefaultDirName = {commonpf}\KMS-PLC
OutputBaseFilename = KMS-PLC_2.0.0-dev_x64
OutputDir = Installer

[Files]
Source: "_DocUser\Documentation.html"                       ; DestDir: {app}; Flags: isreadme
Source: "_DocUser\KMS-PLC.ReadMe.txt"                       ; DestDir: {app}
Source: "Import\KMS.ReadMe.txt"                             ; DestDir: {app}
Source: "Import\KMS-Tools.ComTool.ReadMe.txt"               ; DestDir: {app}
Source: "Import\KMS-Tools.ModbusSim.ReadMe.txt"             ; DestDir: {app}
Source: "Import\KMS-Tools.ModbusTool.ReadMe.txt"            ; DestDir: {app}
Source: "Import\KMS-Tools.ReadMe.txt"                       ; DestDir: {app}
Source: "Import\Binaries\Release_Static_x64\ComTool.exe"    ; DestDir: {app}
Source: "Import\Binaries\Release_Static_x64\ModbusSim.exe"  ; DestDir: {app}
Source: "Import\Binaries\Release_Static_x64\ModbusTool.exe" ; DestDir: {app}
Source: "KMS-PLC\_DocUser\KMS-PLC.KMS-PLC.ReadMe.txt"       ; DestDir: {app}
Source: "Scripts"                                           ; DestDir: {app}
Source: "Templates"                                         ; DestDir: {app}
Source: "x64\Release_Static\EBPro_Build_Address.exe"        ; DestDir: {app}
Source: "x64\Release_Static\EBPro_Build_Label.exe"          ; DestDir: {app}
Source: "x64\Release_Static\EBPro_Convert_Address.exe"      ; DestDir: {app}
Source: "x64\Release_Static\EBPro_Convert_Label.exe"        ; DestDir: {app}
Source: "x64\Release_Static\TriLogi_Build.exe"              ; DestDir: {app}
Source: "x64\Release_Static\TriLogi_Convert.exe"            ; DestDir: {app}
