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
	# %01 @u64 = alloca
	# $06 @bool = call ok(5)
	movsd xmm0, QWORD PTR [float0]
	call ok
	# $07 @u32 = cast @bool $06
	movzx r10d, al
	mov ebx, r10d
	# $08 @u32 = mul @u32 1, $07
	mov r10d, 1
	imul r10d, ebx
	mov ebx, r10d
	# $11 @u32 = add @u32 $08, 1
	add ebx, 1
	# $12 @s32 = cast @u32 $11
	# $15 @f32 = call test1($12, 10.5)
	mov edi, ebx
	movsd xmm0, QWORD PTR [float1]
	call test1
	# $17 @f32 = mul @f32 $15, 2.01
	mulss xmm0, DWORD PTR [float2]
	movss xmm8, xmm0
	# $20 @f32 = sub @f32 $17, 42
	subss xmm8, DWORD PTR [float3]
	# $21 @u64 = cast @f32 $20
	cvttss2si r10, xmm8
	mov rbx, r10
	# store @u64 $21, %01
	mov QWORD PTR [rbp - 8], rbx
	# $22 @u64 = load %01
	mov rax, QWORD PTR [rbp - 8]
	# ret $22
	leave
	ret

ok:
	enter 16, 0
	# %01 @bool = alloca
	# %02 @f64 = alloca
	# store @f64 foo1, %02
	movsd QWORD PTR [rbp - 9], xmm0
	# $03 @f64 = load %02
	movsd xmm8, QWORD PTR [rbp - 9]
	# $06 @bool = gth @f64 $03, 5
	ucomisd xmm8, QWORD PTR [float4]
	seta bl
	# if $06 then L4 else L5
	test bl, bl
	jnz L4
	jmp L5
L5:
	# $09 @f64 = load %02
	movsd xmm8, QWORD PTR [rbp - 9]
	# $12 @bool = gth @f64 $09, 10
	ucomisd xmm8, QWORD PTR [float5]
	seta bl
	# if $12 then L7 else L8
	test bl, bl
	jnz L7
	jmp L8
L8:
	# store @f64 21, %02
	movsd xmm10, QWORD PTR [float6]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L7:
	# store @f64 20, %02
	movsd xmm10, QWORD PTR [float7]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L4:
	# store @f64 0, %02
	movsd xmm10, QWORD PTR [float8]
	movsd QWORD PTR [rbp - 9], xmm10
	# goto L6
	jmp L6
L6:
	# $19 @s32 = div @s32 4, 2
	mov eax, 4
	mov r11d, 2
	idiv r11d
	mov ebx, eax
	# $20 @f64 = load %02
	movsd xmm8, QWORD PTR [rbp - 9]
	# $21 @f32 = call test2($19, $20)
	mov edi, ebx
	movsd xmm0, xmm8
	call test2
	# $22 @bool = cast @f32 $21
	xorps xmm11, xmm11
	ucomiss xmm11, xmm0
	setne r10b
	setp r11b
	or r10b, r11b
	mov bl, r10b
	# store @bool $22, %01
	mov BYTE PTR [rbp - 1], bl
	# $23 @bool = load %01
	mov al, BYTE PTR [rbp - 1]
	# ret $23
	leave
	ret

test1:
	# %01 @f32 = alloca
	# %02 @s32 = alloca
	# %03 @f64 = alloca
	# store @s32 foo2, %02
	mov DWORD PTR [rsp - 8], edi
	# store @f64 bar, %03
	movsd QWORD PTR [rsp - 16], xmm0
	# %04 @f32 = alloca
	# store @f32 0, %04
	movss xmm10, DWORD PTR [float9]
	movss DWORD PTR [rsp - 20], xmm10
	# $06 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $07 @f64 = cast @s32 $06
	cvtsi2sd xmm10, ebx
	movsd xmm8, xmm10
	# $08 @f64 = load %03
	movsd xmm9, QWORD PTR [rsp - 16]
	# $09 @bool = lth @f64 $07, $08
	ucomisd xmm8, xmm9
	setb bl
	# if $09 then L12 else L13
	test bl, bl
	jnz L12
	jmp L13
L13:
	# $20 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $21 @f64 = cast @s32 $20
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $22 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $23 @f64 = mul @f64 $21, $22
	mulsd xmm9, xmm8
	movsd xmm8, xmm9
	# $24 @f32 = cast @f64 $23
	cvtsd2ss xmm8, xmm8
	# store @f32 $24, %04
	movss DWORD PTR [rsp - 20], xmm8
	# goto L14
	jmp L14
L12:
	# $10 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $11 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $12 @f64 = cast @s32 $11
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $13 @f64 = mul @f64 $10, $12
	mulsd xmm8, xmm9
	# $14 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $15 @s32 = load %02
	mov r12d, DWORD PTR [rsp - 8]
	# $16 @bool = lth @s32 $14, $15
	cmp ebx, r12d
	setl bl
	# $17 @f64 = cast @bool $16
	movzx r10d, bl
	cvtsi2sd xmm10, r10d
	movsd xmm9, xmm10
	# $18 @f64 = add @f64 $13, $17
	addsd xmm8, xmm9
	# $19 @f32 = cast @f64 $18
	cvtsd2ss xmm8, xmm8
	# store @f32 $19, %04
	movss DWORD PTR [rsp - 20], xmm8
	# goto L14
	jmp L14
L14:
	# $25 @f32 = load %04
	movss xmm8, DWORD PTR [rsp - 20]
	# store @f32 $25, %01
	movss DWORD PTR [rsp - 4], xmm8
	# $26 @f32 = load %01
	movss xmm0, DWORD PTR [rsp - 4]
	# ret $26
	ret

test2:
	# %01 @f32 = alloca
	# %02 @s32 = alloca
	# %03 @f64 = alloca
	# store @s32 foo2, %02
	mov DWORD PTR [rsp - 8], edi
	# store @f64 bar, %03
	movsd QWORD PTR [rsp - 16], xmm0
	# $04 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $05 @f64 = cast @s32 $04
	cvtsi2sd xmm10, ebx
	movsd xmm8, xmm10
	# $06 @f64 = load %03
	movsd xmm9, QWORD PTR [rsp - 16]
	# $07 @bool = lth @f64 $05, $06
	ucomisd xmm8, xmm9
	setb bl
	# if $07 then L17 else L18
	test bl, bl
	jnz L17
	jmp L18
L18:
	# $13 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $14 @f64 = cast @s32 $13
	cvtsi2sd xmm10, ebx
	movsd xmm9, xmm10
	# $15 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $16 @f64 = mul @f64 $14, $15
	mulsd xmm9, xmm8
	movsd xmm8, xmm9
	# store @f64 $16, %03
	movsd QWORD PTR [rsp - 16], xmm8
	# $17 @f64 = load %03
	movsd xmm8, QWORD PTR [rsp - 16]
	# $18 @f32 = cast @f64 $17
	cvtsd2ss xmm8, xmm8
	# store @f32 $18, %01
	movss DWORD PTR [rsp - 4], xmm8
	# goto L16
	jmp L16
L17:
	# $08 @f64 = load %03
	movsd xmm9, QWORD PTR [rsp - 16]
	# $09 @s32 = load %02
	mov ebx, DWORD PTR [rsp - 8]
	# $10 @f64 = cast @s32 $09
	cvtsi2sd xmm10, ebx
	movsd xmm8, xmm10
	# $11 @f64 = mul @f64 $08, $10
	mulsd xmm9, xmm8
	movsd xmm8, xmm9
	# $12 @f32 = cast @f64 $11
	cvtsd2ss xmm8, xmm8
	# store @f32 $12, %01
	movss DWORD PTR [rsp - 4], xmm8
	# goto L16
	jmp L16
L16:
	# $19 @f32 = load %01
	movss xmm0, DWORD PTR [rsp - 4]
	# ret $19
	ret

.section .data
	float0: .double	5.000000
	float1: .double	10.500000
	float2: .float	2.010000
	float3: .float	42.000000
	float4: .double	5.000000
	float5: .double	10.000000
	float6: .double	21.000000
	float7: .double	20.000000
	float8: .double	0.000000
	float9: .float	0.000000
