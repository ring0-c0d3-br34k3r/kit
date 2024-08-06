#ifndef _TRANSME_IM_JUSTan_PATH_
#define _TRANSME_IM_JUSTan_PATH_ 1

NTSTATUS PsGetTarProcessInfo(HANDLE pid);

BOOLEAN PathPebLdr(PEPROCESS Process, WCHAR* szFullName, WCHAR* szBaseName);

BOOLEAN PathPebProcessParameters(PEPROCESS Process, WCHAR* szFullName);

BOOLEAN PathSeAuditProcessCreationInfo(PEPROCESS Process, WCHAR* ProcessName);

BOOLEAN PathImageFileName(PEPROCESS Process, char* cName);

BOOLEAN PathSeFileObject(PEPROCESS Process, WCHAR* szFullName);

BOOLEAN Transing(HANDLE pid);
#endif
