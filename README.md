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

Run `register.bat` to register right-click menu.

If you installed old version in the past, please restart explorer or reboot system after registering new version.

### Uninstall

Run `unregister.bat` to remove right-click menu.

### Usage

Select a `*.exe` application and right click, there will show a section named "Locale Remulator x64", and choose what config you want.

## Built

### Prerequisites

* [Detours](https://github.com/microsoft/Detours) - Used to hook unicode functions
* [SharpShell](https://github.com/dwmkerr/sharpshell) - Used to generate right-click menu

### Compile

choose solution Platform x86 or x64.

Copy these files at the same Location

```
unregister.bat
LREditor.dll
LREditor.exe
LREditor.runtimeconfig.json
LRHook.dll
LRProc.dll
LRProc.exe
LRSubMenus.dll
register.bat
SharpShell.dll
```

## License

This project is licensed under the LGPL-3.0 License - see the [LICENSE](LICENSE) file for details
