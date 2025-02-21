.intel_syntax noprefix
.section .text
.global _start

_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall

main:
	push rbp
	mov rbp, rsp
	sub rsp, 44
	call ok
	call test1
	mov rax, QWORD PTR [rbp]
	mov rsp, rbp
	pop rbp
	ret

ok:
	push rbp
	mov rbp, rsp
	sub rsp, 29
L5:
L8:
	jmp L6
L7:
	jmp L6
L4:
	jmp L6
L6:
	call test2
	mov al, BYTE PTR [rbp]
	mov rsp, rbp
	pop rbp
	ret

test1:
	push rbp
	mov rbp, rsp
	sub rsp, 105
L13:
	jmp L11
L12:
	jmp L11
L11:
	movss xmm0, DWORD PTR [rbp]
	mov rsp, rbp
	pop rbp
	ret

test2:
	push rbp
	mov rbp, rsp
	sub rsp, 113
L18:
	jmp L16
L17:
	jmp L16
L16:
	movss xmm0, DWORD PTR [rbp]
	mov rsp, rbp
	pop rbp
	ret

.section .data
