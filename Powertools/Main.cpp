#include "common.h"

#include <windows.h>
#include <tchar.h>
#include <detours.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#include "HookedFuncs.h"
#include "Globals.h"

using namespace std;

#define DETOUR(type, lib, func) \
        (type)GetProcAddress(GetModuleHandle(_T(lib)), func)

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
#define PRODUCT_PROFESSIONAL	0x00000030
#define VER_SUITE_WH_SERVER	0x00008000

BOOL WINAPI ProcessorDetect();
BOOL WINAPI LogOSDetect();
VOID WINAPI LogSystemInfo();

BOOL WINAPI InstallHooks();
VOID WINAPI ReadConfiguration();
VOID WINAPI RedirectIOToConsole();
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

			if (hkg_consoleBuffer > 0)
			{
				RedirectIOToConsole();
			}

			printf("Saints Row 2 Powertools\n");
			printf("\n");

			hkg_logFile = CreateFileW(TEXT("Powertools.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			WriteToLog(_T("Powertools"), _T("Opened log file.\n"));

			LogSystemInfo();

			WriteToLog(_T("Powertools"), _T("Configuration state:\n"));
			WriteToLog(_T("Powertools"), _T("[core]\n"));
			WriteToLog(_T("Powertools"), _T("consolebuffer = %d\n"), hkg_consoleBuffer);
			WriteToLog(_T("Powertools"), _T("loglua = %s\n"), hkg_logLua ? _T("true") : _T("false"));
			WriteToLog(_T("Powertools"), _T("osd = %s\n"), hkg_osdEnabled ? _T("true") : _T("false"));
			WriteToLog(_T("Powertools"), _T("[speed]\n"));
			WriteToLog(_T("Powertools"), _T("freqtarget = %d\n"), hkg_freqTarget);
			WriteToLog(_T("Powertools"), _T("timescale = %f\n"), hkg_timescale);

			LARGE_INTEGER currentFreq;
			QueryPerformanceFrequency(&currentFreq);
			WriteToLog(_T("Powertools"), _T("QueryPerformanceFrequency: %d\n"), currentFreq);
			WriteToLog(_T("Powertools"), _T("Scaled QueryPerformanceFrequency: %d\n"), (__int64)(currentFreq.QuadPart * hkg_timescale));

			WriteToLog(_T("Powertools"), _T("Installing Powertools hooks.\n"));
            if(InstallHooks() == false)
            {
				WriteToLog(_T("Powertools"), _T("Failed to install hooks.\n"));
				WriteToLog(_T("Powertools"), _T("Upload your Powertools.log file when you report this!\n"));
                MessageBox(NULL, L"Failed to install Powertools hooks!\nUpload the Powertools.log file in your Saints Row 2 directory when you ask for help.",
                    L"Powertools Error", MB_ICONERROR | MB_OK);

                return false;
            }
			else
			{
				WriteToLog(_T("Powertools"), _T("Finished installing hooks.\n"));
			}
          break;

        case DLL_PROCESS_DETACH:
            // FIXME: Should really detach hooks, although the process is
            // terminating, making this ridiculously unnecessary.

            // Could be useful for post-game activities, like perhaps closing
            // a log file, performing statistical analysis, etc.

			if (hkg_logFile != NULL)
			{
				WriteToLog(_T("Powertools"), _T("Closing log file.\n"));
				CloseHandle(hkg_logFile);
			}
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

	GetPrivateProfileString(_T("core"), _T("consolebuffer"), NULL, buffer, MAX_PATH, path.c_str());

	if (_tcslen(buffer) != 0)
	{
		LPWSTR unused = NULL;
		hkg_consoleBuffer = (SHORT)_tcstol(buffer, &unused, 10);
	}

	GetPrivateProfileString(_T("core"), _T("loglua"), NULL, buffer, MAX_PATH, path.c_str());

	if (_tcsstr(buffer, _T("true")) != NULL)
		hkg_logLua = true;

    GetPrivateProfileString(
        _T("core"),
        _T("osd"),
        NULL,
        buffer,
        MAX_PATH,
        path.c_str());

    //_tcsupr_s(buffer, (size_t)MAX_PATH);

    if( _tcsstr(buffer, _T("true")) != NULL )
        hkg_osdEnabled = true;

	GetPrivateProfileString(_T("speed"), _T("freqtarget"), NULL, buffer, MAX_PATH, path.c_str());
	if (_tcslen(buffer) != 0)
	{
		LPWSTR unused = NULL;
		hkg_freqTarget = _tcstol(buffer, &unused, 10);
	}

    GetPrivateProfileString(
        _T("speed"),
        _T("timescale"),
        NULL,
        buffer,
        MAX_PATH,
        path.c_str());

	if (_tcsstr(buffer, _T("auto")) != NULL)
	{
		LARGE_INTEGER currentFreq;
		QueryPerformanceFrequency(&currentFreq);
		hkg_timescale = (float)hkg_freqTarget / (float)currentFreq.QuadPart;
		hkg_origTimescale = hkg_timescale;
	}
	else if ( _tcslen(buffer) != 0 )
    {
        LPWSTR unused = NULL;
        hkg_timescale = (float)_tcstod(buffer, &unused); 
        hkg_origTimescale = hkg_timescale;
    }

    return;
}

VOID WINAPI RedirectIOToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	// allocate a console for this app
	AllocConsole();
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = hkg_consoleBuffer;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
		*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	// point to console as well
	ios::sync_with_stdio();
}

VOID WINAPI WriteToLog(TCHAR* source, TCHAR* string, ...)
{
	DWORD charsRequired;
	TCHAR* message = NULL;
	
	TCHAR* fullMessage = NULL;

	va_list argptr;
	va_start(argptr, string);

	charsRequired = sizeof(TCHAR) * (_vsctprintf(string, argptr) +1);
	message = (TCHAR*)malloc(charsRequired);
	_vstprintf(message, string, argptr);

	va_end(argptr);

	charsRequired = sizeof(TCHAR) * (_sctprintf(_T("[%s] %s"), source, message) + 1);
	fullMessage = (TCHAR*)malloc(charsRequired);
	_stprintf(fullMessage, _T("[%s] %s"), source, message);

	if (hkg_consoleBuffer > 0)
		_tprintf(fullMessage);

	DWORD bytesWritten = 0;

	WriteFile(hkg_logFile, fullMessage, charsRequired - sizeof(TCHAR), &bytesWritten, NULL);
	free(fullMessage);
	free(message);
}

BOOL WINAPI InstallHooks()
{
    BOOL result = false;
    *hkg_tableSize++;

	WriteToLog(_T("Powertools"), _T("Installing Direct3DCreate9 hook.\n"));
    result = HookFunctionByName((FARPROC*)&RealDirect3DCreate9,
        (FARPROC)HookedDirect3DCreate9, "d3d9", "Direct3DCreate9");

    if(result == false)
        return false;

	WriteToLog(_T("Powertools"), _T("Installing QueryPerformanceCounter hook.\n"));
    result = HookFunctionByName((FARPROC*)&RealQueryPerformanceCounter,
        (FARPROC)HookedQueryPerformanceCounter, "kernel32", "QueryPerformanceCounter");

    if(result == false)
        return false;

	WriteToLog(_T("Powertools"), _T("Installing CutsceneBegin hook.\n"));
    result = HookFunctionByOffset((FARPROC*)&RealCutsceneBegin,
        (FARPROC)HookedCutsceneBegin, _T("sr2_pc.exe"), 0x2bfb50);

    if(result == false)
        return false;
    
	WriteToLog(_T("Powertools"), _T("Installing CutsceneEnd hook.\n"));
    result = HookFunctionByOffset((FARPROC*)&RealCutsceneEnd,
        (FARPROC)HookedCutsceneEnd, _T("sr2_pc.exe"), 0x2c44c0);

    if(result == false)
        return false;
    
	WriteToLog(_T("Powertools"), _T("Installing DebugPrint hook.\n"));
    result = HookFunctionByOffset((FARPROC*)&RealDebugPrint,
        (FARPROC)HookedDebugPrint, _T("sr2_pc.exe"), 0x974ba0);

    if(result == false)
        return false;

	WriteToLog(_T("Powertools"), _T("Installing LuaPushString hook.\n"));
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

VOID WINAPI LogSystemInfo()
{
	WriteToLog(_T("SysInfo"), _T("Detecting Processor...\n"));
	if (!ProcessorDetect())
		WriteToLog(_T("SysInfo"), _T("Failed to detect processor.\n"));

	WriteToLog(_T("SysInfo"), _T("Detecting Operating System...\n"));
	if (!LogOSDetect())
		WriteToLog(_T("SysInfo"), _T("Failed to detect OS.\n"));
}

BOOL WINAPI ProcessorDetect()
{
	char CPUString[0x20];
	char CPUBrandString[0x20];
	int CPUInfo[4] = {-1};
	unsigned int nIds = 0;
	__cpuid(CPUInfo, 0);

	nIds = CPUInfo[0];
    memset(CPUString, 0, sizeof(CPUString));
    *((int*)CPUString) = CPUInfo[1];
    *((int*)(CPUString+4)) = CPUInfo[3];
    *((int*)(CPUString+8)) = CPUInfo[2];

	if (sizeof(TCHAR) == sizeof(char))
		WriteToLog(_T("SysInfo"), _T("CPU manufacturer string: %s\n"), (TCHAR*)CPUString);
	else
	{
		TCHAR* CPUStringW = NULL;
		DWORD CPUStringLen = MultiByteToWideChar(CP_ACP, 0, CPUString, -1, NULL, 0);
		CPUStringW = (TCHAR*)malloc(CPUStringLen * sizeof(TCHAR));
		MultiByteToWideChar(CP_ACP, 0, CPUString, -1, (LPWSTR)CPUStringW, CPUStringLen);
		WriteToLog(_T("SysInfo"), _T("CPU manufacturer string: %s\n"), (TCHAR*)CPUStringW);
		free(CPUStringW);
	}

	__cpuid(CPUInfo, 0x80000000);
	if (CPUInfo[0] >= 0x80000004)
	{
		__cpuid(CPUInfo, 0x80000002);
		memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		__cpuid(CPUInfo, 0x80000003);
		memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		__cpuid(CPUInfo, 0x80000004);
		memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));

		if (sizeof(TCHAR) == sizeof(char))
			WriteToLog(_T("SysInfo"), _T("CPU Brand string: %s\n"), (TCHAR*)CPUBrandString);
		else
		{
			TCHAR* CPUStringW = NULL;
			DWORD CPUStringLen = MultiByteToWideChar(CP_ACP, 0, CPUBrandString, -1, NULL, 0);
			CPUStringW = (TCHAR*)malloc(CPUStringLen * sizeof(TCHAR));
			MultiByteToWideChar(CP_ACP, 0, CPUBrandString, -1, (LPWSTR)CPUStringW, CPUStringLen);
			WriteToLog(_T("SysInfo"), _T("CPU Brand string: %s\n"), (TCHAR*)CPUStringW);
			free(CPUStringW);
		}

		return true;
	}
	else
	{
		WriteToLog(_T("SysInfo"), _T("Cannot get CPU Brand String."));
		return false;
	}
}

BOOL WINAPI LogOSDetect()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOSVersionInfoEx;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOSVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);

	if(!bOSVersionInfoEx)
	{
		return false;
	}

	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pGNSI)
		pGNSI(&si);
	else
		GetSystemInfo(&si);

	WriteToLog(_T("SysInfo"), _T("OS information: PlatformId: %d, Version: %d.%d, Build: %d, Service Pack: %d.%d (%s), Suite Mask: 0x%08X, Processor Architecture: %s\n"),
		osvi.dwPlatformId, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.wServicePackMajor, osvi.wServicePackMinor, osvi.szCSDVersion, osvi.wSuiteMask,
		(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ? _T("Itanium") : (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? _T("x86_64") : _T("x86"))));

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (osvi.dwMajorVersion == 5)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					WriteToLog(_T("SysInfo"), _T("OS is Windows 2000 Professional.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					WriteToLog(_T("SysInfo"), _T("OS is Windows 2000 Datacenter Server.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					WriteToLog(_T("SysInfo"), _T("OS is Windows 2000 Enterprise Server.\n"));
				else
					WriteToLog(_T("SysInfo"), _T("OS is Windows 2000 Server.\n"));
			}
			else if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					WriteToLog(_T("SysInfo"), _T("OS is Windows XP Home.\n"));
				else
					WriteToLog(_T("SysInfo"), _T("OS is Windows XP Professional.\n"));
			}
			else if (osvi.dwMinorVersion == 2)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					WriteToLog(_T("SysInfo"), _T("OS is Windows XP Professional x64 Edition.\n"));
				else if (GetSystemMetrics(SM_SERVERR2))
					WriteToLog(_T("SysInfo"), _T("OS is Windows Server 2003 R2.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
					WriteToLog(_T("SysInfo"), _T("OS is Windows Storage Server 2003.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
					WriteToLog(_T("SysInfo"), _T("OS is Windows Home Server.\n"));
				else
					WriteToLog(_T("SysInfo"), _T("OS is Windows Server 2003.\n"));
			}
			else
			{
				WriteToLog(_T("SysInfo"), _T("Unknown minor version.\n"));
				return false;
			}
		}
		else if (osvi.dwMajorVersion == 6)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					WriteToLog(_T("SysInfo"), _T("OS is Windows Vista.\n"));
				else
					WriteToLog(_T("SysInfo"), _T("OS is Windows Server 2008.\n"));
			}
			else if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					WriteToLog(_T("SysInfo"), _T("OS is Windows 7.\n"));
				else
					WriteToLog(_T("SysInfo"), _T("OS is Windows Server 2008 R2.\n"));
			}
			else
			{
				WriteToLog(_T("SysInfo"), _T("Unknown minor version.\n"));
				return false;
			}
		}
		else if (osvi.dwMajorVersion < 5)
		{
			WriteToLog(_T("SysInfo"), _T("Unknown (old?) Windows version.\n"));
			return false;
		}
		else if (osvi.dwMajorVersion > 6)
		{
			WriteToLog(_T("SysInfo"), _T("Unknown (future?) Windows version.\n"));
			return false;
		}
	}
	else
	{
		WriteToLog(_T("SysInfo"), _T("OS platform is unknown.\n"), osvi.dwPlatformId);
		return false;
	}

	return true;
}