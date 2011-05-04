#pragma once

#include "common.h"
#include <vector>
#include <windows.h>

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
#define PRODUCT_PROFESSIONAL	0x00000030
#define VER_SUITE_WH_SERVER	0x00008000

typedef struct _PROCESSOR_POWER_INFORMATION {
      ULONG  Number;
      ULONG  MaxMhz;
      ULONG  CurrentMhz;
      ULONG  MhzLimit;
      ULONG  MaxIdleState;
      ULONG  CurrentIdleState;
    } PROCESSOR_POWER_INFORMATION , *PPROCESSOR_POWER_INFORMATION;
typedef std::vector<PROCESSOR_POWER_INFORMATION> PPIVector;

extern BOOL WINAPI ProcessorDetect();
extern BOOL WINAPI LogOSDetect();
extern VOID WINAPI LogSystemInfo();
extern BOOL WINAPI CheckSaintsRow2Integrity();