extern main
global _start
_start:
	mov [environ], esi
	mov [argc], eax
	mov [argv], ebx
	push dword [argv]
	push dword [argc]
	call main
	mov ebx, eax
	mov eax, 1
	int 0x80
	jmp $

section .bss
environ: resd 1
argc: resd 1
argv: resd 1