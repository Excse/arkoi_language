.intel_syntax noprefix
.section .text
.global _start

_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:
	enter 48, 0
	# $05 @f64 = const 5
	movsd xmm0, QWORD PTR [float0]
	# $06 @bool = call ok($05)
	call ok
	# $07 @u32 = cast @bool $06
	# $08 @u32 = mul @u32 1, $07
	mov r10d, 1
	imul r10d, DWORD PTR [rbp - 16]
	mov DWORD PTR [rbp - 20], r10d
	# $11 @u32 = add @u32 $08, 1
	mov r10d, DWORD PTR [rbp - 20]
	add r10d, 1
	mov DWORD PTR [rbp - 24], r10d
	# $12 @s32 = cast @u32 $11
	# $14 @f64 = const 10.5
	movsd xmm0, QWORD PTR [float1]
	# $15 @f32 = call test1($12, $14)
	call test1
	# $17 @f32 = mul @f32 $15, 2.01
	movss xmm10, xmm0
	mulss xmm10, DWORD PTR [float2]
	movss DWORD PTR [rbp - 28], xmm10
	# $20 @f32 = sub @f32 $17, 42
	movss xmm10, DWORD PTR [rbp - 28]
	addss xmm10, DWORD PTR [float3]
	movss DWORD PTR [rbp - 32], xmm10
	# $21 @u64 = cast @f32 $20
	# store @u64 $21, $01
	mov r10, QWORD PTR [rbp - 36]
	mov QWORD PTR [rbp - 8], r10
	# $22 @u64 = load $01
	mov r10, QWORD PTR [rbp - 8]
	mov QWORD PTR [rbp - 44], r10
	# ret $22
	mov rax, QWORD PTR [rbp - 44]
	leave
	ret

ok:
	enter 32, 0
	# store @f64 foo1, $02
	movsd QWORD PTR [rbp - 9], xmm0
	# $03 @f64 = load $02
	movsd xmm10, QWORD PTR [rbp - 9]
	movsd QWORD PTR [rbp - 17], xmm10
	# $06 @bool = gth @f64 $03, 5
	movsd xmm10, QWORD PTR [rbp - 17]
	# TODO: Needs to be implemented
	# if $06 then L4 else L5
L5:
	# $09 @f64 = load $02
	movsd xmm10, QWORD PTR [rbp - 9]
	movsd QWORD PTR [rbp - 26], xmm10
	# $12 @bool = gth @f64 $09, 10
	movsd xmm10, QWORD PTR [rbp - 26]
	# TODO: Needs to be implemented
	# if $12 then L7 else L8
L8:
	# store @f64 21, $02
	movsd xmm10, QWORD PTR [float6]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L7:
	# store @f64 20, $02
	movsd xmm10, QWORD PTR [float7]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L4:
	# store @f64 0, $02
	movsd xmm10, QWORD PTR [float8]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L6:
	# $17 @s32 = const 2
	mov edi, 2
	# $18 @f64 = load $02
	movsd xmm0, QWORD PTR [rbp - 9]
	# $19 @f32 = call test2($17, $18)
	call test2
	# $20 @bool = cast @f32 $19
	# store @bool $20, $01
	mov r10b, BYTE PTR [rbp - 35]
	mov BYTE PTR [rbp - 8], r10b
	# $21 @bool = load $01
	mov r10b, BYTE PTR [rbp - 8]
	mov BYTE PTR [rbp - 36], r10b
	# ret $21
	mov al, BYTE PTR [rbp - 36]
	leave
	ret

test1:
	enter 112, 0
	# store @s32 foo2, $02
	mov DWORD PTR [rbp - 12], edi
	# store @f64 bar, $03
	movsd QWORD PTR [rbp - 16], xmm0
	# $04 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 24], r10d
	# $05 @f64 = cast @s32 $04
	# $06 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 36], xmm10
	# $07 @bool = lth @f64 $05, $06
	movsd xmm10, QWORD PTR [rbp - 28]
	# TODO: Needs to be implemented
	# if $07 then L12 else L13
L13:
	# $13 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 77], r10d
	# $14 @f64 = cast @s32 $13
	# $15 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 89], xmm10
	# $16 @f64 = mul @f64 $14, $15
	movsd xmm10, QWORD PTR [rbp - 81]
	mulsd xmm10, QWORD PTR [rbp - 89]
	movsd QWORD PTR [rbp - 97], xmm10
	# $17 @f32 = cast @f64 $16
	# store @f32 $17, $01
	movss xmm10, DWORD PTR [rbp - 105]
	movss DWORD PTR [rbp - 8], xmm10
	# goto L11
	jmp L11
L12:
	# $08 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 45], xmm10
	# $09 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 53], r10d
	# $10 @f64 = cast @s32 $09
	# $11 @f64 = mul @f64 $08, $10
	movsd xmm10, QWORD PTR [rbp - 45]
	mulsd xmm10, QWORD PTR [rbp - 57]
	movsd QWORD PTR [rbp - 65], xmm10
	# $12 @f32 = cast @f64 $11
	# store @f32 $12, $01
	movss xmm10, DWORD PTR [rbp - 73]
	movss DWORD PTR [rbp - 8], xmm10
	# goto L11
	jmp L11
L11:
	# $18 @f32 = load $01
	movss xmm10, DWORD PTR [rbp - 8]
	movss DWORD PTR [rbp - 109], xmm10
	# ret $18
	movss xmm0, DWORD PTR [rbp - 109]
	leave
	ret

test2:
	enter 128, 0
	# store @s32 foo2, $02
	mov DWORD PTR [rbp - 12], edi
	# store @f64 bar, $03
	movsd QWORD PTR [rbp - 16], xmm0
	# $04 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 24], r10d
	# $05 @f64 = cast @s32 $04
	# $06 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 36], xmm10
	# $07 @bool = lth @f64 $05, $06
	movsd xmm10, QWORD PTR [rbp - 28]
	# TODO: Needs to be implemented
	# if $07 then L17 else L18
L18:
	# $13 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 77], r10d
	# $14 @f64 = cast @s32 $13
	# $15 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 89], xmm10
	# $16 @f64 = mul @f64 $14, $15
	movsd xmm10, QWORD PTR [rbp - 81]
	mulsd xmm10, QWORD PTR [rbp - 89]
	movsd QWORD PTR [rbp - 97], xmm10
	# store @f64 $16, $03
	movsd xmm10, QWORD PTR [rbp - 97]
	movsd QWORD PTR [rbp - 16], xmm10
	# $17 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 105], xmm10
	# $18 @f32 = cast @f64 $17
	# store @f32 $18, $01
	movss xmm10, DWORD PTR [rbp - 113]
	movss DWORD PTR [rbp - 8], xmm10
	# goto L16
	jmp L16
L17:
	# $08 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 16]
	movsd QWORD PTR [rbp - 45], xmm10
	# $09 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 53], r10d
	# $10 @f64 = cast @s32 $09
	# $11 @f64 = mul @f64 $08, $10
	movsd xmm10, QWORD PTR [rbp - 45]
	mulsd xmm10, QWORD PTR [rbp - 57]
	movsd QWORD PTR [rbp - 65], xmm10
	# $12 @f32 = cast @f64 $11
	# store @f32 $12, $01
	movss xmm10, DWORD PTR [rbp - 73]
	movss DWORD PTR [rbp - 8], xmm10
	# goto L16
	jmp L16
L16:
	# $19 @f32 = load $01
	movss xmm10, DWORD PTR [rbp - 8]
	movss DWORD PTR [rbp - 117], xmm10
	# ret $19
	movss xmm0, DWORD PTR [rbp - 117]
	leave
	ret

.section .data
	float0: .double	5
	float1: .double	10.5
	float2: .float	2.01
	float3: .float	42
	float4: .double	5
	float5: .double	10
	float6: .double	21
	float7: .double	20
	float8: .double	0
