#define MyAppName "R-Type"
#define MyAppVersion "1.0"
#define MyAppPublisher "PileAA Corp."
#define MyAppExeName "r-type_client.exe"
#define BuildType "Debug"

[Setup]
AppId={{69FE69E3-4D43-4A21-8671-4D88AD8D6807}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
;PrivilegesRequired=lowest
OutputBaseFilename=r-type_setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]  
Source: "..\build\target\*"; DestDir: "{app}\"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\Client\binary\{#MyAppExeName}"
Name: "{autoprograms}\{#MyAppName} Server"; Filename: "{app}\Server\binary\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\Client\binary\\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\Client\binary\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

