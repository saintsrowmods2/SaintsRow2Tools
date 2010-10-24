#pragma once

#include <windows.h>
#include <winsock.h>
#include <x3daudio.h>
#include <d3d9.h>

#include <vector>
using namespace std;

#include "Globals.h"

extern "C"
{
    #include <lualib.h>
}

#define IN_THREAD(type) \
    (threadFlags[GetCurrentThreadId()] & type == type)

#define SET_THREAD_FLAG(type) \
    threadFlags[GetCurrentThreadId()] |= type

#define THREADFLAG_NETWORK		(1 << 0)
#define THREADFLAG_AUDIO		(1 << 1)


//
// Direct3DCreate9
//
IDirect3D9* WINAPI HookedDirect3DCreate9(UINT SDKVersion);
typedef IDirect3D9* (WINAPI * fnDirect3DCreate9)(UINT SDKVersion);

extern fnDirect3DCreate9 RealDirect3DCreate9;

//
// IDirect3D9::CreateDevice
//
typedef HRESULT (WINAPI* fnCreateDevice)(IDirect3D9 FAR *This, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
    IDirect3DDevice9** ppReturnedDeviceInterface);

HRESULT WINAPI HookedCreateDevice(IDirect3D9 FAR *This, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
    IDirect3DDevice9** ppReturnedDeviceInterface);

extern fnCreateDevice RealCreateDevice;

//
// IDirect3DDevice9::EndScene
//
typedef HRESULT (WINAPI* fnEndScene)(IDirect3DDevice9 FAR *This);
HRESULT WINAPI HookedEndScene(IDirect3DDevice9 FAR *This);

extern fnEndScene RealEndScene;

//
// QueryPerformanceCounter
//
typedef BOOL (WINAPI * fnQueryPerformanceCounter)(LARGE_INTEGER *lpPerformanceCount);
BOOL WINAPI HookedQueryPerformanceCounter(LARGE_INTEGER*);

extern fnQueryPerformanceCounter RealQueryPerformanceCounter;

//
// Saints Row 2 -> CutsceneBegin (fuzzy) function
//
typedef INT (WINAPIV * fnCutsceneBegin)();
VOID WINAPIV HookedCutsceneBegin();

extern fnCutsceneBegin RealCutsceneBegin;

//
// Saints Row 2 -> CutsceneEnd (fuzzy) function
//
typedef INT (WINAPIV * fnCutsceneEnd)();
VOID WINAPIV HookedCutsceneEnd();

extern fnCutsceneEnd RealCutsceneEnd;

//
// Saints Row 2 -> lua function: mission_end_success
//
typedef INT (WINAPIV * fnMissionEndSuccess)(lua_State *lua);
INT WINAPIV HookedMissionEndSuccess(lua_State *lua);

extern fnMissionEndSuccess RealMissionEndSuccess;

//
// Saints Row 2 -> CutsceneExists function
//
typedef BOOL (WINAPIV * fnCutsceneExists)(CHAR* cutscene);
BOOL WINAPIV HookedCutsceneExists(CHAR* cutscene);


//
// Saints Row 2 -> DebugPrint function (lua)
//
typedef INT (WINAPIV * fnDebugPrint)(lua_State *lua);
INT WINAPIV HookedDebugPrint(lua_State *lua);

extern fnDebugPrint RealDebugPrint;

typedef VOID (__cdecl * fnLuaFile)(lua_State *lua);

typedef VOID (__cdecl * fnLuaPushString)(LPCSTR);
VOID WINAPIV HookedLuaPushString(LPCSTR);
extern fnLuaPushString RealLuaPushString;

typedef DWORD (__cdecl * fnLuaRegister)(DWORD*, DWORD*, DWORD*, DWORD*);
typedef DWORD (__cdecl * fnLuaPushCFunction)(DWORD, DWORD);