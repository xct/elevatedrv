#pragma once

#define IOCTL_ELEVATE CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TOKEN_OFFSET 0x4b8
#define QWORD ULONGLONG

struct ProcessData {
	ULONG ProcessId;
};
