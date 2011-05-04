#include <intrin.h>
#include <tchar.h>
#include <windows.h>
#include "SysInfo.h"
#include "Globals.h"

#include <powrprof.h>
#pragma comment(lib, "powrprof.lib")

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
	typedef std::vector<PROCESSOR_POWER_INFORMATION> PPIVector;

	SYSTEM_INFO sys_info;
    PPIVector ppis;

    // find out how many processors we have in the system
    GetSystemInfo(&sys_info);
    ppis.resize(sys_info.dwNumberOfProcessors);

    // get CPU stats
    if (CallNtPowerInformation(ProcessorInformation, NULL, 0, &ppis[0], sizeof(PROCESSOR_POWER_INFORMATION) * ppis.size()) != ERROR_SUCCESS)
    {
		WriteToLog(_T("SysInfo"), _T("Unable to get CPU information.\n"));
        return false;
    }

	for (PPIVector::iterator it = ppis.begin(); it != ppis.end(); ++it)
    {
        WriteToLog(_T("SysInfo"), _T("stats for CPU %d: "), it->Number);
		WriteToLog(_T("SysInfo"), _T("max frequency: %d MHz"), it->MaxMhz);
		WriteToLog(_T("SysInfo"), _T("current frequency: %d MHz"), it->CurrentMhz);
    }

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
