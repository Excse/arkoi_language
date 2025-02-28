.intel_syntax noprefix
.section .text
.global _start

_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:
	enter 32, 0
	# $03 @u32 = const 1
	mov edi, 1
	# $05 @u32 = const 2
	mov esi, 2
	# $07 @u32 = const 3
	mov edx, 3
	# $09 @u32 = const 4
	mov ecx, 4
	# $11 @u32 = const 5
	mov r8d, 5
	# $13 @u32 = const 6
	mov r9d, 6
	# $15 @u32 = const 7
	mov DWORD PTR [rsp - 16], 7
	# $17 @u32 = const 8
	mov DWORD PTR [rsp - 8], 8
	# $18 @bool = call calling_convention($03, $05, $07, $09, $11, $13, $15, $17)
	sub rsp, 16
	call calling_convention
	add rsp, 16
	# $19 @u64 = cast @bool $18
	# store @u64 $19, $01
	mov r10, QWORD PTR [rbp - 16]
	mov QWORD PTR [rbp - 8], r10
	# $20 @u64 = load $01
	mov r10, QWORD PTR [rbp - 8]
	mov QWORD PTR [rbp - 24], r10
	# ret $20
	mov rax, QWORD PTR [rbp - 24]
	leave
	ret

calling_convention:
	enter 48, 0
	# store @u32 a, $02
	mov DWORD PTR [rbp - 9], edi
	# store @u32 b, $03
	mov DWORD PTR [rbp - 13], esi
	# store @u32 c, $04
	mov DWORD PTR [rbp - 17], edx
	# store @u32 d, $05
	mov DWORD PTR [rbp - 21], ecx
	# store @u32 e, $06
	mov DWORD PTR [rbp - 25], r8d
	# store @u32 f, $07
	mov DWORD PTR [rbp - 29], r9d
	# store @u32 g, $08
	mov r10d, DWORD PTR [rbp + 16]
	mov DWORD PTR [rbp - 33], r10d
	# store @u32 h, $09
	mov r10d, DWORD PTR [rbp + 24]
	mov DWORD PTR [rbp - 37], r10d
	# store @bool 1, $01
	mov BYTE PTR [rbp - 8], 1
	# $11 @bool = load $01
	mov r10b, BYTE PTR [rbp - 8]
	mov BYTE PTR [rbp - 41], r10b
	# ret $11
	mov al, BYTE PTR [rbp - 41]
	leave
	ret

.section .data
