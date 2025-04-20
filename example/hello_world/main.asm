.intel_syntax noprefix
.section .text
.global _start

_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:
	# store @s32 0, %01
	mov DWORD PTR [rsp - 4], 0
	# $03 @s32 = load %01
	mov eax, DWORD PTR [rsp - 4]
	mov DWORD PTR [rsp - 8], eax
	# ret $03
	mov eax, DWORD PTR [rsp - 8]
	ret

.section .data
