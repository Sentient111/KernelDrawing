#pragma once
#include "Nt.h"

void Sleep(int ms)
{
	LARGE_INTEGER time = { 0 };
	time.QuadPart = -(ms) * 10 * 1000;
	KeDelayExecutionThread(KernelMode, TRUE, &time);
}

PVOID QuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInfoClass, ULONG* size)
{
	int currAttempt = 0;
	int maxAttempt = 20;


QueryTry:
	if (currAttempt >= maxAttempt)
		return 0;

	currAttempt++;
	ULONG neededSize = 0;
	ZwQuerySystemInformation(SystemInfoClass, NULL, neededSize, &neededSize);
	if (!neededSize)
		goto QueryTry;

	ULONG allocationSize = neededSize;
	PVOID informationBuffer = ExAllocatePool(NonPagedPool ,allocationSize);
	if (!informationBuffer)
		goto QueryTry;

	NTSTATUS status = ZwQuerySystemInformation(SystemInfoClass, informationBuffer, neededSize, &neededSize);
	if (!NT_SUCCESS(status))
	{
		ExFreePoolWithTag(informationBuffer, 0);
		goto QueryTry;
	}

	*size = allocationSize;
	return informationBuffer;
}


UINT64 GetKernelModuleBase(const char* name)
{
	ULONG size = 0;
	PSYSTEM_MODULE_INFORMATION moduleInformation = (PSYSTEM_MODULE_INFORMATION)QuerySystemInformation(SystemModuleInformation, &size);

	if (!moduleInformation || !size)
		return 0;

	for (size_t i = 0; i < moduleInformation->Count; i++)
	{
		char* fileName = (char*)moduleInformation->Module[i].FullPathName + moduleInformation->Module[i].OffsetToFileName;
		if (!strcmp(fileName, name))
		{
			UINT64 imageBase = (UINT64)moduleInformation->Module[i].ImageBase;
			ExFreePoolWithTag(moduleInformation, 0);
			return imageBase;
		}
	}

	ExFreePoolWithTag(moduleInformation, 0);
}

