#pragma once
#include <Ntifs.h>
#include <WinDef.h>
#include <wingdi.h>

#define Print(fmt, ...) DbgPrint("[s11]: " fmt, ##__VA_ARGS__)


//EPROC stuff for find eproc by name
ULONG imageFileNameOffset = 0x5a8; //_PEPROCESS->UCHAR ImageFileName[15];   
ULONG activeProcessLinksOffset = 0x448; //_PEPROCESS->_LIST_ENTRY ActiveProcessLinks; 
ULONG activeThreadsOffset = 0x5f0; //_PEPROCESS->ULONG ActiveThreads;   

//_KPROCESS stuff for translation
ULONG userDirectoryTableBaseOffset = 0x388; //_KPROCESS->ULONGLONG UserDirectoryTableBase; 

//_KTHREAD stuff for drawing
ULONG processOffset = 0x220; //_KTHREAD->_KPROCESS* Process;
ULONG win32ThreadOffset = 0x1c8; //_KTHREAD->VOID* volatile Win32Thread

//_ETHREAD stuff for drawing
ULONG cidOffset = 0x478; //_ETHREAD->_CLIENT_ID Cid;  

typedef DWORD LFTYPE;

extern "C"
{
	NTSTATUS NTAPI ZwQuerySystemInformation(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
	NTKERNELAPI PVOID NTAPI RtlFindExportedRoutineByName(PVOID ImageBase, PCCH RoutineNam);
	NTKERNELAPI PVOID NTAPI PsGetCurrentThreadWin32Thread(VOID);
}


typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;


typedef struct _SYSTEM_MODULE_ENTRY {
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG Count;
	SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;


PVOID NtUserGetDCPtr = 0;
PVOID NtGdiSelectBrushPtr = 0;
PVOID NtGdiPatBltPtr = 0;
PVOID NtUserReleaseDCPtr = 0;
PVOID NtGdiCreateSolidBrushPtr = 0;
PVOID NtGdiDeleteObjectAppPtr = 0;
PVOID NtGdiExtTextOutWPtr = 0;
PVOID NtGdiHfontCreatePtr = 0;
PVOID NtGdiSelectFontPtr = 0;

inline HDC NtUserGetDC(HWND hwnd)
{
	auto fn = reinterpret_cast<HDC(*)(HWND hwnd)>(NtUserGetDCPtr);
	return fn(hwnd);
}

inline HBRUSH NtGdiSelectBrush(HDC hdc, HBRUSH hbrush)
{
	auto fn = reinterpret_cast<HBRUSH(*)(HDC hdc, HBRUSH hbrush)>(NtGdiSelectBrushPtr);
	return fn(hdc, hbrush);
}

inline BOOL NtGdiPatBlt(HDC hdcDest, INT x, INT y, INT cx, INT cy, DWORD dwRop)
{
	auto fn = reinterpret_cast<BOOL(*)(HDC hdcDest, INT x, INT y, INT cx, INT cy, DWORD dwRop)>(NtGdiPatBltPtr);
	return fn(hdcDest, x, y, cx, cy, dwRop);
}

inline int NtUserReleaseDC(HDC hdc)
{
	auto fn = reinterpret_cast<int(*)(HDC hdc)>(NtUserReleaseDCPtr);
	return fn(hdc);
}

inline HBRUSH NtGdiCreateSolidBrush(COLORREF cr, HBRUSH hbr)
{
	auto fn = reinterpret_cast<HBRUSH(*)(COLORREF cr, HBRUSH hbr)>(NtGdiCreateSolidBrushPtr);
	return fn(cr, hbr);
}

inline BOOL NtGdiDeleteObjectApp(HANDLE hobj)
{
	auto fn = reinterpret_cast<BOOL(*)(HANDLE hobj)>(NtGdiDeleteObjectAppPtr);
	return fn(hobj);
}

inline BOOL NtGdiExtTextOutW(HDC hDC, INT XStart, INT YStart, UINT fuOptions, LPRECT UnsafeRect, LPWSTR UnsafeString, INT Count, LPINT UnsafeDx, DWORD dwCodePage)
{
	auto fn = reinterpret_cast<BOOL(*)(HDC hDC, INT XStart, INT YStart, UINT fuOptions, LPRECT UnsafeRect, LPWSTR UnsafeString, INT Count, LPINT UnsafeDx, DWORD dwCodePage)>(NtGdiExtTextOutWPtr);
	return fn(hDC, XStart, YStart, fuOptions, UnsafeRect, UnsafeString, Count, UnsafeDx, dwCodePage);
}

inline HFONT NtGdiHfontCreate(PENUMLOGFONTEXDVW pelfw, ULONG cjElfw, LFTYPE lft, FLONG fl, PVOID pvCliData)
{
	auto fn = reinterpret_cast<HFONT(*)(PENUMLOGFONTEXDVW pelfw, ULONG cjElfw, LFTYPE lft, FLONG fl, PVOID pvCliData)>(NtGdiHfontCreatePtr);
	return fn(pelfw, cjElfw, lft, fl, pvCliData);
}

inline HFONT NtGdiSelectFont(HDC hdc, HFONT hfont)
{
	auto fn = reinterpret_cast<HFONT(*)(HDC hdc, HFONT hfont)>(NtGdiSelectFontPtr);
	return fn(hdc, hfont);
}


