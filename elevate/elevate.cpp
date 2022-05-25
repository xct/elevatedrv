#include <Windows.h>
#include <stdio.h>
#include "..\kelevate\kelevate.h"

int main(int argc, char** argv) {

	HANDLE hDevice = CreateFile(L"\\\\.\\Kelevate", GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Error opening device (%u)\n", GetLastError());
		return 1;
	}

	DWORD currentProcessPid = GetCurrentProcessId();	
	ProcessData data;
	data.ProcessId = currentProcessPid;
	printf("[*] Elevating %d\n", data.ProcessId);

	DWORD bytesRet;
	if(!DeviceIoControl(hDevice, IOCTL_ELEVATE, &data, sizeof(data), nullptr, 0, &bytesRet, nullptr)){
		printf("Error in DeviceIoControl (%u)\n", GetLastError());
		return 1;
	}

	printf("[>] Done\n");
	CloseHandle(hDevice);
	system("cmd.exe");
}