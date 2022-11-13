using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Xml.Linq;
using System.Windows.Forms;
using System.IO.MemoryMappedFiles;
using System.Threading;

//C# Share Memory with C++
//https://docs.microsoft.com/en-us/dotnet/standard/io/memory-mapped-files?redirectedfrom=MSDN

namespace LRCSharpLibrary
{
    public static class LRConfig
    {
        public static string CurrentPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        public static string ConfigPath =
            Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
                         "LRConfig.xml");
        public static LRProfile GetProfile(string Guid,string path)
        {
            try
            {
                return GetProfiles(path).Where(p => p.Guid == Guid).ToArray()[0];
            }
            catch
            {
                return new LRProfile();
            }
        }
        public static LRProfile GetProfile(string Guid)
        {
            return GetProfile(Guid, ConfigPath);
        }
        public static LRProfile[] GetProfiles(string path)
        {
            CheckConfigFile(path);
            try
            {
                var dict = XDocument.Load(path);
                var proc = from i in dict.Descendants("LRConfig").Elements("Profiles").Elements() select i;
                var profiles =
                    proc.Select(p => new LRProfile(p.Attribute("Name").Value,
                                                 p.Attribute("Guid").Value,
                                                 p.Element("Location").Value,
                                                 uint.Parse(p.Element("CodePage").Value),
                                                 uint.Parse(p.Element("LCID").Value),
                                                 p.Element("Font").Value,
                                                 bool.Parse(p.Element("RunAsAdmin").Value),
                                                 bool.Parse(p.Element("HookIME").Value),
                                                 bool.Parse(p.Element("HookLCID").Value)
                                                )).ToArray();
                return profiles;
            }
            catch
            {
                return new LRProfile[0];
            }
        }
        public static LRProfile[] GetProfiles()
        {
            return GetProfiles(ConfigPath);
        }
        public static void CheckConfigFile(string path)
        {
            if (!File.Exists(path))
                BuildDefaultConfigFile();
        }
        public static void CheckConfigFile()
        {
            if (!File.Exists(ConfigPath))
                BuildDefaultConfigFile();
        }
        private static void BuildDefaultConfigFile()
        {
            var defaultProfiles = new[]
                                  {
                                      new LRProfile("Run in Japanese",
                                                    Guid.NewGuid().ToString(),
                                                    "ja-JP",
                                                    932,
                                                    0x0411,
                                                    "None",
                                                    false,
                                                    false,
                                                    true
                                          ),
                                      new LRProfile("Run in Japanese (Admin)",
                                                    Guid.NewGuid().ToString(),
                                                    "ja-JP",
                                                    932,
                                                    0x0411,
                                                    "None",
                                                    true,
                                                    true,
                                                    true
                                          ),
                                      new LRProfile("Run in Taiwan (Admin)",
                                                    Guid.NewGuid().ToString(),
                                                    "zh-TW",
                                                    950,
                                                    0x0404,
                                                    "None",
                                                    true,
                                                    true,
                                                    true
                                          ),
                                      new LRProfile("Run in Korean (Admin)",
                                                    Guid.NewGuid().ToString(),
                                                    "ko-KR",
                                                    949,
                                                    0x0412,
                                                    "Gulim",
                                                    true,
                                                    true,
                                                    true
                                          )
                                  };

            WriteConfig(defaultProfiles);
        }
        public static void WriteConfig(LRProfile[] profiles)
        {
            var baseNode = new XElement("Profiles");

            foreach (var p in profiles)
            {
                baseNode.Add(new XElement("Profile",
                                          new XAttribute("Name", p.Name),
                                          new XAttribute("Guid", p.Guid),
                                          new XElement("Location", p.Location),
                                          new XElement("CodePage",p.CodePage),
                                          new XElement("LCID", p.LCID),
                                          new XElement("Font", p.Font),
                                          new XElement("RunAsAdmin", p.RunAsAdmin),
                                          new XElement("HookIME", p.HookIME),
                                          new XElement("HookLCID", p.HookLCID)
                                 )
                    );
            }

            var tree = new XElement("LRConfig", baseNode);

            try
            {
                tree.Save(ConfigPath);
            }
            catch
            {
                MessageBox.Show("Could not Save Config File.");
            }
        }
    }
}
