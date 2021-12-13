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
        public string Location;
        public uint CodePage;
        public bool RunAsAdmin;

        public override string ToString()
        {
            return Name;
        }
        public LRProfile()
        {
            Name = "Run as Japanese";
            Guid = System.Guid.NewGuid().ToString();
            Location = "ja-JP";
            CodePage = 932;
            RunAsAdmin = false;
        }
        public LRProfile(LRProfile p)
        {
            this.Name = p.Name;
            this.Guid = p.Guid;
            this.Location = p.Location;
            this.CodePage = p.CodePage;
            this.RunAsAdmin = p.RunAsAdmin;
        }
        public LRProfile(string Name,
                         string Guid,
                         string Location,
                         uint CodePage,
                         bool RunAsAdmin)
        {
            this.Name = Name;
            this.Guid = Guid;
            this.Location = Location;
            this.CodePage = CodePage;
            this.RunAsAdmin = RunAsAdmin;
        }
    }
}
