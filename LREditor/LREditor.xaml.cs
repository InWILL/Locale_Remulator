using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.IO;
using LRCSharpLibrary;
using System.Globalization;
using System.Drawing.Text;
using Microsoft.Win32;
using IWshRuntimeLibrary;

namespace LREditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly List<LRProfile> profiles = new List<LRProfile>();
        private readonly List<CultureInfo> cultureinfos = new List<CultureInfo>();
        private readonly List<string> fontinfo = new List<string>();
        public MainWindow()
        {
            InitializeComponent();
            profiles = LRConfig.GetProfiles().ToList();
            cultureinfos = CultureInfo.GetCultures(CultureTypes.AllCultures).ToList();
            fontinfo = System.Drawing.FontFamily.Families.Select(c => c.Name).ToList();
            fontinfo.Add("None");
            ComboBox_Profile.ItemsSource=profiles;
            ComboBox_CodePage.ItemsSource = cultureinfos.Select(c=>c.DisplayName);
            ComboBox_Fonts.ItemsSource = fontinfo;
        }

        private void Button_Save_Click(object sender, RoutedEventArgs e)
        {
            if (ComboBox_Profile.SelectedIndex == -1)
            {
                return;
            }
            var p = profiles[ComboBox_Profile.SelectedIndex];
            p.Location = cultureinfos[ComboBox_CodePage.SelectedIndex].Name;
            p.CodePage = (uint)cultureinfos[ComboBox_CodePage.SelectedIndex].TextInfo.ANSICodePage;
            p.Font = fontinfo[ComboBox_Profile.SelectedIndex];
            p.RunAsAdmin = CheckBox_RunAsAdmin.IsChecked ?? false;
            p.HookIME = CheckBox_IME.IsChecked ?? false;    
            profiles[ComboBox_Profile.SelectedIndex] = p;
            LRConfig.WriteConfig(profiles.ToArray());
        }

        private void Button_SaveAs_Click(object sender, RoutedEventArgs e)
        {
            if (ComboBox_Profile.SelectedIndex == -1)
            {
                return;
            }
            var f = new InputBox();
            LRProfile item = (LRProfile)ComboBox_Profile.SelectedItem;
            if (f.ShowDialog() == true && !string.IsNullOrEmpty(f.InputResult)) 
            {
                
                var p = new LRProfile();
                p.Name = f.InputResult;
                p.Guid = Guid.NewGuid().ToString();
                p.Location = cultureinfos[ComboBox_CodePage.SelectedIndex].Name;
                p.CodePage = (uint)cultureinfos[ComboBox_CodePage.SelectedIndex].TextInfo.ANSICodePage;
                p.Font = fontinfo[ComboBox_Fonts.SelectedIndex];
                p.RunAsAdmin = CheckBox_RunAsAdmin.IsChecked ?? false;
                p.HookIME = CheckBox_IME.IsChecked ?? false;
                profiles.Add(p);
                LRConfig.WriteConfig(profiles.ToArray());
                ComboBox_Profile.Items.Refresh();
                ComboBox_Profile.SelectedItem = p;
            }
        }

        private void Button_Delete_Click(object sender, RoutedEventArgs e)
        {
            if (ComboBox_Profile.SelectedIndex == -1)
            {
                return;
            }
            LRProfile item = (LRProfile)ComboBox_Profile.SelectedItem;
            profiles.Remove(item);
            LRConfig.WriteConfig(profiles.ToArray());
            ComboBox_Profile.Items.Refresh();
        }

        private void ComboBox_Profile_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (ComboBox_Profile.SelectedIndex == -1)
            {
                return;
            }
            LRProfile p = (LRProfile)ComboBox_Profile.SelectedItem;
            ComboBox_CodePage.SelectedIndex = cultureinfos.FindIndex(c=>c.Name==p.Location);
            ComboBox_Fonts.SelectedIndex = fontinfo.FindIndex(c => c == p.Font);
            CheckBox_RunAsAdmin.IsChecked = p.RunAsAdmin;
            CheckBox_IME.IsChecked = p.HookIME;
        }

        private void Button_Shortcut_Click(object sender, RoutedEventArgs e)
        {
            if (ComboBox_Profile.SelectedIndex == -1)
            {
                return;
            }
            OpenFileDialog openFileDlg = new OpenFileDialog();
            openFileDlg.DefaultExt = ".exe"; // Default file extension
            openFileDlg.Filter = "Executable Files (.exe)|*.exe"; // Filter files by extension
            if (openFileDlg.ShowDialog() == true)
            {
                string SelectedFileName = openFileDlg.FileName;
                string dllpath = System.IO.Path.Combine(LRConfig.CurrentPath, "LRHookx64.dll");
                LRProfile p = (LRProfile)ComboBox_Profile.SelectedItem;
                WshShell shell = new WshShell();
                string shortcutAddress = string.Format("{0}.{1}.lnk", SelectedFileName,p.Name);
                IWshShortcut shortcut = (IWshShortcut)shell.CreateShortcut(shortcutAddress);
                shortcut.TargetPath = LRConfig.CurrentPath + "\\LRProc.exe";
                shortcut.Arguments = "\"" + SelectedFileName + "\" " + p.Guid + " \"" + dllpath + "\"";
                shortcut.IconLocation = SelectedFileName;
                shortcut.WorkingDirectory = System.IO.Path.GetDirectoryName(SelectedFileName);
                shortcut.Save();

                if (p.RunAsAdmin)
                    using (FileStream stream = new FileStream(shortcutAddress, FileMode.Open, FileAccess.ReadWrite))
                    {
                        stream.Seek(21, SeekOrigin.Begin);
                        stream.WriteByte(0x22);
                    }
            }
        }
    }
}
