#pragma once
#include "Misc.h"

PETHREAD GetValidWin32Thread(PVOID* win32Thread)
{
	int currentThreadId = 1;
	NTSTATUS status = STATUS_SUCCESS;
	do
	{
		PETHREAD currentEthread = 0;
		status = PsLookupThreadByThreadId((HANDLE)currentThreadId, &currentEthread);

		if (!NT_SUCCESS(status) || !currentEthread)
		{
			currentThreadId++;
			continue;
		}

		if (PsIsThreadTerminating(currentEthread))
		{
			currentThreadId++;
			continue;
		}

		PVOID Win32Thread;
		memcpy(&Win32Thread, (PVOID)((UINT64)currentEthread + win32ThreadOffset), sizeof(PVOID));

		if (Win32Thread)
		{
			PEPROCESS threadOwner = PsGetThreadProcess(currentEthread);
			char procName[15];
			memcpy(&procName, (PVOID)((UINT64)threadOwner + imageFileNameOffset), sizeof(procName));
			if (!strcmp(procName, "explorer.exe"))
			{
				*win32Thread = Win32Thread;
				return currentEthread;
			}
		}
		currentThreadId++;
	} while (0x3000 > currentThreadId);

	return 0;
}

inline void SpoofWin32Thread(PVOID newWin32Value, PEPROCESS newProcess, CLIENT_ID newClientId)
{
	PKTHREAD currentThread = KeGetCurrentThread();

	PVOID win32ThreadPtr = (PVOID)((char*)currentThread + win32ThreadOffset);
	memcpy(win32ThreadPtr, &newWin32Value, sizeof(PVOID));

	PVOID processPtr = (PVOID)((char*)currentThread + processOffset);
	memcpy(processPtr, &newProcess, sizeof(PEPROCESS));

	PVOID clientIdPtr = (PVOID)((char*)currentThread + cidOffset);
	memcpy(clientIdPtr, &newClientId, sizeof(CLIENT_ID));
}

KAPC_STATE apc = { 0 };

PVOID currentWin32Thread = 0;
PEPROCESS currentProcess = 0;
PETHREAD currentThread = 0;
CLIENT_ID currentCid = { 0 };

HDC hdc;
HBRUSH brush;

bool BeginFrame()
{
	PVOID targetWin32Thread = 0;
	PETHREAD targetThread = GetValidWin32Thread(&targetWin32Thread);
	if (!targetWin32Thread || !targetThread)
	{
		Print("failed to find win32thread");
		return false;
	}
	PEPROCESS targetProcess = PsGetThreadProcess(targetThread);

	CLIENT_ID targetCid = { 0 };
	memcpy(&targetCid, (PVOID)((char*)targetThread + cidOffset), sizeof(CLIENT_ID));

	KeStackAttachProcess(targetProcess, &apc);
	SpoofWin32Thread(targetWin32Thread, targetProcess, targetCid);



	hdc = NtUserGetDC(0);
	if (!hdc)
	{
		Print("failed to get userdc");
		return false;
	}

	brush = NtGdiCreateSolidBrush(RGB(255, 0, 0), NULL);
	if (!brush)
	{
		Print("failed create brush");
		NtUserReleaseDC(hdc);
		return false;
	}
}

void EndFrame()
{
	NtGdiDeleteObjectApp(brush);
	NtUserReleaseDC(hdc);

	SpoofWin32Thread(currentWin32Thread, currentProcess, currentCid);
	KeUnstackDetachProcess(&apc);
}


INT FrameRect(RECT rect, int thickness)
{
	HBRUSH oldBrush = NtGdiSelectBrush(hdc, brush);
	if (!oldBrush)
	{
		Print("failed to get brush");
		return 0;
	}


	NtGdiPatBlt(hdc, rect.left, rect.top, thickness, rect.bottom - rect.top, PATCOPY);
	NtGdiPatBlt(hdc, rect.right - thickness, rect.top, thickness, rect.bottom - rect.top, PATCOPY);
	NtGdiPatBlt(hdc, rect.left, rect.top, rect.right - rect.left, thickness, PATCOPY);
	NtGdiPatBlt(hdc, rect.left, rect.bottom - thickness, rect.right - rect.left, thickness, PATCOPY);

	NtGdiSelectBrush(hdc, oldBrush);
	return 1;
}