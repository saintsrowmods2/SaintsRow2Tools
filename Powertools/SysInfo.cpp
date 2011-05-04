#include <intrin.h>
#include <tchar.h>
#include <vector>
#include <windows.h>
#include "SysInfo.h"
#include "Globals.h"

#include <powrprof.h>
#pragma comment(lib, "powrprof.lib")

ULONG crc32_table[256];
int SR2_EXE_CRC = 0;

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
	char CPUBrandString[0x40];
	int CPUInfo[4] = {-1};
	int nSteppingID = 0;
	int nModel = 0;
	int nFamily = 0;
	int nProcessorType = 0;
	int nExtendedmodel = 0;
	int nExtendedfamily = 0;
	int nBrandIndex = 0;

	unsigned int nIds = 0;
	__cpuid(CPUInfo, 0);

	nIds = CPUInfo[0];
    memset(CPUString, 0, sizeof(CPUString));
    *((int*)CPUString) = CPUInfo[1];
    *((int*)(CPUString+4)) = CPUInfo[3];
    *((int*)(CPUString+8)) = CPUInfo[2];

	if (sizeof(TCHAR) == sizeof(char))
		WriteToLog(_T("CPU Info"), _T("Current CPU manufacturer string: %s\n"), (TCHAR*)CPUString);
	else
	{
		TCHAR* CPUStringW = NULL;
		DWORD CPUStringLen = MultiByteToWideChar(CP_ACP, 0, CPUString, -1, NULL, 0);
		CPUStringW = (TCHAR*)malloc(CPUStringLen * sizeof(TCHAR));
		MultiByteToWideChar(CP_ACP, 0, CPUString, -1, (LPWSTR)CPUStringW, CPUStringLen);
		WriteToLog(_T("CPU Info"), _T("Current CPU manufacturer string: %s\n"), (TCHAR*)CPUStringW);
		free(CPUStringW);
	}

	__cpuid(CPUInfo, 1);
	nSteppingID = CPUInfo[0] & 0xf;
	nModel = (CPUInfo[0] >> 4) & 0xf;
	nFamily = (CPUInfo[0] >> 8) & 0xf;
	nProcessorType = (CPUInfo[0] >> 12) & 0x3;
	nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
	nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
	nBrandIndex = CPUInfo[1] & 0xff;

	WriteToLog(_T("CPU Info"), _T("Family: %d, Model: %d, Stepping: %d, Type: %d, ExtendedModel: %d, ExtendedFamily: %d, BrandIndex: %d\n"),
		nFamily, nModel, nSteppingID, nProcessorType, nExtendedmodel, nExtendedfamily, nBrandIndex);

	__cpuid(CPUInfo, 0x80000000);
	memset(CPUBrandString, 0, sizeof(CPUBrandString));
	if (CPUInfo[0] >= 0x80000004)
	{
		__cpuid(CPUInfo, 0x80000002);
		memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		__cpuid(CPUInfo, 0x80000003);
		memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		__cpuid(CPUInfo, 0x80000004);
		memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));

		if (sizeof(TCHAR) == sizeof(char))
			WriteToLog(_T("CPU Info"), _T("Current CPU Brand string: %s\n"), (TCHAR*)CPUBrandString);
		else
		{
			TCHAR* CPUStringW = NULL;
			DWORD CPUStringLen = MultiByteToWideChar(CP_ACP, 0, CPUBrandString, -1, NULL, 0);
			CPUStringW = (TCHAR*)malloc(CPUStringLen * sizeof(TCHAR));
			MultiByteToWideChar(CP_ACP, 0, CPUBrandString, -1, (LPWSTR)CPUStringW, CPUStringLen);
			WriteToLog(_T("CPU Info"), _T("Current CPU Brand string: %s\n"), (TCHAR*)CPUStringW);
			free(CPUStringW);
		}
	}
	else
	{
		WriteToLog(_T("CPU Info"), _T("Cannot get CPU Brand String."));
		return false;
	}

	SYSTEM_INFO sys_info;
    PPIVector ppis;

    // find out how many processors we have in the system
    GetSystemInfo(&sys_info);
    ppis.resize(sys_info.dwNumberOfProcessors);

    // get CPU stats
    if (CallNtPowerInformation(ProcessorInformation, NULL, 0, &ppis[0], sizeof(PROCESSOR_POWER_INFORMATION) * ppis.size()) != ERROR_SUCCESS)
    {
		WriteToLog(_T("CPU Info"), _T("Unable to get CPU information.\n"));
        return false;
    }

	for (PPIVector::iterator it = ppis.begin(); it != ppis.end(); ++it)
    {
		WriteToLog(_T("CPU Info"), _T("stats for CPU %d: max: %d MHz, current: %d MHz\n"), it->Number, it->MaxMhz, it->CurrentMhz);
    }

	return true;
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

	WriteToLog(_T("OS Info"), _T("OS information: PlatformId: %d, Version: %d.%d, Build: %d, Service Pack: %d.%d (%s), Suite Mask: 0x%08X, Processor Architecture: %s\n"),
		osvi.dwPlatformId, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.wServicePackMajor, osvi.wServicePackMinor, osvi.szCSDVersion, osvi.wSuiteMask,
		(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ? _T("Itanium") : (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? _T("x86_64") : _T("x86"))));

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (osvi.dwMajorVersion == 5)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					WriteToLog(_T("OS Info"), _T("OS is Windows 2000 Professional.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					WriteToLog(_T("OS Info"), _T("OS is Windows 2000 Datacenter Server.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					WriteToLog(_T("OS Info"), _T("OS is Windows 2000 Enterprise Server.\n"));
				else
					WriteToLog(_T("OS Info"), _T("OS is Windows 2000 Server.\n"));
			}
			else if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					WriteToLog(_T("OS Info"), _T("OS is Windows XP Home.\n"));
				else
					WriteToLog(_T("OS Info"), _T("OS is Windows XP Professional.\n"));
			}
			else if (osvi.dwMinorVersion == 2)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					WriteToLog(_T("OS Info"), _T("OS is Windows XP Professional x64 Edition.\n"));
				else if (GetSystemMetrics(SM_SERVERR2))
					WriteToLog(_T("OS Info"), _T("OS is Windows Server 2003 R2.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
					WriteToLog(_T("OS Info"), _T("OS is Windows Storage Server 2003.\n"));
				else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
					WriteToLog(_T("OS Info"), _T("OS is Windows Home Server.\n"));
				else
					WriteToLog(_T("OS Info"), _T("OS is Windows Server 2003.\n"));
			}
			else
			{
				WriteToLog(_T("OS Info"), _T("Unknown minor version.\n"));
				return false;
			}
		}
		else if (osvi.dwMajorVersion == 6)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					WriteToLog(_T("OS Info"), _T("OS is Windows Vista.\n"));
				else
					WriteToLog(_T("OS Info"), _T("OS is Windows Server 2008.\n"));
			}
			else if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					WriteToLog(_T("OS Info"), _T("OS is Windows 7.\n"));
				else
					WriteToLog(_T("OS Info"), _T("OS is Windows Server 2008 R2.\n"));
			}
			else
			{
				WriteToLog(_T("OS Info"), _T("Unknown minor version.\n"));
				return false;
			}
		}
		else if (osvi.dwMajorVersion < 5)
		{
			WriteToLog(_T("OS Info"), _T("Unknown (old?) Windows version.\n"));
			return false;
		}
		else if (osvi.dwMajorVersion > 6)
		{
			WriteToLog(_T("OS Info"), _T("Unknown (future?) Windows version.\n"));
			return false;
		}
	}
	else
	{
		WriteToLog(_T("OS Info"), _T("OS platform is unknown.\n"), osvi.dwPlatformId);
		return false;
	}

	return true;
}

BOOL WINAPI CheckSaintsRow2Integrity()
{
	HANDLE sr2Exe;
	DWORD exeLength;
	LPVOID* buffer;
	DWORD totalBytesRead = 0;
	DWORD bytesRead = 0;
	DWORD crc;

	Init_CRC32_Table();

	sr2Exe = CreateFileW(TEXT("SR2_pc.exe"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	exeLength = GetFileSize(sr2Exe, NULL);

	buffer = (LPVOID*)malloc(exeLength);
	
	while (totalBytesRead < exeLength)
	{
		if (!ReadFile(sr2Exe, buffer + totalBytesRead, exeLength - totalBytesRead, &bytesRead, NULL))
		{
			CloseHandle(sr2Exe);
			return false;
		}
		totalBytesRead += bytesRead;
	}
	CloseHandle(sr2Exe);
	
	crc = Get_CRC((char*)buffer, exeLength);
	free(buffer);

	if (crc == CRC_STEAM_VER_1_2)
	{
		WriteToLog(_T("Powertools"), _T("Found Steam SR2_pc.exe version 1.2, CRC32: 0x%08X\n"), crc);
		SR2_EXE_CRC = crc;
		return true;
	}
	else
	{
		WriteToLog(_T("Powertools"), _T("Unrecognised SR2_pc.exe found. CRC32: 0x%08X\n"), crc);
		return false;
	}

	return false;
}

void Init_CRC32_Table() 
{// Call this function only once to initialize the CRC table. 

      // This is the official polynomial used by CRC-32 
      // in PKZip, WinZip and Ethernet. 
      ULONG ulPolynomial = 0x04c11db7; 

      // 256 values representing ASCII character codes. 
      for(int i = 0; i <= 0xFF; i++) 
      { 
            crc32_table[i]=Reflect(i, 8) << 24; 
            for (int j = 0; j < 8; j++) 
                  crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0); 
            crc32_table[i] = Reflect(crc32_table[i], 32); 
      } 
} 

ULONG Reflect(ULONG ref, char ch) 
{// Used only by Init_CRC32_Table(). 

      ULONG value(0); 

      // Swap bit 0 for bit 7 
      // bit 1 for bit 6, etc. 
      for(int i = 1; i < (ch + 1); i++) 
      { 
            if(ref & 1) 
                  value |= 1 << (ch - i); 
            ref >>= 1; 
      } 
      return value; 
}

int Get_CRC(char* text, int len) 
{// Pass a text string to this function and it will return the CRC. 

      // Once the lookup table has been filled in by the two functions above, 
      // this function creates all CRCs using only the lookup table. 

      // Be sure to use unsigned variables, 
      // because negative values introduce high bits 
      // where zero bits are required. 

      // Start out with all bits set high. 
      ULONG  ulCRC(0xffffffff); 
      unsigned char* buffer; 

      // Save the text in the buffer. 
      buffer = (unsigned char*)text; 
      // Perform the algorithm on each character 
      // in the string, using the lookup table values. 
      while(len--) 
            ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++]; 
      // Exclusive OR the result with the beginning value. 
      return ulCRC ^ 0xffffffff; 
}