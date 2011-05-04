#pragma once

#include "common.h"
#include <vector>
#include <windows.h>

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
#define PRODUCT_PROFESSIONAL	0x00000030
#define VER_SUITE_WH_SERVER	0x00008000

#define CRC_STEAM_VER_1_2 0x077CE924D

typedef struct _PROCESSOR_POWER_INFORMATION {
      ULONG  Number;
      ULONG  MaxMhz;
      ULONG  CurrentMhz;
      ULONG  MhzLimit;
      ULONG  MaxIdleState;
      ULONG  CurrentIdleState;
    } PROCESSOR_POWER_INFORMATION , *PPROCESSOR_POWER_INFORMATION;
typedef std::vector<PROCESSOR_POWER_INFORMATION> PPIVector;

extern ULONG crc32_table[256];  // Lookup table array 
extern void Init_CRC32_Table();  // Builds lookup table array 
extern ULONG Reflect(ULONG ref, char ch);  // Reflects CRC bits in the lookup table 
extern int Get_CRC(char* text, int len);

extern int SR2_EXE_CRC;

extern BOOL WINAPI ProcessorDetect();
extern BOOL WINAPI LogOSDetect();
extern VOID WINAPI LogSystemInfo();
extern BOOL WINAPI CheckSaintsRow2Integrity();