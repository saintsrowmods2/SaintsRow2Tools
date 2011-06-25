using System;
using System.Collections.Generic;
using System.Text;

using EasyHook;

namespace ManagedPowertools.Client
{
    public class EasyHookEntry : EasyHook.IEntryPoint
    {
        ManagedPowertools.Server.PowertoolsInterface Interface;


        public EasyHookEntry(RemoteHooking.IContext context, string channelName)
        {
            Interface = RemoteHooking.IpcConnectClient<ManagedPowertools.Server.PowertoolsInterface>(channelName);
        }

        public void Run(RemoteHooking.IContext context, string channelName)
        {
            try
            {
                LuaHooks.RegisterHooks(Interface);
            }
            catch (Exception ex)
            {
                Interface.ReportException(ex);

                return;
            }

            Interface.IsInstalled(RemoteHooking.GetCurrentProcessId());

            RemoteHooking.WakeUpProcess();

            try
            {
                while (true)
                {
                    System.Threading.Thread.Sleep(500);

                    Interface.Ping();
                }
            }
            catch
            {
            }
        }
    }
}
