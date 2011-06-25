using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

using EasyHook;

namespace ManagedPowertools.Client
{
    public class LuaHooks
    {
        static int LuaDebugPrintOffset = 0x974ba0;

        public static void RegisterHooks(ManagedPowertools.Server.PowertoolsInterface powertoolsInterface)
        {
            LuaHooks luaHooks = new LuaHooks(powertoolsInterface);

            IntPtr luaDebugPrintPtr = Utility.GetFunctionAddress("sr2_pc.exe", LuaDebugPrintOffset);
            powertoolsInterface.OnHookStart(luaDebugPrintPtr, "Lua debug_print");
            luaHooks.LuaDebugPrintReal = Utility.GetProcDelegate<LuaDebugPrintDelegate>(luaDebugPrintPtr);
            luaHooks.LuaDebugPrintHook = LocalHook.Create(luaDebugPrintPtr, new LuaDebugPrintDelegate(luaHooks.DebugPrint_Hooked), luaHooks);
        }


        LocalHook LuaDebugPrintHook;
        LuaDebugPrintDelegate LuaDebugPrintReal;

        ManagedPowertools.Server.PowertoolsInterface PowertoolsInterface;

        private LuaHooks(ManagedPowertools.Server.PowertoolsInterface powertoolsInterface)
        {
            PowertoolsInterface = powertoolsInterface;
        }

        private int DebugPrint_Hooked(IntPtr luaStatePtr)
        {
            PowertoolsInterface.OnLuaDebugPrint("not parsed yet");

            return LuaDebugPrintReal(luaStatePtr);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet=CharSet.Ansi, SetLastError=true, ThrowOnUnmappableChar=false)]
        delegate int LuaDebugPrintDelegate(IntPtr luaStatePtr);
    }
}
