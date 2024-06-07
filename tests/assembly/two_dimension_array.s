	.file	"two_dimension_array.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -92(%rbp)
	jmp	.L2
.L5:
	movl	$0, -88(%rbp)
	jmp	.L3
.L4:
	movl	-88(%rbp), %eax
	movslq	%eax, %rdx
	movl	-92(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	$999, -80(%rbp,%rax,4)
	addl	$1, -88(%rbp)
.L3:
	cmpl	$3, -88(%rbp)
	jle	.L4
	addl	$1, -92(%rbp)
.L2:
	cmpl	$3, -92(%rbp)
	jle	.L5
	movl	$0, -92(%rbp)
	jmp	.L6
.L7:
	movl	-92(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$80, %rax
	movl	$0, (%rax)
	addl	$1, -92(%rbp)
.L6:
	cmpl	$3, -92(%rbp)
	jle	.L7
	movl	$3, -76(%rbp)
	movl	$5, -68(%rbp)
	movl	$2, -64(%rbp)
	movl	$4, -52(%rbp)
	movl	$1, -44(%rbp)
	movl	$2, -24(%rbp)
	movl	$0, -84(%rbp)
	jmp	.L8
.L14:
	movl	$0, -92(%rbp)
	jmp	.L9
.L13:
	movl	$0, -88(%rbp)
	jmp	.L10
.L12:
	movl	-84(%rbp), %eax
	movslq	%eax, %rdx
	movl	-92(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	-80(%rbp,%rax,4), %edx
	movl	-88(%rbp), %eax
	movslq	%eax, %rcx
	movl	-84(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rcx, %rax
	movl	-80(%rbp,%rax,4), %eax
	addl	%eax, %edx
	movl	-88(%rbp), %eax
	movslq	%eax, %rcx
	movl	-92(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rcx, %rax
	movl	-80(%rbp,%rax,4), %eax
	cmpl	%eax, %edx
	jge	.L11
	movl	-84(%rbp), %eax
	movslq	%eax, %rdx
	movl	-92(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	-80(%rbp,%rax,4), %edx
	movl	-88(%rbp), %eax
	movslq	%eax, %rcx
	movl	-84(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rcx, %rax
	movl	-80(%rbp,%rax,4), %eax
	addl	%eax, %edx
	movl	-88(%rbp), %eax
	movslq	%eax, %rcx
	movl	-92(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	%rcx, %rax
	movl	%edx, -80(%rbp,%rax,4)
.L11:
	addl	$1, -88(%rbp)
.L10:
	cmpl	$3, -88(%rbp)
	jle	.L12
	addl	$1, -92(%rbp)
.L9:
	cmpl	$3, -92(%rbp)
	jle	.L13
	addl	$1, -84(%rbp)
.L8:
	cmpl	$3, -84(%rbp)
	jle	.L14
	movl	-68(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	-56(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	-44(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	-32(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L16
	call	__stack_chk_fail@PLT
.L16:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
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
