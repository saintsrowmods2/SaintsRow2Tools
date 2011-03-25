#include "common.h"

#include <windows.h>
#include <tchar.h>
#include <detours.h>

#include "HookedFuncs.h"
#include "Globals.h"

using namespace std;

#define DETOUR(type, lib, func) \
        (type)GetProcAddress(GetModuleHandle(_T(lib)), func)

BOOL WINAPI InstallHooks();
VOID WINAPI ReadConfiguration();
BOOL WINAPI TranslateFunction(PVOID, CONST CHAR*, CONST CHAR*);
BOOL WINAPI HookFunctionByName(FARPROC*, FARPROC, LPCSTR, LPCSTR);
BOOL WINAPI HookFunctionByOffset(FARPROC*, FARPROC, LPTSTR, DWORD);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(ul_reason_for_call);
    UNREFERENCED_PARAMETER(lpReserved);
    
    switch(ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(GetModuleHandle(NULL));
            ReadConfiguration();
            
            if(InstallHooks() == false)
            {
                MessageBox(NULL, L"Failed to install Powertools hooks!",
                    L"Powertools Error", MB_ICONERROR | MB_OK);

                return false;
            }

            //#ifdef _DEBUG
            //    Sleep(5000); // wait for debugger attach
            //#endif
          break;

        case DLL_PROCESS_DETACH:
            // FIXME: Should really detach hooks, although the process is
            // terminating, making this ridiculously unnecessary.

            // Could be useful for post-game activities, like perhaps closing
            // a log file, performing statistical analysis, etc.
          break;
    }

  return true;
}

VOID WINAPI ReadConfiguration()
{
    tstring path;
    TCHAR buffer[MAX_PATH];

    GetCurrentDirectory(
        MAX_PATH,
        buffer);
    
    path = buffer; path += _T("\\powertools.ini");

    GetPrivateProfileString(
        _T("core"),
        _T("osd"),
        NULL,
        buffer,
        MAX_PATH,
        path.c_str());

    _tcsupr_s(buffer, (size_t)MAX_PATH);

    if( _tcsstr(buffer, _T("true")) != NULL )
        hkg_osdEnabled = true;

    GetPrivateProfileString(
        _T("speed"),
        _T("percentage"),
        NULL,
        buffer,
        MAX_PATH,
        path.c_str());

    if( _tcslen(buffer) != 0 )
    {
        LPWSTR unused = NULL;
        hkg_percentage = (float)_tcstod(buffer, &unused); 
        hkg_origPercentage = hkg_percentage;
    }

    return;
}

BOOL WINAPI InstallHooks()
{
    BOOL result = false;
    *hkg_tableSize++;

    result = HookFunctionByName((FARPROC*)&RealDirect3DCreate9,
        (FARPROC)HookedDirect3DCreate9, "d3d9", "Direct3DCreate9");

    if(result == false)
        return false;

    result = HookFunctionByName((FARPROC*)&RealQueryPerformanceCounter,
        (FARPROC)HookedQueryPerformanceCounter, "kernel32", "QueryPerformanceCounter");

    if(result == false)
        return false;

    result = HookFunctionByOffset((FARPROC*)&RealCutsceneBegin,
        (FARPROC)HookedCutsceneBegin, _T("sr2_pc.exe"), 0x2bfb50);

    if(result == false)
        return false;
    
    result = HookFunctionByOffset((FARPROC*)&RealCutsceneEnd,
        (FARPROC)HookedCutsceneEnd, _T("sr2_pc.exe"), 0x2c44c0);

    if(result == false)
        return false;
    
    result = HookFunctionByOffset((FARPROC*)&RealDebugPrint,
        (FARPROC)HookedDebugPrint, _T("sr2_pc.exe"), 0x974ba0);

    if(result == false)
        return false;

    result = HookFunctionByOffset((FARPROC*)&RealLuaPushString,
        (FARPROC)HookedLuaPushString, _T("sr2_pc.exe"), 0x8dcaf0);
    
    if(result == false)
        return false;

    return true;
}

BOOL WINAPI HookFunctionByName(FARPROC* tramp, FARPROC target, LPCSTR lib, LPCSTR fn)
{
    FARPROC fp = GetProcAddress(GetModuleHandleA(lib), fn);

    if(fp == NULL)
        return false;

    PVOID unused = 0;

    DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());	
      DetourAttachEx(&(PVOID&)fp, target, (PDETOUR_TRAMPOLINE*)tramp, &unused, &unused);
    LONG result = DetourTransactionCommit();
    
    if(result > 0)
        return false;

    return true;
}

BOOL WINAPI HookFunctionByOffset(FARPROC* tramp, FARPROC target, LPTSTR lib, DWORD offset)
{
    HMODULE mod = GetModuleHandle(lib);

    if(mod == NULL)
        return false;

    FARPROC* fp = (FARPROC*)((DWORD)mod + offset);

    PVOID unused = 0;

    DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());	
      DetourAttachEx(&(PVOID&)fp, target, (PDETOUR_TRAMPOLINE*)tramp, &unused, &unused);
    LONG result = DetourTransactionCommit();
    
    if(result > 0)
        return false;

    return true;
}