using System;
using System.Linq;
using System.Windows;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Principal;
using LRCSharpLibrary;

namespace LRInstaller
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly string crtDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Install_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var proc = new Process();
                proc.StartInfo.FileName = "ServerRegistrationManager.exe";
                proc.StartInfo.Arguments = "install LRSubMenus.dll -codebase";
                proc.StartInfo.Verb = "runas";
                proc.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void Button_Uninstall_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var proc = new Process();
                proc.StartInfo.FileName = "ServerRegistrationManager.exe";
                proc.StartInfo.Arguments = "uninstall LRSubMenus.dll";
                proc.StartInfo.Verb = "runas";
                proc.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void Restart_Explorer()
        {
            var explorerProcess = Process.GetProcesses().FirstOrDefault(p => p.ProcessName == "explorer");
            if (explorerProcess != null)
                explorerProcess.Kill();
        }

        private void Button_Restart_Explorer_Click(object sender, RoutedEventArgs e)
        {
            Restart_Explorer();
        }

        private void Button_LREditor_Click(object sender, RoutedEventArgs e)
        {
            var proc = new Process();
            proc.StartInfo.FileName = "LREditor.exe";
            proc.Start();
        }

        private void Button_FileType_EXE_Click(object sender, RoutedEventArgs e)
        {
            LRConfig.FileType = "exe";
            LRProfile[] profiles = LRConfig.GetProfiles();
            LRConfig.WriteConfig(profiles);
            Restart_Explorer();
        }

        private void Button_FileType_ALL_Click(object sender, RoutedEventArgs e)
        {
            LRConfig.FileType = "all";
            LRProfile[] profiles = LRConfig.GetProfiles();
            LRConfig.WriteConfig(profiles);
            Restart_Explorer();
        }
    }
}
