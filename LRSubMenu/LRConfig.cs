﻿using System;
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
        public static LRProfile GetProfile(string name)
        {
            try
            {
                return GetProfiles().Where(p => p.Name == name).ToArray()[0];
            }
            catch
            {
                return new LRProfile();
            }
        }
        public static LRProfile[] GetProfiles()
        {
            try
            {
                var dict = XDocument.Load(ConfigPath);
                var proc = from i in dict.Descendants("LRConfig").Elements("Profiles").Elements() select i;
                var profiles =
                    proc.Select(p => new LRProfile(p.Attribute("Name").Value,
                                                 p.Attribute("Guid").Value,
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
                                                    932,
                                                    false
                                          ),
                                      new LRProfile("Run in Japanese (Admin)",
                                                    Guid.NewGuid().ToString(),
                                                    932,
                                                    true
                                          )
                                  };

            WriteConfig(defaultProfiles);
        }
        private static void WriteConfig(LRProfile[] profiles)
        {
            var baseNode = new XElement("Profiles");

            foreach (var p in profiles)
            {
                baseNode.Add(new XElement("Profile",
                                          new XAttribute("Name", p.Name),
                                          new XAttribute("Guid", p.Guid),
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