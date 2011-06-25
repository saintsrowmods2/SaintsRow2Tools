using System;
using System.Collections.Generic;
using System.Text;

namespace ManagedPowertools.Server
{
    public class PowertoolsInterface : MarshalByRefObject
    {
        public void IsInstalled(int clientPid)
        {
            Console.WriteLine("Installed in process id: {0}", clientPid);
        }

        public void OnHookStart(IntPtr functionAddress, string functionName)
        {
            Console.WriteLine("Hooking function {0} at address 0x{1:X8}", functionName, functionAddress);
        }

        public void OnLuaDebugPrint(string message)
        {
            Console.WriteLine("Lua: {0}", message);
        }

        public void ReportException(Exception ex)
        {
            Console.WriteLine("Exception in hooked process:\n{0}", ex);
        }

        public void Ping()
        {
        }
    }
}
