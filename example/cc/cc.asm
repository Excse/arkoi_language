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
	# $04 @u32 = const 1
	mov edi, 1
	# $06 @u32 = const 2
	mov esi, 2
	# $08 @u32 = const 3
	mov edx, 3
	# $10 @u32 = const 4
	mov ecx, 4
	# $12 @u32 = const 5
	mov r8d, 5
	# $14 @u32 = const 6
	mov r9d, 6
	# $16 @u32 = const 7
	push 7
	# $18 @u32 = const 8
	push 8
	# $19 @u64 = call calling_convention($04, $06, $08, $10, $12, $14, $16, $18)
	call calling_convention
	add rsp, 16
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
