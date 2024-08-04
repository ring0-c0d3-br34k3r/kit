#ifndef HARDWAREIO_H
#define HARDWAREIO_H

#include <windows.h>
#include <tchar.h>
#include <ntsecapi.h>
#include <process.h>
#include <string.h>

BOOL EnablePrivilege(PTCHAR Privilege);
BOOL EnableProcPrivilege();
BOOL EnableUserModeHardwareIO();
int __cdecl main(int argc, char* argv[]);

#endif
