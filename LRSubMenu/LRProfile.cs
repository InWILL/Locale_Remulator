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
        public uint LCID;
        public string Font;
        public bool RunAsAdmin;
        public bool HookIME;
        public bool HookLCID;

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
            LCID = 0x0411;
            Font = "None";
            RunAsAdmin = false;
            HookIME = false;
            HookLCID = true;
        }
        public LRProfile(LRProfile p)
        {
            this.Name = p.Name;
            this.Guid = p.Guid;
            this.Location = p.Location;
            this.CodePage = p.CodePage;
            this.LCID = p.LCID;
            this.Font = p.Font;
            this.RunAsAdmin = p.RunAsAdmin;
            this.HookIME = p.HookIME;
            this.HookLCID = p.HookLCID;
        }
        public LRProfile(string Name,
                         string Guid,
                         string Location,
                         uint CodePage,
                         uint LCID,
                         string Font,
                         bool RunAsAdmin,
                         bool HookIME,
                         bool HookLCID)
        {
            this.Name = Name;
            this.Guid = Guid;
            this.Location = Location;
            this.CodePage = CodePage;
            this.LCID = LCID;
            this.Font = Font;
            this.RunAsAdmin = RunAsAdmin;
            this.HookIME = HookIME;
            this.HookLCID = HookLCID;
        }
    }
}
