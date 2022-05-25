using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.IO;
using System.Windows;
using IWshRuntimeLibrary;
using LRCSharpLibrary;

namespace LREditor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        void App_Startup(object sender, StartupEventArgs e)
        {
            if (e.Args.Length < 3)
            {
                Current.StartupUri = new Uri("LREditor.xaml", UriKind.RelativeOrAbsolute);
                return;
            }
            var filepath = e.Args[0];
            var Guid = e.Args[1];
            var CommandLine = e.Args[2];
            LRProfile profile = LRConfig.GetProfile(Guid);
            var shortcutAddress = string.Format("{0}.{1}.lnk", filepath, profile.Name);

            WshShell shell = new WshShell();
            IWshShortcut shortcut = (IWshShortcut)shell.CreateShortcut(shortcutAddress);

            shortcut.TargetPath = LRConfig.CurrentPath + "\\LRProc.exe";
            shortcut.Arguments = CommandLine;
            shortcut.IconLocation = filepath;
            shortcut.WorkingDirectory = Path.GetDirectoryName(filepath);
            shortcut.Save();
            Current.Shutdown();
        }
    }
}
