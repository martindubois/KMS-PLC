
; Author    KMS - Martin Dubois, P. Eng.
; Copyright (C) 2024 KMS
; License   http://www.apache.org/licenses/LICENSE-2.0
; Product   KMS-PLC
; File      Product_x64.iss

[Setup]
AppName = KMS-PLC
AppPublisher = KMS
AppPublisherURL = https://www.kms-quebec.com
AppSupportURL = https://www.kms-quebec.com
AppVersion = 1.0.3
ArchitecturesInstallIn64BitMode = x64
DefaultDirName = {commonpf}\KMS-PLC
OutputBaseFilename = KMS-PLC_1.0.3_x64
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
Source: "x64\Release_Static\KMS-PLC.exe"                    ; DestDir: {app}

[Registry]
Root: HKCR; SubKey: "Directory\shell\kms_plc"         ; Flags: uninsdeletekey
Root: HKCR; SubKey: "Directory\shell\kms_plc"         ; ValueType: string; ValueData: "KMS-PLC Here"
Root: HKCR; SubKey: "Directory\shell\kms_plc\command" ;
Root: HKCR; SubKey: "Directory\shell\kms_plc\command" ; ValueType: string; ValueData: "cmd.exe /s /k pushd ""%V"" && ""{app}\KMS-PLC.exe"" && exit"
