#pragma once
#include "common.h"
#include <windows.h>

extern LPSTR hkg_cutsceneName;
extern BOOL  hkg_isCutscenePlaying;
extern FLOAT* hkg_playerPosition;
extern FLOAT hkg_timescale;
extern FLOAT hkg_origTimescale;
extern __int64 hkg_freqTarget;
extern BOOL hkg_osdEnabled;
extern BYTE* hkg_tableSize;
extern vector<char*> g_debugLines;
extern BOOL hkg_initializedCriticalSection;

extern SHORT hkg_consoleBuffer;
extern BOOL hkg_logLua;
extern HANDLE hkg_logFile;

extern VOID WINAPI WriteToLog(TCHAR*, TCHAR*, ...);

typedef union {
  void *gc;
  void *p;
  double n;
  int b;
} Value;

typedef struct lua_TObject {
  int tt;
  Value value;
} TObject;


typedef unsigned char lu_byte;
typedef TObject *StkId;  /* index to stack elements */

#define CommonHeader    void *next; lu_byte tt; lu_byte marked

struct lua_State {
  CommonHeader;
  DWORD* top;  /* first free slot in the stack */
  DWORD* base;  /* base of current function */
  void *l_G;
  void *ci;  /* call info for current function */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  int stacksize;
  void *end_ci;  /* points after end of ci array*/
  void *base_ci;  /* array of CallInfo's */
  unsigned short size_ci;  /* size of array `base_ci' */
  unsigned short nCcalls;  /* number of nested C calls */
  lu_byte hookmask;
  lu_byte allowhook;
  lu_byte hookinit;
  int basehookcount;
  int hookcount;
  void* hook;
  DWORD _gt;  /* table of globals */
  void *openupval;  /* list of open upvalues in this stack */
  void *gclist;
  void *errorJmp;  /* current error recover point */
  void *errfunc;  /* current error handling function (stack index) */
};
