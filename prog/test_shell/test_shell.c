// Need to link with Iphlpapi.lib and Ws2_32.lib
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

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

char* _strcpy(char* dst,rsize_t size, const char* src) {
    char* d = dst;
    int i = 0;
    while ((*d++ = *src++)&&i<size)
        ;
    return dst;
}

int main()
{
    printf("%x", 600^0xffffffff);
    // Declare and initialize variables
    PMIB_TCPTABLE pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    char* port = (char*)malloc(6);
    char szLocalAddr[128];

    struct in_addr IpAddr;

    int i;

    pTcpTable = (MIB_TCPTABLE*)malloc(sizeof(MIB_TCPTABLE));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    dwSize = sizeof(MIB_TCPTABLE);
    // Make an initial call to GetTcpTable to
    // get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) ==
        ERROR_INSUFFICIENT_BUFFER) {
        pTcpTable = (MIB_TCPTABLE*)malloc(dwSize);
        if (pTcpTable == NULL) {
            printf("Error allocating memory\n");
            return 1;
        }
    }
    char cstrFileName[] = { 'r','e','s','u','l','t','.','t','x','t','\0' };
    HANDLE hFile = CreateFileA(cstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    DWORD bytesWritten;
    char newLine = '\n';
    // Make a second call to GetTcpTable to get
    // the actual data we require
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
        for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            _strcpy(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;

            printf("\nTCP Local Addr: %s ", szLocalAddr);
            printf(" local Port: %d", ntohs((u_short)pTcpTable->table[i].dwLocalPort));
            WriteFile(hFile,"TCP Local Addr: ", strlen("TCP Local Addr: "), &bytesWritten, NULL);
            WriteFile(hFile, szLocalAddr, strlen(szLocalAddr), &bytesWritten, NULL);
            WriteFile(hFile, " local Port:", strlen(" local Port:"), &bytesWritten, NULL);
            memset(port, 0, 6);
            port = _iltoa((u_short)pTcpTable->table[i].dwLocalPort, port, 10);
            WriteFile(hFile, port, strlen(port), &bytesWritten, NULL);
            WriteFile(hFile, &newLine, 1, &bytesWritten, NULL);
        }
    }
    return 0;
}