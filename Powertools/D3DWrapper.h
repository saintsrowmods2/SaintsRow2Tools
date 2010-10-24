#pragma once

#include "common.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <detours.h>

#if DETOURS_VERSION < 20100
#error Microsoft Detours 2.1 Express (or later) is required for compilation.
#endif

#define FONT_SIZE 14
#define MIN_TEXT_BUFFER 1
#define MAX_TEXT_BUFFER 512

#ifdef UNICODE
#define DrawText DrawTextW
#else
#define DrawText DrawTextA
#endif

class CDirect3DHelper
{
public:
	CDirect3DHelper(void);
	~CDirect3DHelper(void);

	HRESULT InstallHooks();

	HRESULT DrawTextA(LPCSTR string, INT line, ...);
	HRESULT DrawTextW(LPCWSTR string, INT line, ...);
	HRESULT DrawDebugText();

	VOID SetFontColor(DWORD a, DWORD r, DWORD g, DWORD b);

	VOID SetDevicePtr(IDirect3DDevice9* ptr);
	IDirect3DDevice9* GetDevicePtr();

	VOID SetInterfacePtr(IDirect3D9* ptr);
	IDirect3D9* GetInterfacePtr();

private:
	VOID InitText();

	IDirect3D9* Direct3D9_;
	IDirect3DDevice9* Direct3DDevice9_;

	LPD3DXFONT font_;
	D3DCOLOR fontColor_;
	LPSTR textBufferA_;
	LPWSTR textBufferW_;
	RECT textRect_;

	BOOL currentInCutscene_;
};
