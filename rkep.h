#ifndef _RTKEPROCESS_H_
#define _RTKEPROCESS_H_

#include <ntddk.h>

#pragma alloc_text(INIT, DriverEntry)
  #pragma alloc_text(PAGE, OnUnload)
#pragma alloc_text(PAGE, Dispatch)

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING theRegistryPath);
  VOID OnUnload(IN PDRIVER_OBJECT DriverObject);
  NTSTATUS Dispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS HideProcess();

#define DEVICE_NAME L"\\Device\\RTKEProcess"
#define DOS_DEVICE_NAME L"\\DosDevices\\RTKEProcess"

PLIST_ENTRY pList;

#endif 
