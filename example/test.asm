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
	# $01 @u64 = alloca
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
	# $22 @f64 = const 5
	movsd xmm0, QWORD PTR [float0]
	# $23 @bool = call ok($22)
	call ok
	# $24 @u32 = cast @bool $23
	# $25 @u32 = mul @u32 1, $24
	# $28 @u32 = add @u32 $25, 1
	# $29 @s32 = cast @u32 $28
	# $31 @f64 = const 10.5
	movsd xmm0, QWORD PTR [float1]
	# $32 @f32 = call test1($29, $31)
	call test1
	# $34 @f32 = mul @f32 $32, 2.01
	# $37 @f32 = sub @f32 $34, 42
	# $38 @u64 = cast @f32 $37
	# store @u64 $38, $01
	mov r10, QWORD PTR [rbp - 16]
	mov QWORD PTR [rbp - 44], r10
	# $39 @u64 = load $01
	mov r10, QWORD PTR [rbp - 44]
	mov QWORD PTR [rbp - 8], r10
	# ret $39
	mov rax, QWORD PTR [rbp - 8]
	leave
	ret

ok:
	enter 32, 0
	# $01 @bool = alloca
	# $02 @f64 = alloca
	# store @f64 foo1, $02
	movsd QWORD PTR [rbp - 28], xmm0
	# $03 @f64 = load $02
	movsd xmm10, QWORD PTR [rbp - 28]
	movsd QWORD PTR [rbp - 20], xmm10
	# $06 @bool = gth @f64 $03, 5
	# if $06 then L4 else L5
L5:
	# $09 @f64 = load $02
	movsd xmm10, QWORD PTR [rbp - 28]
	movsd QWORD PTR [rbp - 10], xmm10
	# $12 @bool = gth @f64 $09, 10
	# if $12 then L7 else L8
L8:
	# store @f64 21, $02
	movsd xmm10, QWORD PTR [float2]
	movsd QWORD PTR [rbp - 28], xmm10
	# goto L6
	jmp L6
L7:
	# store @f64 20, $02
	movsd xmm10, QWORD PTR [float3]
	movsd QWORD PTR [rbp - 28], xmm10
	# goto L6
	jmp L6
L4:
	# store @f64 0, $02
	movsd xmm10, QWORD PTR [float4]
	movsd QWORD PTR [rbp - 28], xmm10
	# goto L6
	jmp L6
L6:
	# $17 @s32 = const 2
	mov edi, 2
	# $18 @f64 = load $02
	movsd xmm0, QWORD PTR [rbp - 28]
	# $19 @f32 = call test2($17, $18)
	call test2
	# $20 @bool = cast @f32 $19
	# store @bool $20, $01
	mov r10b, BYTE PTR [rbp - 9]
	mov BYTE PTR [rbp - 36], r10b
	# $21 @bool = load $01
	mov r10b, BYTE PTR [rbp - 36]
	mov BYTE PTR [rbp - 8], r10b
	# ret $21
	mov al, BYTE PTR [rbp - 8]
	leave
	ret

test1:
	enter 112, 0
	# $01 @f32 = alloca
	# $02 @s32 = alloca
	# $03 @f64 = alloca
	# store @s32 foo2, $02
	mov DWORD PTR [rbp - 40], edi
	# store @f64 bar, $03
	movsd QWORD PTR [rbp - 52], xmm0
	# $04 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 40]
	mov DWORD PTR [rbp - 60], r10d
	# $05 @f64 = cast @s32 $04
	# $06 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 52]
	movsd QWORD PTR [rbp - 76], xmm10
	# $07 @bool = lth @f64 $05, $06
	# if $07 then L12 else L13
L13:
	# $13 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 40]
	mov DWORD PTR [rbp - 93], r10d
	# $14 @f64 = cast @s32 $13
	# $15 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 52]
	movsd QWORD PTR [rbp - 44], xmm10
	# $16 @f64 = mul @f64 $14, $15
	# $17 @f32 = cast @f64 $16
	# store @f32 $17, $01
	movss xmm10, DWORD PTR [rbp - 64]
	movss DWORD PTR [rbp - 36], xmm10
	# goto L11
	jmp L11
L12:
	# $08 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 52]
	movsd QWORD PTR [rbp - 105], xmm10
	# $09 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 40]
	mov DWORD PTR [rbp - 32], r10d
	# $10 @f64 = cast @s32 $09
	# $11 @f64 = mul @f64 $08, $10
	# $12 @f32 = cast @f64 $11
	# store @f32 $12, $01
	movss xmm10, DWORD PTR [rbp - 12]
	movss DWORD PTR [rbp - 36], xmm10
	# goto L11
	jmp L11
L11:
	# $18 @f32 = load $01
	movss xmm10, DWORD PTR [rbp - 36]
	movss DWORD PTR [rbp - 8], xmm10
	# ret $18
	movss xmm0, DWORD PTR [rbp - 8]
	leave
	ret

test2:
	enter 128, 0
	# $01 @f32 = alloca
	# $02 @s32 = alloca
	# $03 @f64 = alloca
	# store @s32 foo2, $02
	mov DWORD PTR [rbp - 52], edi
	# store @f64 bar, $03
	movsd QWORD PTR [rbp - 64], xmm0
	# $04 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 52]
	mov DWORD PTR [rbp - 72], r10d
	# $05 @f64 = cast @s32 $04
	# $06 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 64]
	movsd QWORD PTR [rbp - 92], xmm10
	# $07 @bool = lth @f64 $05, $06
	# if $07 then L17 else L18
L18:
	# $13 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 52]
	mov DWORD PTR [rbp - 109], r10d
	# $14 @f64 = cast @s32 $13
	# $15 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 64]
	movsd QWORD PTR [rbp - 56], xmm10
	# $16 @f64 = mul @f64 $14, $15
	# store @f64 $16, $03
	movsd xmm10, QWORD PTR [rbp - 101]
	movsd QWORD PTR [rbp - 64], xmm10
	# $17 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 64]
	movsd QWORD PTR [rbp - 76], xmm10
	# $18 @f32 = cast @f64 $17
	# store @f32 $18, $01
	movss xmm10, DWORD PTR [rbp - 48]
	movss DWORD PTR [rbp - 44], xmm10
	# goto L16
	jmp L16
L17:
	# $08 @f64 = load $03
	movsd xmm10, QWORD PTR [rbp - 64]
	movsd QWORD PTR [rbp - 36], xmm10
	# $09 @s32 = load $02
	mov r10d, DWORD PTR [rbp - 52]
	mov DWORD PTR [rbp - 32], r10d
	# $10 @f64 = cast @s32 $09
	# $11 @f64 = mul @f64 $08, $10
	# $12 @f32 = cast @f64 $11
	# store @f32 $12, $01
	movss xmm10, DWORD PTR [rbp - 12]
	movss DWORD PTR [rbp - 44], xmm10
	# goto L16
	jmp L16
L16:
	# $19 @f32 = load $01
	movss xmm10, DWORD PTR [rbp - 44]
	movss DWORD PTR [rbp - 8], xmm10
	# ret $19
	movss xmm0, DWORD PTR [rbp - 8]
	leave
	ret

calling_convention:
	enter 48, 0
	# $01 @bool = alloca
	# $02 @u32 = alloca
	# $03 @u32 = alloca
	# $04 @u32 = alloca
	# $05 @u32 = alloca
	# $06 @u32 = alloca
	# $07 @u32 = alloca
	# $08 @u32 = alloca
	# $09 @u32 = alloca
	# store @u32 a, $02
	mov DWORD PTR [rbp - 37], edi
	# store @u32 b, $03
	mov DWORD PTR [rbp - 33], esi
	# store @u32 c, $04
	mov DWORD PTR [rbp - 29], edx
	# store @u32 d, $05
	mov DWORD PTR [rbp - 25], ecx
	# store @u32 e, $06
	mov DWORD PTR [rbp - 21], r8d
	# store @u32 f, $07
	mov DWORD PTR [rbp - 17], r9d
	# store @u32 g, $08
	mov r10d, DWORD PTR [rbp + 16]
	mov DWORD PTR [rbp - 13], r10d
	# store @u32 h, $09
	mov r10d, DWORD PTR [rbp + 24]
	mov DWORD PTR [rbp - 9], r10d
	# store @bool 1, $01
	mov BYTE PTR [rbp - 41], 1
	# $11 @bool = load $01
	mov r10b, BYTE PTR [rbp - 41]
	mov BYTE PTR [rbp - 8], r10b
	# ret $11
	mov al, BYTE PTR [rbp - 8]
	leave
	ret

.section .data
	float0: .double	5
	float1: .double	10.5
	float2: .double	21
	float3: .double	20
	float4: .double	0
