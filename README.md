# Locale Remulator

[![license](https://img.shields.io/github/license/InWILL/Locale_Remulator.svg)](https://www.gnu.org/licenses/lgpl-3.0.en.html)
[![GitHub all releases](https://img.shields.io/github/downloads/InWILL/Locale_Remulator/total)](https://github.com/InWILL/Locale_Remulator/releases/latest)

类似于Locale Emulator，但是没有64位，所以基于Detours重新实现了一个，外围部分参照了LE的代码。

最主要的原因是日服MapleStory要实装64位了。

wiki在做了在做了（咕咕咕······

System Region and Language Simulator.

The project is similar with Locale-Emulator, but LE doesn't support 64-bit application, so I base on Detours to start a new project.

The most important reason is that Japan MapleStory will become 64-bit.

## Download

Download available at <https://github.com/InWILL/Locale_Remulator/releases/latest>.

## Getting Started

### Prerequisites

* [.NET Framework 4.8](https://dotnet.microsoft.com/en-us/download/dotnet-framework/net48)
* [.NET Desktop Runtime 6.0](https://dotnet.microsoft.com/en-us/download/dotnet/6.0)

### Install

Run `LRInstaller.exe` to install Locale_Remulator.

If you installed old version in the past, please restart explorer or reboot system after installing new version.

如果先前安装过旧版LR，安装新版LR后，请重启资源管理器或者重启系统。

### Uninstall

Run `LRInstaller.exe` to remove Locale_Remulator.

### Usage Method 1

Select a `*.exe` application and right click, there will show a section named "Locale Remulator x64", and choose what config you want.

### Usage Method 2

Run `LREditor.exe` and click `Shortcut` button, choose what config and application you want to run, there will generate a lnk file in the same path. You could use this lnk file to use LR conveniently without right click menu.

## Built With

* [Detours](https://github.com/microsoft/Detours) - Used to hook ANSI/Unicode functions
* [SharpShell](https://github.com/dwmkerr/sharpshell) - Used to generate right-click menu

## Built

(1) choose platform x86 to generate LRHookx32.dll

(2) choose platform x64 to build all project.

(3) In `Locale_Remulator\LRInstaller` there will generate `LRInstaller.exe` which is the Installer application.

## Debug

Copy these files to a same folder.

```
    LRConfig.xml
    LREditor.deps.json
    LREditor.dll
    LREditor.exe
    LREditor.pdb
    LREditor.runtimeconfig.json
    LRHookx32.dll
    LRHookx64.dll
    LRProc.exe
    LRSubMenus.dll
    ServerRegistrationManager.exe
    SharpShell.dll
```

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/InWILL/Locale_Remulator/tags). 

## Authors

* **InWILL** - *Initial work* - [InWILL](https://github.com/InWILL)

See also the list of [contributors](https://github.com/InWILL/Locale_Remulator/graphs/contributors) who participated in this project.

## License

This project is licensed under the LGPL-3.0 License - see the [LICENSE](LICENSE) file for details
