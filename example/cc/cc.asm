.intel_syntax noprefix
.section .text
.global _start

_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:
	enter 16, 0
	# $19 @u64 = call calling_convention(1, 2, 3, 4, 5, 6, 7, 8)
	sub rsp, 16
	mov edi, 1
	mov esi, 2
	mov edx, 3
	mov ecx, 4
	mov r8d, 5
	mov r9d, 6
	mov DWORD PTR [rsp], 7
	mov DWORD PTR [rsp + 8], 8
	call calling_convention
	add rsp, 16
	# $20 @u32 = cast @u64 $19
	mov DWORD PTR [rbp - 8], eax
	# store @u32 $20, %01
	mov eax, DWORD PTR [rbp - 8]
	mov DWORD PTR [rbp - 4], eax
	# $21 @u32 = load %01
	mov eax, DWORD PTR [rbp - 4]
	mov DWORD PTR [rbp - 12], eax
	# ret $21
	mov eax, DWORD PTR [rbp - 12]
	leave
	ret

calling_convention:
	# store @u64 1, %01
	mov QWORD PTR [rsp - 8], 1
	# $12 @u64 = load %01
	mov rax, QWORD PTR [rsp - 8]
	mov QWORD PTR [rsp - 16], rax
	# ret $12
	mov rax, QWORD PTR [rsp - 16]
	ret

.section .data
