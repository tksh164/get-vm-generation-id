#pragma once

#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wchar.h>

DEFINE_GUID(
	GUID_DEVINTERFACE_VM_GENCOUNTER,
	0x3ff2c92b,
	0x6598,
	0x4e60,
	0x8e,
	0x1c,
	0x0c,
	0xcf,
	0x49,
	0x27,
	0xe3,
	0x19);

#define VM_GENCOUNTER_SYMBOLIC_LINK_NAME L"VmGenerationCounter"

#define IOCTL_VMGENCOUNTER_READ CTL_CODE( \
    FILE_DEVICE_ACPI, \
    0x1, METHOD_BUFFERED, \
    FILE_READ_ACCESS | FILE_WRITE_ACCESS)

typedef struct _VM_GENCOUNTER
{
	ULONGLONG GenerationCount;
	ULONGLONG GenerationCountHigh;
} VM_GENCOUNTER, * PVM_GENCOUNTER;
