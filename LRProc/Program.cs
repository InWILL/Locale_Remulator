using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using LRCSharpLibrary;

namespace LRProc
{
    static class LRCPPInject
    {
        [DllImport("LRInject.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int LRInject(StringBuilder filepath, StringBuilder dllpath,uint CodePage);
    }
    class Program
    {
        static void Main(string[] args)
        {
            LRProfile profile = LRConfig.GetProfile(args[2]);
            StringBuilder filepath = new StringBuilder(args[0]);
            StringBuilder dllpath = new StringBuilder(args[1]);
            LRCPPInject.LRInject(filepath, dllpath, profile.CodePage);
        }
    }
}
