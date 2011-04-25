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

BOOL WINAPI InstallHooks();
VOID WINAPI ReadConfiguration();
VOID WINAPI RedirectIOToConsole();
VOID WINAPI WriteToLog(TCHAR*, TCHAR*, ...);
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

			WriteToLog(_T("Powertools"), _T("Configuration state:\n"));
			WriteToLog(_T("Powertools"), _T("[core]\n"));
			WriteToLog(_T("Powertools"), _T("consolebuffer = %d\n"), hkg_consoleBuffer);
			WriteToLog(_T("Powertools"), _T("loglua = %s\n"), hkg_logLua ? _T("true") : _T("false"));
			WriteToLog(_T("Powertools"), _T("osd = %s\n"), hkg_osdEnabled ? _T("true") : _T("false"));
			WriteToLog(_T("Powertools"), _T("[speed]\n"));
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

	GetPrivateProfileString(_T("core"), _T("log"), NULL, buffer, MAX_PATH, path.c_str());

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

    GetPrivateProfileString(
        _T("speed"),
        _T("timescale"),
        NULL,
        buffer,
        MAX_PATH,
        path.c_str());

    if( _tcslen(buffer) != 0 )
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