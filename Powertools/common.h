#pragma once

#define CINTERFACE

//
// minimum platforms
//

#define WINVER _WIN32_WINNT_LONGHORN
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN
#define _WIN32_WINDOWS _WIN32_WINNT_LONGHORN
#define _WIN32_IE _WIN32_IE_IE70

//
// eliminates a bunch of APIs, speeding up compile
//

#define NOSOUND
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX

//
// some macro magic
//

#if UNICODE || _UNICODE
#define tstring wstring
#else
#define tstring string
#endif