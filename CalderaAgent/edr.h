#include <windows.h>
#include <stdio.h> 
#include <string>
#include "syscalls_mem.h"
#include <winternl.h>

using namespace std;

//Defense Evasion
VOID PatchHook(PVOID address, unsigned char id);
char* ReadNTDLLFile(DWORD &FileSize);
