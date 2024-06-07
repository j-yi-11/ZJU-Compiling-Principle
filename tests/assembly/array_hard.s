	.file	"array_hard.c"
	.text
	.globl	initW
	.type	initW, @function
initW:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$0, (%rax)
	movq	-8(%rbp), %rax
	addq	$4, %rax
	movl	$1, (%rax)
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movl	$2, (%rax)
	movq	-8(%rbp), %rax
	addq	$12, %rax
	movl	$5, (%rax)
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movl	$6, (%rax)
	movq	-8(%rbp), %rax
	addq	$20, %rax
	movl	$7, (%rax)
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	initW, .-initW
	.globl	initV
	.type	initV, @function
initV:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$0, (%rax)
	movq	-8(%rbp), %rax
	addq	$4, %rax
	movl	$1, (%rax)
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movl	$6, (%rax)
	movq	-8(%rbp), %rax
	addq	$12, %rax
	movl	$18, (%rax)
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movl	$22, (%rax)
	movq	-8(%rbp), %rax
	addq	$20, %rax
	movl	$28, (%rax)
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	initV, .-initV
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$384, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-368(%rbp), %rax
	movq	%rax, %rdi
	call	initW
	leaq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	initV
	movl	$0, -384(%rbp)
	jmp	.L6
.L9:
	movl	$0, -380(%rbp)
	jmp	.L7
.L8:
	movl	-380(%rbp), %eax
	movslq	%eax, %rcx
	movl	-384(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rcx, %rax
	movl	$0, -304(%rbp,%rax,4)
	addl	$1, -380(%rbp)
.L7:
	cmpl	$11, -380(%rbp)
	jle	.L8
	addl	$1, -384(%rbp)
.L6:
	cmpl	$5, -384(%rbp)
	jle	.L9
	movl	$1, -376(%rbp)
	jmp	.L10
.L16:
	movl	$1, -372(%rbp)
	jmp	.L11
.L15:
	movl	-376(%rbp), %eax
	cltq
	movl	-368(%rbp,%rax,4), %eax
	cmpl	%eax, -372(%rbp)
	jge	.L12
	movl	-376(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-372(%rbp), %eax
	movslq	%eax, %rcx
	movslq	%edx, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rcx, %rax
	movl	-304(%rbp,%rax,4), %edx
	movl	-372(%rbp), %eax
	movslq	%eax, %rsi
	movl	-376(%rbp), %eax
	movslq	%eax, %rcx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$2, %rax
	addq	%rsi, %rax
	movl	%edx, -304(%rbp,%rax,4)
	jmp	.L13
.L12:
	movl	-376(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-372(%rbp), %eax
	movslq	%eax, %rcx
	movslq	%edx, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rcx, %rax
	movl	-304(%rbp,%rax,4), %ecx
	movl	-376(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %esi
	movl	-376(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-376(%rbp), %eax
	cltq
	movl	-368(%rbp,%rax,4), %edi
	movl	-372(%rbp), %eax
	subl	%edi, %eax
	movslq	%eax, %rdi
	movslq	%edx, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rdi, %rax
	movl	-304(%rbp,%rax,4), %eax
	addl	%esi, %eax
	cmpl	%eax, %ecx
	jle	.L14
	movl	-376(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-372(%rbp), %eax
	movslq	%eax, %rcx
	movslq	%edx, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rcx, %rax
	movl	-304(%rbp,%rax,4), %edx
	movl	-372(%rbp), %eax
	movslq	%eax, %rsi
	movl	-376(%rbp), %eax
	movslq	%eax, %rcx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$2, %rax
	addq	%rsi, %rax
	movl	%edx, -304(%rbp,%rax,4)
	jmp	.L13
.L14:
	movl	-376(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %ecx
	movl	-376(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-376(%rbp), %eax
	cltq
	movl	-368(%rbp,%rax,4), %esi
	movl	-372(%rbp), %eax
	subl	%esi, %eax
	movslq	%eax, %rsi
	movslq	%edx, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rsi, %rax
	movl	-304(%rbp,%rax,4), %eax
	addl	%eax, %ecx
	movl	-372(%rbp), %eax
	movslq	%eax, %rsi
	movl	-376(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rsi, %rax
	movl	%ecx, -304(%rbp,%rax,4)
.L13:
	addl	$1, -372(%rbp)
.L11:
	cmpl	$11, -372(%rbp)
	jle	.L15
	addl	$1, -376(%rbp)
.L10:
	cmpl	$5, -376(%rbp)
	jle	.L16
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L18
	call	__stack_chk_fail@PLT
.L18:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
