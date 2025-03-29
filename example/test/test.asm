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
	enter 48, 0
	# $06 @bool = call ok(5)
	call ok
	# $07 @u32 = cast @bool $06
	# TODO: Not implemented yet.
	# $08 @u32 = mul @u32 1, $07
	mov eax, 1
	imul eax, DWORD PTR [rbp - 8]
	mov DWORD PTR [rbp - 12], eax
	# $11 @u32 = add @u32 $08, 1
	mov eax, DWORD PTR [rbp - 12]
	add eax, 1
	mov DWORD PTR [rbp - 16], eax
	# $12 @s32 = cast @u32 $11
	# TODO: Not implemented yet.
	# $15 @f32 = call test1($12, 10.5)
	call test1
	# $17 @f32 = mul @f32 $15, 2.01
	mulss xmm0, DWORD PTR [float0]
	movss DWORD PTR [rbp - 24], xmm0
	# $20 @f32 = sub @f32 $17, 42
	movss xmm0, DWORD PTR [rbp - 24]
	subss xmm0, DWORD PTR [float1]
	movss DWORD PTR [rbp - 28], xmm0
	# $21 @u64 = cast @f32 $20
	# TODO: Not implemented yet.
	# store @u64 $21, %01
	mov rax, QWORD PTR [rbp - 32]
	mov QWORD PTR [rbp - 48], rax
	# $22 @u64 = load %01
	mov rax, QWORD PTR [rbp - 48]
	mov QWORD PTR [rbp - 40], rax
	# ret $22
	mov rax, QWORD PTR [rbp - 40]
	leave
	ret

ok:
	enter 48, 0
	# store @f64 foo1, %02
	movsd QWORD PTR [rbp - 41], xmm0
	# $03 @f64 = load %02
	movsd xmm0, QWORD PTR [rbp - 41]
	movsd QWORD PTR [rbp - 8], xmm0
	# $06 @bool = gth @f64 $03, 5
	movsd xmm0, QWORD PTR [rbp - 8]
	ucomisd xmm0, QWORD PTR [float2]
	seta BYTE PTR [rbp - 16]
	# if $06 then L4 else L5
	mov al, BYTE PTR [rbp - 16]
	test al, al
	jnz L4
	jmp L5
L5:
	# $09 @f64 = load %02
	movsd xmm0, QWORD PTR [rbp - 41]
	movsd QWORD PTR [rbp - 17], xmm0
	# $12 @bool = gth @f64 $09, 10
	movsd xmm0, QWORD PTR [rbp - 17]
	ucomisd xmm0, QWORD PTR [float3]
	seta BYTE PTR [rbp - 25]
	# if $12 then L7 else L8
	mov al, BYTE PTR [rbp - 25]
	test al, al
	jnz L7
	jmp L8
L8:
	# store @f64 21, %02
	movsd xmm0, QWORD PTR [float4]
	movsd QWORD PTR [rbp - 41], xmm0
	# goto L6
	jmp L6
L7:
	# store @f64 20, %02
	movsd xmm0, QWORD PTR [float5]
	movsd QWORD PTR [rbp - 41], xmm0
	# goto L6
	jmp L6
L4:
	# store @f64 0, %02
	movsd xmm0, QWORD PTR [float6]
	movsd QWORD PTR [rbp - 41], xmm0
	# goto L6
	jmp L6
L6:
	# $19 @s32 = div @s32 4, 2
	mov eax, 4
	mov ecx, 2
	idiv ecx
	mov DWORD PTR [rbp - 26], eax
	# $20 @f64 = load %02
	movsd xmm0, QWORD PTR [rbp - 41]
	movsd QWORD PTR [rbp - 30], xmm0
	# $21 @f32 = call test2($19, $20)
	call test2
	# $22 @bool = cast @f32 $21
	xorps xmm1, xmm1
	ucomiss xmm0, xmm1
	setne al
	setp cl
	or al, cl
	mov BYTE PTR [rbp - 38], al
	# store @bool $22, %01
	mov al, BYTE PTR [rbp - 38]
	mov BYTE PTR [rbp - 40], al
	# $23 @bool = load %01
	mov al, BYTE PTR [rbp - 40]
	mov BYTE PTR [rbp - 39], al
	# ret $23
	mov al, BYTE PTR [rbp - 39]
	leave
	ret

test1:
	enter 144, 0
	# store @s32 foo2, %02
	mov DWORD PTR [rbp - 126], edi
	# store @f64 bar, %03
	movsd QWORD PTR [rbp - 130], xmm0
	# $04 @s32 = load %02
	mov eax, DWORD PTR [rbp - 126]
	mov DWORD PTR [rbp - 8], eax
	# $05 @f64 = cast @s32 $04
	# TODO: Not implemented yet.
	# $06 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 130]
	movsd QWORD PTR [rbp - 20], xmm0
	# $07 @bool = lth @f64 $05, $06
	movsd xmm0, QWORD PTR [rbp - 12]
	ucomisd xmm0, QWORD PTR [rbp - 20]
	setb BYTE PTR [rbp - 28]
	# if $07 then L12 else L13
	mov al, BYTE PTR [rbp - 28]
	test al, al
	jnz L12
	jmp L13
L13:
	# $18 @s32 = load %02
	mov eax, DWORD PTR [rbp - 126]
	mov DWORD PTR [rbp - 86], eax
	# $19 @f64 = cast @s32 $18
	# TODO: Not implemented yet.
	# $20 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 130]
	movsd QWORD PTR [rbp - 98], xmm0
	# $21 @f64 = mul @f64 $19, $20
	movsd xmm0, QWORD PTR [rbp - 90]
	mulsd xmm0, QWORD PTR [rbp - 98]
	movsd QWORD PTR [rbp - 106], xmm0
	# $22 @f32 = cast @f64 $21
	movsd xmm0, QWORD PTR [rbp - 106]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rbp - 114], xmm0
	# store @f32 $22, %01
	movss xmm0, DWORD PTR [rbp - 114]
	movss DWORD PTR [rbp - 122], xmm0
	# goto L11
	jmp L11
L12:
	# $08 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 130]
	movsd QWORD PTR [rbp - 29], xmm0
	# $09 @s32 = load %02
	mov eax, DWORD PTR [rbp - 126]
	mov DWORD PTR [rbp - 37], eax
	# $10 @f64 = cast @s32 $09
	# TODO: Not implemented yet.
	# $11 @f64 = mul @f64 $08, $10
	movsd xmm0, QWORD PTR [rbp - 29]
	mulsd xmm0, QWORD PTR [rbp - 41]
	movsd QWORD PTR [rbp - 49], xmm0
	# $12 @s32 = load %02
	mov eax, DWORD PTR [rbp - 126]
	mov DWORD PTR [rbp - 57], eax
	# $13 @s32 = load %02
	mov eax, DWORD PTR [rbp - 126]
	mov DWORD PTR [rbp - 61], eax
	# $14 @bool = lth @s32 $12, $13
	mov eax, DWORD PTR [rbp - 57]
	cmp eax, DWORD PTR [rbp - 61]
	setl BYTE PTR [rbp - 65]
	# $15 @f64 = cast @bool $14
	# TODO: Not implemented yet.
	# $16 @f64 = add @f64 $11, $15
	movsd xmm0, QWORD PTR [rbp - 49]
	addsd xmm0, QWORD PTR [rbp - 66]
	movsd QWORD PTR [rbp - 74], xmm0
	# $17 @f32 = cast @f64 $16
	movsd xmm0, QWORD PTR [rbp - 74]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rbp - 82], xmm0
	# store @f32 $17, %01
	movss xmm0, DWORD PTR [rbp - 82]
	movss DWORD PTR [rbp - 122], xmm0
	# goto L11
	jmp L11
L11:
	# $23 @f32 = load %01
	movss xmm0, DWORD PTR [rbp - 122]
	movss DWORD PTR [rbp - 118], xmm0
	# ret $23
	movss xmm0, DWORD PTR [rbp - 118]
	leave
	ret

test2:
	# store @s32 foo2, %02
	mov DWORD PTR [rbp - 109], edi
	# store @f64 bar, %03
	movsd QWORD PTR [rbp - 113], xmm0
	# $04 @s32 = load %02
	mov eax, DWORD PTR [rbp - 109]
	mov DWORD PTR [rbp - 8], eax
	# $05 @f64 = cast @s32 $04
	# TODO: Not implemented yet.
	# $06 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 113]
	movsd QWORD PTR [rbp - 20], xmm0
	# $07 @bool = lth @f64 $05, $06
	movsd xmm0, QWORD PTR [rbp - 12]
	ucomisd xmm0, QWORD PTR [rbp - 20]
	setb BYTE PTR [rbp - 28]
	# if $07 then L17 else L18
	mov al, BYTE PTR [rbp - 28]
	test al, al
	jnz L17
	jmp L18
L18:
	# $13 @s32 = load %02
	mov eax, DWORD PTR [rbp - 109]
	mov DWORD PTR [rbp - 61], eax
	# $14 @f64 = cast @s32 $13
	# TODO: Not implemented yet.
	# $15 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 113]
	movsd QWORD PTR [rbp - 73], xmm0
	# $16 @f64 = mul @f64 $14, $15
	movsd xmm0, QWORD PTR [rbp - 65]
	mulsd xmm0, QWORD PTR [rbp - 73]
	movsd QWORD PTR [rbp - 81], xmm0
	# store @f64 $16, %03
	movsd xmm0, QWORD PTR [rbp - 81]
	movsd QWORD PTR [rbp - 113], xmm0
	# $17 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 113]
	movsd QWORD PTR [rbp - 89], xmm0
	# $18 @f32 = cast @f64 $17
	movsd xmm0, QWORD PTR [rbp - 89]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rbp - 97], xmm0
	# store @f32 $18, %01
	movss xmm0, DWORD PTR [rbp - 97]
	movss DWORD PTR [rbp - 105], xmm0
	# goto L16
	jmp L16
L17:
	# $08 @f64 = load %03
	movsd xmm0, QWORD PTR [rbp - 113]
	movsd QWORD PTR [rbp - 29], xmm0
	# $09 @s32 = load %02
	mov eax, DWORD PTR [rbp - 109]
	mov DWORD PTR [rbp - 37], eax
	# $10 @f64 = cast @s32 $09
	# TODO: Not implemented yet.
	# $11 @f64 = mul @f64 $08, $10
	movsd xmm0, QWORD PTR [rbp - 29]
	mulsd xmm0, QWORD PTR [rbp - 41]
	movsd QWORD PTR [rbp - 49], xmm0
	# $12 @f32 = cast @f64 $11
	movsd xmm0, QWORD PTR [rbp - 49]
	cvtsd2ss xmm0, xmm0
	movss DWORD PTR [rbp - 57], xmm0
	# store @f32 $12, %01
	movss xmm0, DWORD PTR [rbp - 57]
	movss DWORD PTR [rbp - 105], xmm0
	# goto L16
	jmp L16
L16:
	# $19 @f32 = load %01
	movss xmm0, DWORD PTR [rbp - 105]
	movss DWORD PTR [rbp - 101], xmm0
	# ret $19
	movss xmm0, DWORD PTR [rbp - 101]
	ret

.section .data
	float0: .float	2.01
	float1: .float	42
	float2: .double	5
	float3: .double	10
	float4: .double	21
	float5: .double	20
	float6: .double	0
