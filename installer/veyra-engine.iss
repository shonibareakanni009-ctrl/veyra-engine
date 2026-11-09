#ifndef AppVersion
  #define AppVersion "0.2.0"
#endif

[Setup]
AppId=VeyraEngine
AppName=Veyra Engine
AppVersion={#AppVersion}
AppVerName=Veyra Engine {#AppVersion}
AppPublisher=Veyra Project
DefaultDirName={localappdata}\Programs\Veyra Engine
DefaultGroupName=Veyra Engine
PrivilegesRequired=lowest
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DisableProgramGroupPage=yes
UsePreviousAppDir=yes
Uninstallable=yes
UninstallDisplayIcon={app}\editor\VeyraEditor.exe
OutputDir=..\build\installer
OutputBaseFilename=VeyraEngine-Setup-{#AppVersion}-win64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes
CloseApplications=yes
RestartApplications=no
VersionInfoVersion={#AppVersion}
VersionInfoProductName=Veyra Engine
VersionInfoCompany=Veyra Project

[Files]
Source: "..\build\installer-stage\editor\*"; DestDir: "{app}\editor"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\build\installer-stage\runtime\*"; DestDir: "{app}\runtime"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\build\installer-stage\updater\*"; DestDir: "{app}\updater"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\Veyra Engine\Veyra Editor"; Filename: "{app}\editor\VeyraEditor.exe"; WorkingDir: "{app}\editor"
Name: "{autoprograms}\Veyra Engine\Run Sample Scene"; Filename: "{app}\runtime\VeyraRuntime.exe"; Parameters: """{app}\runtime\first-scene.vscene"""; WorkingDir: "{app}\runtime"
Name: "{autodesktop}\Veyra Editor"; Filename: "{app}\editor\VeyraEditor.exe"; WorkingDir: "{app}\editor"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Run]
Filename: "{app}\editor\VeyraEditor.exe"; Description: "Launch Veyra Editor"; Flags: postinstall nowait skipifsilent

; User projects and settings are deliberately stored outside {app}.
; No UninstallDelete entries are used, so uninstall does not remove user content.
