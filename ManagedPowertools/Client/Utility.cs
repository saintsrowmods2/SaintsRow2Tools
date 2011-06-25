using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

using EasyHook;

namespace ManagedPowertools.Client
{
    public static class Utility
    {
        public static TDelegate GetProcDelegate<TDelegate>(IntPtr originalFunctionPtr)
        {
            return (TDelegate)(Object)Marshal.GetDelegateForFunctionPointer(originalFunctionPtr, typeof(TDelegate));
        }

        public static IntPtr GetFunctionAddress(string module, int offset)
        {
            IntPtr modulePtr = NativeAPI.GetModuleHandle(module);

            if (modulePtr == IntPtr.Zero)
                throw new DllNotFoundException("The given library is not loaded into the current process.");

            IntPtr methodPtr = (IntPtr)(offset);

            return methodPtr;
        }
    }
}
