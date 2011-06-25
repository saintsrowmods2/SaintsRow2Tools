using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

using EasyHook;
using ManagedPowertools.Server;

namespace PowertoolsInjector
{
    public static class Program
    {
        private static string ChannelName = null;

        public static void Main(string[] args)
        {
            try
            {
                string directory = Utility.GetAssemblyDirectory();
                string injectorPath = Path.Combine(Utility.GetAssemblyDirectory(), "PowertoolsInjector.exe");
                string managedPowertoolsPath = Path.Combine(Utility.GetAssemblyDirectory(), "ManagedPowertools.dll");
                Config.Register("Saints Row 2 Managed Powertools", injectorPath, managedPowertoolsPath);
                RemoteHooking.IpcCreateServer<PowertoolsInterface>(ref ChannelName, System.Runtime.Remoting.WellKnownObjectMode.SingleCall);

                int processId = -1;

                RemoteHooking.CreateAndInject("SR2_pc.exe", "", 0, "ManagedPowertools.dll", "ManagedPowertools.dll", out processId, ChannelName);

                Console.ReadLine();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Failed to connect to target.\nException:\n{0}", ex);
            }
        }
    }
}
