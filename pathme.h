#ifndef _TRANSME_IM_JUSTan_PATH_
#define _TRANSME_IM_JUSTan_PATH_ 1

NTSTATUS prcc_Info(HANDLE pid);
  BOOLEAN PebHouse(PEPROCESS Process, WCHAR* szFullName, WCHAR* szBaseName);
    BOOLEAN TakeMe(PEPROCESS Process, WCHAR* szFullName);
      BOOLEAN PairToPair(PEPROCESS Process, WCHAR* ProcessName);

      BOOLEAN MyFace(PEPROCESS Process, char* cName);
    BOOLEAN PebFace(PEPROCESS Process, WCHAR* szFullName);
  BOOLEAN Transing(HANDLE pid);
BOOLEAN W10_imgPOINT(PEPROCESS Process, WCHAR* szFullName);

#endif
