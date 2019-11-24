#include "vmgenerationcounter.h"

HRESULT GetVmCounter(bool fWaitForChange)
{
	BOOL success = FALSE;
	DWORD error = ERROR_SUCCESS;
	VM_GENCOUNTER vmCounterOutput = { 0 };
	DWORD size = 0;
	HANDLE handle = INVALID_HANDLE_VALUE;
	HRESULT hr = S_OK;

	handle = CreateFile(
		L"\\\\.\\" VM_GENCOUNTER_SYMBOLIC_LINK_NAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		error = GetLastError();
		wprintf(L"Unable to open device %s. Error code = %d.", VM_GENCOUNTER_SYMBOLIC_LINK_NAME, error);
		hr = HRESULT_FROM_WIN32(error);
		goto Cleanup;
	}

	// Call into the driver.
	// Because the 4th parameter to DeviceIoControl (nInBufferSize) is zero, this
	// is a polling request rather than an event-driven request.
	success = DeviceIoControl(
		handle,
		IOCTL_VMGENCOUNTER_READ,
		NULL,
		0,
		&vmCounterOutput,
		sizeof(vmCounterOutput),
		&size,
		NULL);

	if (!success)
	{
		error = GetLastError();
		wprintf(L"Call IOCTL_VMGENCOUNTER_READ failed with %d.", error);
		hr = HRESULT_FROM_WIN32(error);
		goto Cleanup;
	}

	wprintf(L"VmCounterValue: %I64x:%I64x\n", vmCounterOutput.GenerationCount, vmCounterOutput.GenerationCountHigh);

	wprintf(L"msDS-GenerationId: ");
	for (int i = 0; i < 8; i++)
	{
		wprintf(L"%02I64X ", vmCounterOutput.GenerationCount >> (8 * i) & 0x00000000000000ff);
	}
	wprintf(L"\n");

	if (fWaitForChange)
	{
		// Call into the driver again in event-driven mode. DeviceIoControl won't
		// return until the generation identifier has changed.
		success = DeviceIoControl(
			handle,
			IOCTL_VMGENCOUNTER_READ,
			&vmCounterOutput,
			sizeof(vmCounterOutput),
			&vmCounterOutput,
			sizeof(vmCounterOutput),
			&size,
			NULL);

		if (!success)
		{
			error = GetLastError();
			wprintf(L"Call IOCTL_VMGENCOUNTER_READ failed with %d.", error);
			hr = HRESULT_FROM_WIN32(error);
			goto Cleanup;
		}

		wprintf(L"VmCounterValue changed to: %I64x:%I64x", vmCounterOutput.GenerationCount, vmCounterOutput.GenerationCountHigh);
	}

Cleanup:

	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
	}

	return hr;
};


INT wmain(INT argc, WCHAR* argv[])
{
	GetVmCounter(FALSE);
	return 0;
}
