using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.IO;
using System.Security.AccessControl;
using LRCSharpLibrary;

namespace LREditor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private void Application_Startup(object sender, StartupEventArgs e)
        {
            if(e.Args.Length == 0)
            {
                var f = new MainWindow();
                f.ShowDialog();
            }
            else
            switch(e.Args[0])
            {
                case "/Install":
                    LRConfig.GetProfiles();
                    FileInfo fileInfo = new FileInfo(LRConfig.ConfigPath);
                    System.Security.AccessControl.FileSecurity fileSecurity = fileInfo.GetAccessControl();
                    fileSecurity.AddAccessRule(new FileSystemAccessRule("Everyone", FileSystemRights.FullControl, AccessControlType.Allow));
                    fileSecurity.AddAccessRule(new FileSystemAccessRule("Users", FileSystemRights.FullControl, AccessControlType.Allow));
                    fileInfo.SetAccessControl(fileSecurity);
                    break;
            }
            Shutdown();
        }
    }
}
