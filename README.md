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

## Deployment

choose solution Platform x86 or x64.

Copy these file to in a same directory.

```
    LREditor.exe
    LRHookx32.dll
    LRHookx64.dll
    LRInstaller.exe
    LRProc.exe
    LRSubMenus.dll
    ServerRegistrationManager.exe
    SharpShell.dll
    System.Drawing.Common.dll
```

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/InWILL/Locale_Remulator/tags). 

## Contributing

If you want to help translating Locale Remulator, you could help to tranlate readme, I have created some blank readme files.

After you translated the above into your language, please inform me by creating a pull request.

## Contributors

* **InWILL** - *Initial work* - [InWILL](https://github.com/InWILL)
* **lintx** - [lintx](https://github.com/lintx)

See also the list of [contributors](https://github.com/InWILL/Locale_Remulator/graphs/contributors) who participated in this project.

## License

This project is licensed under the LGPL-3.0 License - see the [LICENSE](LICENSE) file for details
