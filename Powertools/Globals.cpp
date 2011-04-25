#include <windows.h>

LPSTR	hkg_cutsceneName		= NULL;
BOOL	hkg_isCutscenePlaying	= FALSE;
FLOAT*  hkg_playerPosition		= (FLOAT*)0x00FA6DB0;
BYTE*   hkg_tableSize			= (BYTE*)0x00D74D52;


FLOAT	hkg_timescale			= 0.0;
FLOAT	hkg_origTimescale		= 0.0;
BOOL	hkg_osdEnabled			= TRUE;
BOOL    hkg_initializedCriticalSection = FALSE;