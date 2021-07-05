; Script name: Adapt It Unicode Git.iss
; Author: Bill Martin
; Purpose: This script creates our stand-alone git downloader / installer Git_Downloader_<version>_4AI.exe 
;          and copies it into the 'adaptit/setup Git' staging folder of the developer's adaptit 
;          working tree. The main Windows Inno Setup installers expect to find this download installer in
;          that location. This Git downloader is called/used by those main Adapt It installers in a 
;          ssPostInstall step within those Adapt It installers. The executable produced by this script, 
;          when invoked (by a main AI installer or from within the AI App's Tools menu), also places a copy 
;          of itself, Git_Downloader_<version>_4AI.exe in the user's AI installation folder at:
;          C:\Program Files (x86)\Adapt It WX Unicode\ for use by the CInstallGitOptionsDlg::OnRadioDownloadGitAndInstall() 
;          method - which gets called when a user chooses Tools > Install the Git program... menu item and 
;          opts to "Download and Install Git from the Internet". That OnRadioDownloadGitAndInstall() 
;          choice/routine first determines if Git has previously been installed, and if so, queries the 
;          user whether git should be re-installed or not. 
; Usage: IMPORTANT: COMPILE THIS SCRIPT BEFORE COMPILING THE OTHER Windows installer ISS SCRIPTS!
;        The main Windows installer scripts won't compile unless this script is compiled first.
;   This script was initially generated by the Inno Setup Script Wizard.
;   See the Inno Setup Documentation for details on creading Inno Setup script files.
; Revisions:
; whm 28Jun2021 made extensive revisions. This script now gets the appropriate versions from a
; separate VersionNumbersAIandGit.h header file. It now uses Inno Setup's built-in downloading 
; capability instead of its original dependence on the ITD downloader. Also brought some deprecated 
; symbols up to date, renamed some defines, and streamlined some of the code.
;
; IMPORTANT: RUN THis Adapt It Unicode Git.iss SCRIPT TO CREATE Git_Downloader_<version>_4AI.exe
; BEFORE COMPILING THE OTHER ISS SCRIPTS.

; ALSO IMPORTANT TODO: designate the version numbers in the following header file within the
; Inno_Setup_Scripts folder. The version numbers in this header file are the only location
; where all the Inno Setup scripts get their version numbers in the form of:
;   #define AI_VERSION_MAJOR 6
;   #define AI_VERSION_MINOR 10
;   #define AI_REVISION 4
; and
;   #define GIT_VERSION_MAJOR 2
;   #define GIT_VERSION_MINOR 32
;   #define GIT_REVISION 0
; Note: Although the line below includes the AI_... version defines, they are not used in this 
; script, but only in the main AI installer .iss scripts
#include "VersionNumbersAIandGit.h"


#define GitDownloaderAppName "Git Downloader for Adapt It"
#define GitAppVersion Str(GIT_VERSION_MAJOR) + "." +Str(GIT_VERSION_MINOR) + "." + Str(GIT_REVISION) ; 2.32.0
#define GitAppUnderscoredVersion Str(GIT_VERSION_MAJOR) + "_" +Str(GIT_VERSION_MINOR) + "_" + Str(GIT_REVISION) ; 2_32_0
#define GitAppShortName "Git Downloader"
#define GitInsAppPrefix "Git-"
#define GitInsAppSuffix "-32-bit"
#define GitAppExe ".exe"
#define GitInstallerName GitInsAppPrefix + GitAppVersion + GitInsAppSuffix
#define GitInstallerFullName GitInstallerName + GitAppExe ; "Git-2.32.0-32-bit.exe"
#define GitDownloaderBaseFileName "Git_Downloader_" + GitAppUnderscoredVersion + "_4AI"
#define GitDownloaderFullFileName GitDownloaderBaseFileName + GitAppExe ; "Git_Downloader_2_32_0_4AI.exe"
#define GitDownloaderCopyright "2021 by Bruce Waters, Bill Martin, SIL International"
#define GitDownloaderPublisher "Adapt It"
#define AIAppURL "https://www.adapt-it.org/"
#define GitGitHubSetupURL "https://github.com/git-for-windows/git/releases/download/v" + GitAppVersion + ".windows.1/" + GitInstallerFullName
#define RepoBase ".."

[Setup]

; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={{DB254DB1-ECD3-4583-A392-DC64CB84999F}
AppName={#GitDownloaderAppName}
AppVersion={#GitAppVersion}
AppVerName={#GitDownloaderAppName} {#GitAppVersion}
AppPublisher={#GitDownloaderPublisher}
AppPublisherURL={#AIAppURL}
AppSupportURL={#AIAppURL}
AppUpdatesURL={#AIAppURL}
DefaultDirName={commonpf}\{#GitDownloaderAppName}
DefaultGroupName=
LicenseFile=
InfoBeforeFile=
OutputBaseFilename={#GitDownloaderBaseFileName}
SetupIconFile={#RepoBase}\res\ai_32.ico
Compression=lzma/Max
SolidCompression=true
OutputDir={#RepoBase}\setup Git
VersionInfoCopyright={#GitDownloaderCopyright}
VersionInfoProductName={#GitDownloaderAppName}
VersionInfoProductVersion={#GitAppVersion}
WizardImageFile="{#RepoBase}\res\ai_wiz_bg.bmp"
WizardSmallImageFile="{#RepoBase}\res\AILogo32x32.bmp"
WizardImageStretch=false
AppCopyright={#GitDownloaderCopyright}
PrivilegesRequired=admin
DirExistsWarning=no
VersionInfoVersion={#GitAppVersion}
VersionInfoCompany=
VersionInfoDescription={#GitDownloaderAppName}
UsePreviousGroup=false
UsePreviousAppDir=false
DisableWelcomePage=true
DisableDirPage=true
DisableReadyPage=yes
ShowLanguageDialog=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

; The [Tasks] section is not needed in this script

[Files]
; When this scrip is executed, the file below will be downloaded to {tmp} from the Git GitHub website: 
;    https://github.com/git-for-windows/git/releases/download/v2.32.0.windows.1/Git-2.32.0-32-bit.exe
; and a successful download will automatically place the git installer in the "DestDir:" location which
; (when {commonpf} is expanded) resolves to C:\Program Files (x86)\Adapt It WX Unicode\
; (see the DownloadPage creation and implementation in the [Code] section below.)
Source: "{tmp}\{#GitInstallerFullName}"; DestDir: "{commonpf}\Adapt It WX Unicode"; Flags: external

; The [Registry] section is not needed in this script. See its use in the Adapt It Unicode.iss script
;[Registry]

; The following messages get substituted for the default messages within the Setup app.
; See: https://jrsoftware.org/ishelp/index.php?topic=messagessection and the Default.isl file 
; in your C:\Program Files (x86)\Inno Setup 6\ directory for an inventory of "label"s that can
; be customized.
[Messages]
ReadyLabel1=Setup is now ready to begin downloading and installing the Git program on your computer. Setup will first install the [name].
DownloadingLabel=Downloading additional files and executing the Git installer... 
FinishedHeadingLabel=Completing the Git Setup Wizard
FinishedLabelNoIcons=Setup has finished installing the [name] and the Git program on your computer.

; whm 23 Mar 2017: Code changes to download / install Git
; whm 28Jun2021 updated the 32-bit Git download URL from adapt-it.org back to GitHub which is the actual repository.
; The durrent download URL is: https://github.com/git-for-windows/git/releases/download/v2.32.0.windows.1/Git-2.32.0-32-bit.exe
; For Windows, AI is a 32-bit application so we can download and use the 32-bit version of git.
[Code]
// whm 23Mar2017 updated the 32-bit Git download URL from adapt-it.org to to use an updated Git version 2.12.1.
// whm 25Jun2018 updated the 32-bit Git download URL from adapt-it.org to to use an updated Git version 2.18.0.
// whm 28Jun2021 updated the 32-bit Git download URL from adapt-it.org to Git version 2.32.0, and the 
// GitSetupURL to use https://github.com/git-for-windows/git/releases/download/v2.32.0.windows.1/Git-2.32.0-32-bit.exe

const GitInstallerFileName = '{#GitInstallerFullName}'; // 'Git-2.32.0-32-bit.exe';
const GitSetupURL = '{#GitGitHubSetupURL}';  //'https://github.com/git-for-windows/git/releases/download/v2.32.0.windows.1/Git-2.32.0-32-bit.exe'
  BN_CLICKED = 0;
  WM_COMMAND = $0111;
  CN_BASE = $BC00;
  CN_COMMAND = CN_BASE + WM_COMMAND;

var tmpResult: Integer;     
    DownloadPage: TDownloadWizardPage;  // whm added 28Jun2021

// whm 28Jun2021 added the following OnDownloadProgress function (from Inno example CodeDownloadFiles.iss
function OnDownloadProgress(const Url, FileName: String; const Progress, ProgressMax: Int64): Boolean;
begin
  if Progress = ProgressMax then
    Log(Format('Successfully downloaded file to {tmp}: %s', [FileName]));
  Result := True;
end;
    
procedure InitializeWizard();
begin
    // whm 28Jun2021 added following initialization of DownloadPage
    DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), @OnDownloadProgress);
end;

// The CurPageChanged() procedure is not needed for our git downloader as there is nothing
// by way of initial defaults to be set.

// The NextButtonClick() function is called when the user clicks the Next button.
// The CurPage's controls can be read at this point and variables assigned from
// the controls' values.
// If the function is set to a True Result, the wizard will move to the next page.
// If the function is set to a False Result, it will remain on the current page.
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  if CurPageID = wpReady then begin
    DownloadPage.Clear;
    //DownloadPage.Add('https://github.com/git-for-windows/git/releases/download/v2.32.0.windows.1/Git-2.32.0-32-bit.exe', 'Git-2.32.0-32-bit.exe', '');
    DownloadPage.Add(GitSetupURL, GitInstallerFileName, '');
    DownloadPage.Show;
    try
      try
        DownloadPage.Download;
        Result := True;
      except
        SuppressibleMsgBox(AddPeriod(GetExceptionMessage), mbCriticalError, MB_OK, IDOK);
        Result := False;
      end;
    finally
      DownloadPage.Hide;

      // whm 28Jun2021 Note: the [Files] entry line before this [Code] section that reads:
      //   Source: "{tmp}\{#GitInstallerFullName}"; DestDir: "{app}"; Flags: external 
      // automatically makes a copy of the downloaded installer from the tmp location to 
      // C:\Program Files (x86)\Adapt It WX Unicode\ folder where Adapt It itself is 
      // installed. That is the primary location where our code in InstallGitOptionsDlg.cpp 
      // looks for git installer, whether it is copied there by this downloader, or previously
      // by the main Adapt It program installer. 
    end;

  end else
    Result := True;
end;

// The CurStepChanged() procedure can be used to perform your own pre-install and 
// post-install tasks. If CurStep=ssInstall is used it executes just before the actual 
// installation starts, with CurStep=ssPostInstall it executes just after the actual 
// installation finishes, and with CurStep=ssDone just before Setup terminates after 
// a successful install. We use the ssPostInstall step to execute the git installer 
// that was downloaded to {tmp} by the NextButtonClick() function
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep=ssPostInstall then
    // Silently run the Git installer as a post-install step.
    // whm 28Jun2021 modified to always execute the installer from its tmp location, 
    // and run the git installer silently, with the options '/SILENT /NORESTART'.
    // More info on the command line options for Inno installers can be found here: 
    //   http://www.jrsoftware.org/ishelp/index.php?topic=setupcmdline
    // The installer also needs to be run with the /NORESTART switch to suppress 
    // the restart message. 
    // Note: The "Needs Restart" message is provided in the AI source code in the
    // CInstallGitOptionsDlg class handlers.
    Exec(ExpandConstant('{tmp}\Git-2.32.0-32-bit.exe'), '/SILENT /NORESTART', '', SW_SHOW, ewWaitUntilTerminated, tmpResult);
end;

// The NotOnPathAlready() function is not currently called in this script.
// When called it is used in the [Registry] section to ensure the Windows 
// path to Git is set properly.
function NotOnPathAlready(): Boolean;
var
  BinDir, Path: String;
begin
  if RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', Path) then
  begin 
    // Successfully read the value
    Log('HKCU\Environment\PATH = ' + Path);
    BinDir := ExpandConstant('{commonpf}\Git\bin');
    Log('Looking for Git\bin dir in %PATH%: ' + BinDir + ' in ' + Path);
    if Pos(LowerCase(BinDir), Lowercase(Path)) = 0 then
    begin
      Log('Did not find Git\bin dir in %PATH% so will add it');
      Result := True;
    end
    else
    begin
      Log('Found Git\bin dir in %PATH% so will not add it again');
      Result := False;
    end
  end
  else // The key probably doesn't exist
  begin
    Log('Could not access HKCU\Environment\PATH so assume it is ok to add it');
    Result := True;
  end;
end;

// The NeedRestart() function Return True to instruct Setup to prompt the user 
// to restart the system at the end of a successful installation, False otherwise.
function NeedRestart(): Boolean;
begin
  // Returning False from this function helps to suppress the "Needs Restart" message.
  // The "Needs Restart" message is provided in the AI CInstallGitOptionsDlg class handlers. 
  Result := False;
end;

// We omit a [Run] section in this script to suppress the launch option at end of installation
