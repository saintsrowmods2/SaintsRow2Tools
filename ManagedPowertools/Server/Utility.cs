using System;
using System.Collections.Generic;
using System.Text;

namespace ManagedPowertools.Server
{
    public static class Utility
    {
        public static string GetAssemblyPath()
        {
            return System.Reflection.Assembly.GetCallingAssembly().Location;
        }

        public static string GetAssemblyDirectory()
        {
            return System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetCallingAssembly().Location);
        }
    }
}
