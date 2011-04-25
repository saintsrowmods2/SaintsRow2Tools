#include "D3DWrapper.h"
#include "HookedFuncs.h"
#include "Globals.h"

CDirect3DHelper D3DHelper; // TODO: Get rid of this stupid class. I hate it.
using namespace std;

fnDirect3DCreate9 RealDirect3DCreate9 = NULL;
fnCreateDevice RealCreateDevice = NULL;
fnEndScene RealEndScene = NULL;
fnQueryPerformanceCounter RealQueryPerformanceCounter = NULL;
fnCutsceneBegin RealCutsceneBegin = NULL;
fnCutsceneEnd RealCutsceneEnd = NULL;
fnMissionEndSuccess RealMissionEndSuccess = NULL;
fnCutsceneExists RealCutsceneExists = NULL;
fnDebugPrint RealDebugPrint = NULL;
fnLuaPushString RealLuaPushString = NULL;

fnLuaFile LuaFile = (fnLuaFile)0x00D74B40;
fnLuaRegister LuaRegister = (fnLuaRegister)0x00CD68D0;
fnLuaPushCFunction LuaPushCFunction = (fnLuaPushCFunction)0x00CDCD30;

static volatile INT64 g_lastFakedTime = 0;
static volatile INT64 g_lastRealTime = 0;

static volatile INT64 g_lastFakedTickTime = 0;
static volatile INT64 g_lastRealTickTime = 0;

static CRITICAL_SECTION cs;

vector<char*> g_debugLines;
static lua_State* hkg_luaState = NULL;


HRESULT WINAPI HookedCreateDevice(IDirect3D9 FAR *This, UINT Adapter,
                                              D3DDEVTYPE DeviceType, HWND hFocusWindow,
                                              DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
                                              IDirect3DDevice9** ppReturnedDeviceInterface)
{
    DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      DetourDetach(&(PVOID&)RealCreateDevice, HookedCreateDevice);
    DetourTransactionCommit();

    HRESULT ret = RealCreateDevice(This, Adapter, DeviceType, hFocusWindow, BehaviorFlags,
        pPresentationParameters, ppReturnedDeviceInterface);

    if(FAILED(ret))
    {
        MessageBox(NULL, _T("CreateDevice failed."), _T("Error."), MB_OK | MB_ICONERROR);
    }

    D3DHelper.SetDevicePtr(*ppReturnedDeviceInterface);

    RealEndScene = D3DHelper.GetDevicePtr()->lpVtbl->EndScene;

    DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      DetourAttach(&(PVOID&)RealEndScene, HookedEndScene);
    DetourTransactionCommit();

    return ret;
}

IDirect3D9* WINAPI HookedDirect3DCreate9(UINT SDKVersion)
{
    D3DHelper.SetInterfacePtr(RealDirect3DCreate9(SDKVersion));

    if(D3DHelper.GetInterfacePtr() == NULL)
    {
        MessageBox(NULL, _T("Direct3DCreate9 failed."), _T("Error."), MB_OK | MB_ICONERROR);
    }

    RealCreateDevice = D3DHelper.GetInterfacePtr()->lpVtbl->CreateDevice; // vtable entry #16

    DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      DetourAttach(&(PVOID&)RealCreateDevice, HookedCreateDevice);
    DetourTransactionCommit();	

    return D3DHelper.GetInterfacePtr();
}

HRESULT WINAPI HookedEndScene(IDirect3DDevice9 FAR *This)
{
    if( (GetKeyState(VK_SCROLL) & 0x0001) && (hkg_osdEnabled == TRUE) )
    {
        D3DHelper.DrawDebugText();

        LPTSTR playerPos = new TCHAR[MAX_PATH];
        _stprintf_s(playerPos, MAX_PATH, _T("Player position: (%0.6f, %0.6f, %0.6f)"),
            hkg_playerPosition[0], hkg_playerPosition[1], hkg_playerPosition[2]);

        D3DHelper.DrawTextW(
            L"Player position: (%0.6f, %0.6f, %0.6f)",
            1, // line number 1
            hkg_playerPosition[0],
            hkg_playerPosition[1],
            hkg_playerPosition[2]);

        if(hkg_isCutscenePlaying)
        {
            LPSTR cutsceneInfo = new CHAR[MAX_PATH];
            sprintf_s(cutsceneInfo, MAX_PATH, "[!] Cutscene playing, speed @ normal.\0");

            D3DHelper.DrawTextA(
                cutsceneInfo,
                2, // line number 2
                hkg_cutsceneName);

            delete[] cutsceneInfo;
        }
    }

    return RealEndScene(This);
}

VOID WINAPIV HookedCutsceneBegin()
{
    hkg_timescale = 1.0f;
    hkg_isCutscenePlaying = true;

    RealCutsceneBegin();
}

VOID WINAPIV HookedCutsceneEnd()
{
    hkg_timescale = hkg_origTimescale;
    hkg_isCutscenePlaying = false;

    RealCutsceneEnd();
}

BOOL WINAPIV HookedCutsceneExists(CHAR* cutscene)
{
    if(hkg_cutsceneName)
    {
        free(hkg_cutsceneName);
        hkg_cutsceneName = NULL;
    }

    hkg_cutsceneName = _strdup(cutscene);
    return RealCutsceneExists(cutscene);
}

BOOL WINAPI HookedQueryPerformanceCounter(LARGE_INTEGER* count)
{
    __int64 currentTime = 0;

    LARGE_INTEGER currentFreq;

    RealQueryPerformanceCounter((PLARGE_INTEGER)&currentTime);
    QueryPerformanceFrequency(&currentFreq);

    if(g_lastFakedTime == 0)
        g_lastRealTime = g_lastFakedTime = currentTime;
    else
    {
        // step 1: obtain time difference and get a portion of that for fake
        // timer use
        __int64 realDiff = (currentTime - g_lastRealTime);
        __int64 fakeDiff = (__int64)(realDiff * hkg_timescale);

        // step 2: increment fake timers

        // We can't atomically update 64-bit values on 32-bit platforms
        // without InterlockedExchange64. Unfortunately, it's not-existant
        // on Windows XP platforms. Too bad.
        InterlockedExchange64(&g_lastFakedTime, g_lastFakedTime + fakeDiff);
        InterlockedExchange64(&g_lastRealTime, g_lastRealTime + realDiff);
    }

    if(g_lastFakedTime < 0)
        (*count).QuadPart = currentTime;
    else
        (*count).QuadPart = g_lastFakedTime;

    return true;
}

INT WINAPIV HookedDebugPrint(lua_State *lua)
{
    if(hkg_luaState == NULL)
    {
        hkg_luaState = lua;

        // Just some sample code that loads a custom file upon game start

        // OutputDebugStringA("Loading lua file...\r\n");
        // RealLuaPushString("mod_init.lua");
        // LuaFile(hkg_luaState);
    }
    
    const char *msg = NULL;

    if(lua_isstring(lua, 1) == TRUE)
    {
        // determine if we need to ignore the "vint" debug_print argument
        msg = lua_tostring(lua, 1);

        if(_strcmpi(msg, "vint") == 0)
        {
            if(lua_isstring(lua, 2) == TRUE)
                msg = lua_tostring(lua, 2);
        }

        if(g_debugLines.size() == 10)
        {
            free(g_debugLines.front());
            g_debugLines.erase(g_debugLines.begin());
        }

		if (hkg_logLua)
		{
			if (sizeof(TCHAR) == sizeof(char))
			{
				WriteToLog(_T("Lua"), (TCHAR*)msg);
			}
			else
			{
				TCHAR* msgW = NULL;
				DWORD msgLen = MultiByteToWideChar(CP_ACP, 0, msg, -1, NULL, 0);
				msgW = (TCHAR*)malloc(msgLen * sizeof(TCHAR));
				MultiByteToWideChar(CP_ACP, 0, msg, -1, (LPWSTR)msgW, msgLen);
				WriteToLog(_T("Lua"), (TCHAR*)msgW);
			}
		}

        g_debugLines.push_back(_strdup(msg));
    }

    return 0;
}

VOID WINAPIV HookedLuaPushString(LPCSTR str)
{
    RealLuaPushString(str);
    return;
}