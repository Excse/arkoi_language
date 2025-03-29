.intel_syntax noprefix
.section .text
.global _start

_start:
	and rsp, -16
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:
	enter 16, 0
	# store @u32 a, %02
	mov DWORD PTR [rbp - 20], edi
	# $19 @u64 = call calling_convention(1, 2, 3, 4, 5, 6, 7, 8)
	call calling_convention
	# $20 @u32 = cast @u64 $19
	# TODO: Not implemented yet.
	# store @u32 $20, %01
	mov eax, DWORD PTR [rbp - 8]
	mov DWORD PTR [rbp - 16], eax
	# $21 @u32 = load %01
	mov eax, DWORD PTR [rbp - 16]
	mov DWORD PTR [rbp - 12], eax
	# ret $21
	mov eax, DWORD PTR [rbp - 12]
	leave
	ret

calling_convention:
	# store @u32 a, %02
	mov DWORD PTR [rbp - 24], edi
	# store @u32 b, %03
	mov DWORD PTR [rbp - 28], esi
	# store @u32 c, %04
	mov DWORD PTR [rbp - 32], edx
	# store @u32 d, %05
	mov DWORD PTR [rbp - 36], ecx
	# store @u32 e, %06
	mov DWORD PTR [rbp - 40], r8d
	# store @u32 f, %07
	mov DWORD PTR [rbp - 44], r9d
	# store @u32 g, %08
	mov eax, DWORD PTR [rbp + 16]
	mov DWORD PTR [rbp - 48], eax
	# store @u32 h, %09
	mov eax, DWORD PTR [rbp + 24]
	mov DWORD PTR [rbp - 52], eax
	# store @u64 1, %01
	mov QWORD PTR [rbp - 16], 1
	# $12 @u64 = load %01
	mov rax, QWORD PTR [rbp - 16]
	mov QWORD PTR [rbp - 8], rax
	# ret $12
	mov rax, QWORD PTR [rbp - 8]
	ret

.section .data
