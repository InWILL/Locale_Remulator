using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using SharpShell.Attributes;
using SharpShell.SharpContextMenu;
using System.Collections.Generic;
using System.Linq;
using System.Diagnostics;
using System.Reflection;
using LRCSharpLibrary;

namespace LRSubMenus
{
	[ComVisible(true)]
	[COMServerAssociation(AssociationType.AllFiles)]
	//[COMServerAssociation(AssociationType.ClassOfExtension,".*")]
	//[COMServerAssociation(AssociationType.Directory)]

	public class LRSubMenuExtension : SharpContextMenu
	{
		//  lets create the menu strip.
		private ContextMenuStrip menu = new ContextMenuStrip();
		private static string currentpath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
		private static string dllpath = Path.Combine(currentpath, "LRHookx64.dll");

		//identify if a executable file is 64-bit
		
		public enum BinaryType : uint
		{
			SCS_32BIT_BINARY = 0,
			SCS_64BIT_BINARY = 6,
			SCS_DOS_BINARY = 1,
			SCS_OS216_BINARY = 5,
			SCS_PIF_BINARY = 3,
			SCS_POSIX_BINARY = 4,
			SCS_WOW_BINARY = 2
		}
		public static BinaryType? GetBinaryType(string path)
		{
			using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read))
			{
				stream.Seek(0x3C, SeekOrigin.Begin);
				using (var reader = new BinaryReader(stream))
				{
					if (stream.Position + sizeof(int) > stream.Length)
						return null;
					var peOffset = reader.ReadInt32();
					stream.Seek(peOffset, SeekOrigin.Begin);
					if (stream.Position + sizeof(uint) > stream.Length)
						return null;
					var peHead = reader.ReadUInt32();
					if (peHead != 0x00004550) // "PE\0\0"
						return null;
					if (stream.Position + sizeof(ushort) > stream.Length)
						return null;
					switch (reader.ReadUInt16())
					{
						case 0x14c:
							return BinaryType.SCS_32BIT_BINARY;
						case 0x8664:
							return BinaryType.SCS_64BIT_BINARY;
						default:
							return null;
					}
				}
			}
		}

		[DllImport("Shell32.dll", CharSet = CharSet.Unicode)]
		public static extern uint FindExecutable(
			string lpFile,
			string lpDirectory,
			[Out] StringBuilder lpResult
		);

		// <summary>
		// Determines whether the menu item can be shown for the selected item.
		// </summary>
		// <returns>
		//   <c>true</c> if item can be shown for the selected item for this instance.; otherwise, <c>false</c>.
		// </returns>
		protected override bool CanShowMenu()
		{
			//  We can show the item only for a single selection.
			//string filepath = SelectedItemPaths.First().ToString();
			//BinaryType? filetype= GetBinaryType(filepath);

			//if (filetype == BinaryType.SCS_64BIT_BINARY)
			if(SelectedItemPaths.Count() == 1)
			{
				this.UpdateMenu();
				return true;
			}
			else
			{
				return false;
			}
		}

		// <summary>
		// Creates the context menu. This can be a single menu item or a tree of them.
		// Here we create the menu based on the type of item
		// </summary>
		// <returns>
		// The context menu for the shell context menu.
		// </returns>
		protected override ContextMenuStrip CreateMenu()
		{
			menu.Items.Clear();
			var filepath = SelectedItemPaths.First();
			FileAttributes attr = File.GetAttributes(filepath);

			// check if the selected item is a directory
			if (attr.HasFlag(FileAttributes.Directory)) 
			{
				//this.MenuDirectory();
			}
			else
			{
				var extension = Path.GetExtension(filepath);
				if (LRConfig.FileType.Equals("exe"))
				{
					if (!extension.Equals(".exe"))
					{
						return menu;
					}
				}
				this.MenuFiles();
			}

			// return the menu item
			return menu;
		}

		// <summary>
		// Updates the context menu. 
		// </summary>
		private void UpdateMenu()
		{
			// release all resources associated to existing menu
			menu.Dispose();
			menu = CreateMenu();
		}

		// <summary>
		// Creates the context menu when the selected item is a folder.
		// </summary>
		/*protected void MenuDirectory()
		{
			ToolStripMenuItem MainMenu;
			MainMenu = new ToolStripMenuItem
			{
				Text = "MenuDirectory",
				Image = Properties.Resources.Folder_icon
			};

					ToolStripMenuItem SubMenu1;
					SubMenu1 = new ToolStripMenuItem
					{
						Text = "DirSubMenu1",
						Image = Properties.Resources.Folder_icon
					};

					var SubMenu2 = new ToolStripMenuItem
					{
						Text = "DirSubMenu2",
						Image = Properties.Resources.Folder_icon
					};
					SubMenu2.DropDownItems.Clear();
					SubMenu2.Click += (sender, args) => ShowItemName();

							var SubSubMenu1 = new ToolStripMenuItem
							{
								Text = "DirSubSubMenu1",
								Image = Properties.Resources.Folder_icon
							};
							SubSubMenu1.Click += (sender, args) => ShowItemName();
			
			// Lets attach the submenus to the main menu
			SubMenu1.DropDownItems.Add(SubSubMenu1);
			MainMenu.DropDownItems.Add(SubMenu1);
			MainMenu.DropDownItems.Add(SubMenu2);

			menu.Items.Clear();
			menu.Items.Add(MainMenu);
		}*/

		// <summary>
		// Creates the context menu when the selected item is of file type.
		// </summary>
		protected void MenuFiles()
		{
			LRProfile[] profiles = LRConfig.GetProfiles();

			ToolStripMenuItem MainMenu;
			MainMenu = new ToolStripMenuItem
			{
				Text = "Locale Remulator x64",
				Image = Properties.Resources.file_red
			};
			foreach (var profile in profiles)
			{
				var SubMenu = new ToolStripMenuItem
				{
					Text = profile.Name,
					Image = Properties.Resources.file_blue
				};
				SubMenu.DropDownItems.Clear();
				SubMenu.Click += (sender, args) => CallInject(profile);
				MainMenu.DropDownItems.Add(SubMenu);
			}

			MainMenu.DropDownItems.Add("-");

			var Menu_Editor = new ToolStripMenuItem
			{
				Text = "Edit Profile List",
				Image = Properties.Resources.file_red
			};
			Menu_Editor.DropDownItems.Clear();
			Menu_Editor.Click += (sender, args) => CallLREditor();
			MainMenu.DropDownItems.Add(Menu_Editor);
			// Lets attach the submenus to the main menu

			menu.Items.Clear();
			menu.Items.Add(MainMenu);
		}

		// <summary>
		// Shows name of selected files.
		// </summary>
		private void CallInject(LRProfile profile)
		{
			var proc = new Process();
			var filepath = SelectedItemPaths.First();
			var filedirectory = Path.GetDirectoryName(filepath);

			var extension = Path.GetExtension(filepath);
			var command = filepath;
			if(!extension.Equals(".exe"))
			{
				const int MAX_PATH = 260;
				var associationpath = new StringBuilder(MAX_PATH);
				uint handle = FindExecutable(filepath, null, associationpath);
				if(handle > 32)
				{
					command = associationpath + " " + command;
				}
			}

			proc.StartInfo.FileName = currentpath+"\\LRProc.exe";
			proc.StartInfo.Arguments = profile.Guid + " " + command;
			proc.StartInfo.WorkingDirectory = filedirectory;
			if (profile.RunAsAdmin) proc.StartInfo.Verb = "runas";
			//MessageBox.Show(currentpath);
			proc.Start();
		}
		private void CallLREditor()
		{
			var proc = new Process();

			Directory.SetCurrentDirectory(currentpath);
			proc.StartInfo.FileName = "LREditor.exe";
			//MessageBox.Show(currentpath);
			proc.Start();
		}
	}
}