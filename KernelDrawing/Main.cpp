#include "Drawing.h"


void MainThread()
{
	Print("doin the thing");
	currentProcess = IoGetCurrentProcess();
	currentThread = KeGetCurrentThread();
	memcpy(&currentCid, (PVOID)((char*)currentThread + cidOffset), sizeof(CLIENT_ID));

	while (true)
	{
		BeginFrame();

		FrameRect({ 100, 100, 200, 200 }, 3);

		EndFrame();
	}

	PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS CreateThread(PVOID entry)
{
	HANDLE threadHandle = NULL;
	NTSTATUS status = PsCreateSystemThread(&threadHandle, NULL, NULL, NULL, NULL, (PKSTART_ROUTINE)entry, NULL);

	if (!NT_SUCCESS(status))
	{
		Print("failed to create system thread, %x", status);
		return status;
	}

	ZwClose(threadHandle);
	return status;
}

extern "C" NTSTATUS DriverEntry()
{
	Print("welcome");
	NTSTATUS status = STATUS_SUCCESS;

	
	PVOID win32kBase = (PVOID)GetKernelModuleBase("win32kbase.sys");
	PVOID win32kfullBase = (PVOID)GetKernelModuleBase("win32kfull.sys");

	if (!win32kBase || !win32kfullBase)
	{
		Print("Could not find kernel module bases");
		return STATUS_UNSUCCESSFUL;
	}

	NtUserGetDCPtr = RtlFindExportedRoutineByName(win32kBase, "NtUserGetDC");
	NtGdiPatBltPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiPatBlt");
	NtGdiSelectBrushPtr = RtlFindExportedRoutineByName(win32kBase, "GreSelectBrush");
	NtUserReleaseDCPtr = RtlFindExportedRoutineByName(win32kBase, "NtUserReleaseDC");
	NtGdiCreateSolidBrushPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiCreateSolidBrush");
	NtGdiDeleteObjectAppPtr = RtlFindExportedRoutineByName(win32kBase, "NtGdiDeleteObjectApp");
	NtGdiExtTextOutWPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiExtTextOutW");
	NtGdiHfontCreatePtr = RtlFindExportedRoutineByName(win32kfullBase, "hfontCreate");
	NtGdiSelectFontPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiSelectFont");


	if (!NtUserGetDCPtr || !NtGdiPatBltPtr || !NtGdiSelectBrushPtr ||
		!NtUserReleaseDCPtr || !NtGdiCreateSolidBrushPtr || !NtGdiDeleteObjectAppPtr
		|| !NtGdiExtTextOutWPtr || !NtGdiHfontCreatePtr || !NtGdiSelectFontPtr)
	{
		Print("Could not find kernel functions required for drawing");
		return STATUS_UNSUCCESSFUL;
	}

	Print("found everything");
	CreateThread(MainThread);
	
	return STATUS_SUCCESS;
}
