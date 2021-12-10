using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LRCSharpLibrary
{
    public class LRProfile
    {
        public string Name;
        public string Guid;
        public uint CodePage;
        public bool RunAsAdmin;
        public LRProfile()
        {
            Name = "Run as Japanese";
            Guid = System.Guid.NewGuid().ToString();
            CodePage = 932;
            RunAsAdmin = false;
        }
        public LRProfile(string name,
                         string guid,
                         uint codepage,
                         bool runAsAdmin)
        {
            Name = name;
            Guid = guid;
            CodePage = codepage;
            RunAsAdmin = runAsAdmin;
        }
    }
}
