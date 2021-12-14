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
        public static string ConfigPath =
            Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
                         "LRConfig.xml");
        public static LRProfile GetProfile(uint CodePage)
        {
            try
            {
                return GetProfiles().Where(p => p.CodePage == CodePage).ToArray()[0];
            }
            catch
            {
                return new LRProfile();
            }
        }
        public static LRProfile GetProfile(string Guid)
        {
            try
            {
                return GetProfiles().Where(p => p.Guid == Guid).ToArray()[0];
            }
            catch
            {
                return new LRProfile();
            }
        }
        public static LRProfile[] GetProfiles()
        {
            CheckConfigFile();
            try
            {
                var dict = XDocument.Load(ConfigPath);
                var proc = from i in dict.Descendants("LRConfig").Elements("Profiles").Elements() select i;
                var profiles =
                    proc.Select(p => new LRProfile(p.Attribute("Name").Value,
                                                 p.Attribute("Guid").Value,
                                                 p.Element("Location").Value,
                                                 uint.Parse(p.Element("CodePage").Value),
                                                 bool.Parse(p.Element("RunAsAdmin").Value)
                                                )).ToArray();
                return profiles;
            }
            catch
            {
                return new LRProfile[0];
            }
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
                                                    false
                                          ),
                                      new LRProfile("Run in Japanese (Admin)",
                                                    Guid.NewGuid().ToString(),
                                                    "ja-JP",
                                                    932,
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
                                          new XElement("RunAsAdmin", p.RunAsAdmin)
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
