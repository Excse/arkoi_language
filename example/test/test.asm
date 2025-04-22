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
	# $06 @bool = call ok(5)
	movsd xmm0, QWORD PTR [float0]
	call ok
	# $07 @u32 = cast @bool $06
	movzx eax, al
	mov DWORD PTR [rbp - 12], eax
	# $08 @u32 = mul @u32 1, $07
	mov eax, 1
	imul eax, DWORD PTR [rbp - 12]
	mov DWORD PTR [rbp - 16], eax
	# $11 @u32 = add @u32 $08, 1
	mov eax, DWORD PTR [rbp - 16]
	add eax, 1
	mov DWORD PTR [rbp - 20], eax
	# $12 @s32 = cast @u32 $11
	mov eax, DWORD PTR [rbp - 20]
	mov DWORD PTR [rbp - 24], eax
	# $15 @f32 = call test1($12, 10.5)
	mov edi, DWORD PTR [rbp - 24]
	movsd xmm0, QWORD PTR [float1]
	call test1
	# $17 @f32 = mul @f32 $15, 2.01
	mulss xmm0, DWORD PTR [float2]
	movss DWORD PTR [rbp - 28], xmm0
	# $20 @f32 = sub @f32 $17, 42
	movss xmm0, DWORD PTR [rbp - 28]
	subss xmm0, DWORD PTR [float3]
	movss DWORD PTR [rbp - 32], xmm0
	# $21 @u64 = cast @f32 $20
	cvttss2si rax, DWORD PTR [rbp - 32]
	mov QWORD PTR [rbp - 40], rax
	# store @u64 $21, %01
	mov rax, QWORD PTR [rbp - 40]
	mov QWORD PTR [rbp - 8], rax
	# $22 @u64 = load %01
	mov rax, QWORD PTR [rbp - 8]
	# ret $22
	leave
	ret

ok:
	enter 48, 0
	# store @f64 foo1, %02
	movsd QWORD PTR [rbp - 9], xmm0
	# $03 @f64 = load %02
	movsd xmm0, QWORD PTR [rbp - 9]
	movsd QWORD PTR [rbp - 17], xmm0
	# $06 @bool = gth @f64 $03, 5
	movsd xmm0, QWORD PTR [rbp - 17]
	ucomisd xmm0, QWORD PTR [float4]
	seta BYTE PTR [rbp - 18]
	# if $06 then L4 else L5
	mov al, BYTE PTR [rbp - 18]
	test al, al
	jnz L4
	jmp L5
L5:
	# $09 @f64 = load %02
	movsd xmm0, QWORD PTR [rbp - 9]
	movsd QWORD PTR [rbp - 26], xmm0
	# $12 @bool = gth @f64 $09, 10
	movsd xmm0, QWORD PTR [rbp - 26]
	ucomisd xmm0, QWORD PTR [float5]
	seta BYTE PTR [rbp - 27]
	# if $12 then L7 else L8
	mov al, BYTE PTR [rbp - 27]
	test al, al
	jnz L7
	jmp L8
L8:
	# store @f64 21, %02
	movsd xmm0, QWORD PTR [float6]
	movsd QWORD PTR [rbp - 9], xmm0
	# goto L6
	jmp L6
L7:
	# store @f64 20, %02
	movsd xmm0, QWORD PTR [float7]
	movsd QWORD PTR [rbp - 9], xmm0
	# goto L6
	jmp L6
L4:
	# store @f64 0, %02
	movsd xmm0, QWORD PTR [float8]
	movsd QWORD PTR [rbp - 9], xmm0
	# goto L6
	jmp L6
L6:
	# $19 @s32 = div @s32 4, 2
	mov eax, 4
	mov ecx, 2
	idiv ecx
	mov DWORD PTR [rbp - 31], eax
	# $20 @f64 = load %02
	movsd xmm0, QWORD PTR [rbp - 9]
	movsd QWORD PTR [rbp - 39], xmm0
	# $21 @f32 = call test2($19, $20)
	mov edi, DWORD PTR [rbp - 31]
	movsd xmm0, QWORD PTR [rbp - 39]
	call test2
	# $22 @bool = cast @f32 $21
	xorps xmm1, xmm1
	ucomiss xmm1, xmm0
	setne al
	setp cl
	or al, cl
	mov BYTE PTR [rbp - 40], al
	# store @bool $22, %01
	mov al, BYTE PTR [rbp - 40]
	mov BYTE PTR [rbp - 1], al
	# $23 @bool = load %01
	mov al, BYTE PTR [rbp - 1]
	# ret $23
	leave
	ret

test1:
	# store @s32 foo2, %02
	mov DWORD PTR [rsp - 8], edi
	# store @f64 bar, %03
	movsd QWORD PTR [rsp - 16], xmm0
	# $04 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 20], eax
	# $05 @f64 = cast @s32 $04
	cvtsi2sd xmm0, DWORD PTR [rsp - 20]
	movsd QWORD PTR [rsp - 28], xmm0
	# $06 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 36], xmm0
	# $07 @bool = lth @f64 $05, $06
	movsd xmm0, QWORD PTR [rsp - 28]
	ucomisd xmm0, QWORD PTR [rsp - 36]
	setb BYTE PTR [rsp - 37]
	# if $07 then L12 else L13
	mov al, BYTE PTR [rsp - 37]
	test al, al
	jnz L12
	jmp L13
L13:
	# $18 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 41], eax
	# $19 @f64 = cast @s32 $18
	cvtsi2sd xmm0, DWORD PTR [rsp - 41]
	movsd QWORD PTR [rsp - 49], xmm0
	# $20 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 57], xmm0
	# $21 @f64 = mul @f64 $19, $20
	movsd xmm0, QWORD PTR [rsp - 49]
	mulsd xmm0, QWORD PTR [rsp - 57]
	movsd QWORD PTR [rsp - 65], xmm0
	# $22 @f32 = cast @f64 $21
	movsd xmm0, QWORD PTR [rsp - 65]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rsp - 69], xmm0
	# store @f32 $22, %01
	movss xmm0, DWORD PTR [rsp - 69]
	movss DWORD PTR [rsp - 4], xmm0
	# goto L11
	jmp L11
L12:
	# $08 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 77], xmm0
	# $09 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 81], eax
	# $10 @f64 = cast @s32 $09
	cvtsi2sd xmm0, DWORD PTR [rsp - 81]
	movsd QWORD PTR [rsp - 89], xmm0
	# $11 @f64 = mul @f64 $08, $10
	movsd xmm0, QWORD PTR [rsp - 77]
	mulsd xmm0, QWORD PTR [rsp - 89]
	movsd QWORD PTR [rsp - 97], xmm0
	# $12 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 101], eax
	# $13 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 105], eax
	# $14 @bool = lth @s32 $12, $13
	mov eax, DWORD PTR [rsp - 101]
	cmp eax, DWORD PTR [rsp - 105]
	setl BYTE PTR [rsp - 106]
	# $15 @f64 = cast @bool $14
	movzx eax, BYTE PTR [rsp - 106]
	cvtsi2sd xmm0, eax
	movsd QWORD PTR [rsp - 114], xmm0
	# $16 @f64 = add @f64 $11, $15
	movsd xmm0, QWORD PTR [rsp - 97]
	addsd xmm0, QWORD PTR [rsp - 114]
	movsd QWORD PTR [rsp - 122], xmm0
	# $17 @f32 = cast @f64 $16
	movsd xmm0, QWORD PTR [rsp - 122]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rsp - 126], xmm0
	# store @f32 $17, %01
	movss xmm0, DWORD PTR [rsp - 126]
	movss DWORD PTR [rsp - 4], xmm0
	# goto L11
	jmp L11
L11:
	# $23 @f32 = load %01
	movss xmm0, DWORD PTR [rsp - 4]
	# ret $23
	ret

test2:
	# store @s32 foo2, %02
	mov DWORD PTR [rsp - 8], edi
	# store @f64 bar, %03
	movsd QWORD PTR [rsp - 16], xmm0
	# $04 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 20], eax
	# $05 @f64 = cast @s32 $04
	cvtsi2sd xmm0, DWORD PTR [rsp - 20]
	movsd QWORD PTR [rsp - 28], xmm0
	# $06 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 36], xmm0
	# $07 @bool = lth @f64 $05, $06
	movsd xmm0, QWORD PTR [rsp - 28]
	ucomisd xmm0, QWORD PTR [rsp - 36]
	setb BYTE PTR [rsp - 37]
	# if $07 then L17 else L18
	mov al, BYTE PTR [rsp - 37]
	test al, al
	jnz L17
	jmp L18
L18:
	# $13 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 41], eax
	# $14 @f64 = cast @s32 $13
	cvtsi2sd xmm0, DWORD PTR [rsp - 41]
	movsd QWORD PTR [rsp - 49], xmm0
	# $15 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 57], xmm0
	# $16 @f64 = mul @f64 $14, $15
	movsd xmm0, QWORD PTR [rsp - 49]
	mulsd xmm0, QWORD PTR [rsp - 57]
	movsd QWORD PTR [rsp - 65], xmm0
	# store @f64 $16, %03
	movsd xmm0, QWORD PTR [rsp - 65]
	movsd QWORD PTR [rsp - 16], xmm0
	# $17 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 73], xmm0
	# $18 @f32 = cast @f64 $17
	movsd xmm0, QWORD PTR [rsp - 73]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rsp - 77], xmm0
	# store @f32 $18, %01
	movss xmm0, DWORD PTR [rsp - 77]
	movss DWORD PTR [rsp - 4], xmm0
	# goto L16
	jmp L16
L17:
	# $08 @f64 = load %03
	movsd xmm0, QWORD PTR [rsp - 16]
	movsd QWORD PTR [rsp - 85], xmm0
	# $09 @s32 = load %02
	mov eax, DWORD PTR [rsp - 8]
	mov DWORD PTR [rsp - 89], eax
	# $10 @f64 = cast @s32 $09
	cvtsi2sd xmm0, DWORD PTR [rsp - 89]
	movsd QWORD PTR [rsp - 97], xmm0
	# $11 @f64 = mul @f64 $08, $10
	movsd xmm0, QWORD PTR [rsp - 85]
	mulsd xmm0, QWORD PTR [rsp - 97]
	movsd QWORD PTR [rsp - 105], xmm0
	# $12 @f32 = cast @f64 $11
	movsd xmm0, QWORD PTR [rsp - 105]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rsp - 109], xmm0
	# store @f32 $12, %01
	movss xmm0, DWORD PTR [rsp - 109]
	movss DWORD PTR [rsp - 4], xmm0
	# goto L16
	jmp L16
L16:
	# $19 @f32 = load %01
	movss xmm0, DWORD PTR [rsp - 4]
	# ret $19
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
