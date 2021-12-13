@ECHO OFF
ECHO ###	UnRegister DLL     ###
REM  --> code from https://sites.google.com/site/eneerge/scripts/batchgotadmin

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------


REM  --> Check OS and un-register accordingly 
:CheckOS
reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set SysOS=32BIT || set SysOS=64BIT

if %SysOS%==32BIT ( 
"%windir%\Microsoft.NET\Framework\v4.0.30319\regasm.exe" /u %cd%\LRSubMenus.dll
) else (
"%windir%\Microsoft.NET\Framework64\v4.0.30319\regasm.exe" /u %cd%\LRSubMenus.dll)

:END

PAUSE
@ECHO ON