	.file	"array_init2.c"
	.text
	.globl	print_array
	.type	print_array, @function
print_array:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L2
.L5:
	movl	$0, -4(%rbp)
	jmp	.L3
.L4:
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-24(%rbp), %rax
	addq	%rax, %rdx
	movl	-4(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	addl	$1, -4(%rbp)
.L3:
	cmpl	$1, -4(%rbp)
	jle	.L4
	addl	$1, -8(%rbp)
.L2:
	cmpl	$3, -8(%rbp)
	jle	.L5
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	print_array, .-print_array
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$112, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$1, -112(%rbp)
	movl	$2, -108(%rbp)
	movl	$3, -104(%rbp)
	movl	$4, -100(%rbp)
	movl	$5, -96(%rbp)
	movl	$6, -92(%rbp)
	movl	$7, -88(%rbp)
	movl	$8, -84(%rbp)
	movl	$1, -80(%rbp)
	movl	$2, -76(%rbp)
	movl	$3, -72(%rbp)
	movl	$4, -68(%rbp)
	movl	$5, -64(%rbp)
	movl	$6, -60(%rbp)
	movl	$7, -56(%rbp)
	movl	$8, -52(%rbp)
	movl	$1, -48(%rbp)
	movl	$2, -44(%rbp)
	movl	$3, -40(%rbp)
	movl	$4, -36(%rbp)
	movl	$5, -32(%rbp)
	movl	$6, -28(%rbp)
	movl	$7, -24(%rbp)
	movl	$8, -20(%rbp)
	leaq	-112(%rbp), %rax
	movq	%rax, %rdi
	call	print_array
	leaq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	print_array
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	print_array
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L9
	call	__stack_chk_fail@PLT
.L9:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
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
