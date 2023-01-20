using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Diagnostics;
using System.IO;
using LRCSharpLibrary;
using System.Globalization;
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
		private readonly List<TimeZoneInfo> timezones = new List<TimeZoneInfo>();
		private readonly string dllpath = Path.Combine(LRConfig.CurrentPath, "LRHookx64.dll");
		public MainWindow()
		{
			InitializeComponent();
			
			profiles = LRConfig.GetProfiles().ToList();
			ComboBox_Profile.ItemsSource = profiles;

			cultureinfos = CultureInfo.GetCultures(CultureTypes.AllCultures).ToList();
			ComboBox_CodePage.ItemsSource = cultureinfos.Select(c => c.DisplayName);

			timezones = TimeZoneInfo.GetSystemTimeZones().ToList();
			ComboBox_TimeZone.ItemsSource = timezones;
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
			p.LCID = (uint)cultureinfos[ComboBox_CodePage.SelectedIndex].TextInfo.LCID;
			p.TimeZone = timezones[ComboBox_TimeZone.SelectedIndex].Id;
			p.Bias = timezones[ComboBox_TimeZone.SelectedIndex].BaseUtcOffset.TotalMinutes;
            p.RunAsAdmin = CheckBox_RunAsAdmin.IsChecked ?? false;
			p.HookIME = CheckBox_IME.IsChecked ?? false;
			p.HookLCID = CheckBox_LCID.IsChecked ?? false;
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
				p.LCID = (uint)cultureinfos[ComboBox_CodePage.SelectedIndex].TextInfo.LCID;
				p.TimeZone = timezones[ComboBox_TimeZone.SelectedIndex].Id;
				p.Bias = timezones[ComboBox_TimeZone.SelectedIndex].BaseUtcOffset.TotalMinutes;
                p.RunAsAdmin = CheckBox_RunAsAdmin.IsChecked ?? false;
				p.HookIME = CheckBox_IME.IsChecked ?? false;
				p.HookLCID = CheckBox_LCID.IsChecked ?? false;
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
			ComboBox_CodePage.SelectedIndex = cultureinfos.FindIndex(c => c.Name == p.Location);
			ComboBox_TimeZone.SelectedIndex = timezones.FindIndex(c => c.Id == p.TimeZone);
			CheckBox_RunAsAdmin.IsChecked = p.RunAsAdmin;
			CheckBox_IME.IsChecked = p.HookIME;
			CheckBox_LCID.IsChecked = p.HookLCID;
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
				string filepath = openFileDlg.FileName;
				LRProfile profile = (LRProfile)ComboBox_Profile.SelectedItem;
				WshShell shell = new WshShell();
				string shortcutAddress = string.Format("{0}.{1}.lnk", filepath, profile.Name);
				IWshShortcut shortcut = (IWshShortcut)shell.CreateShortcut(shortcutAddress);
				string CommandLine = filepath;
				if (TextBox_Arguments.Text != "Enter Arguments here...")
				{
					CommandLine = "\"" + filepath + "\" " + TextBox_Arguments.Text;
				}
				shortcut.TargetPath = LRConfig.CurrentPath + "\\LRProc.exe";
				shortcut.Arguments = profile.Guid + " " + CommandLine;
				shortcut.IconLocation = filepath;
				shortcut.WorkingDirectory = Path.GetDirectoryName(filepath);
				try
				{
					// Add administrator authorized
					shortcut.Save();
					if (profile.RunAsAdmin)
						using (FileStream stream = new FileStream(shortcutAddress, FileMode.Open, FileAccess.ReadWrite))
						{
							stream.Seek(21, SeekOrigin.Begin);
							stream.WriteByte(0x22);
						}
				}
				catch (UnauthorizedAccessException ex)
				{
					var proc = new Process();
					proc.StartInfo.FileName = LRConfig.CurrentPath + "\\LREditor.exe";
					proc.StartInfo.Arguments = "\"" + filepath + "\" " + profile.Guid + " \"" + shortcut.Arguments + "\"";
					proc.StartInfo.WorkingDirectory = Path.GetDirectoryName(filepath);
					proc.StartInfo.UseShellExecute = true;
					proc.StartInfo.Verb = "runas";
					proc.Start();
				}
			}
		}

		private void Button_Run_Click(object sender, RoutedEventArgs e)
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
				string filepath = openFileDlg.FileName;
				string CommandLine = filepath;
				if (TextBox_Arguments.Text != "Enter Arguments here...")
				{
					CommandLine = "\"" + filepath + "\" " + TextBox_Arguments.Text;
				}
				LRProfile profile = (LRProfile)ComboBox_Profile.SelectedItem;
				var proc = new Process();
				proc.StartInfo.FileName = LRConfig.CurrentPath + "\\LRProc.exe";
				proc.StartInfo.Arguments = profile.Guid + " " + CommandLine;
				proc.StartInfo.WorkingDirectory = Path.GetDirectoryName(filepath);
				proc.StartInfo.UseShellExecute = true;
				if (profile.RunAsAdmin) proc.StartInfo.Verb = "runas";
				proc.Start();
			}
		}

		private void TextBox_Arguments_GotFocus(object sender, RoutedEventArgs e)
		{
			if (TextBox_Arguments.Text == "Enter Arguments here...")
			{
				TextBox_Arguments.Text = "";
			}
		}

		private void TextBox_Arguments_LostFocus(object sender, RoutedEventArgs e)
		{
			if (string.IsNullOrWhiteSpace(TextBox_Arguments.Text))
			{
				TextBox_Arguments.Text = "Enter Arguments here...";
			}
		}
	}
}
