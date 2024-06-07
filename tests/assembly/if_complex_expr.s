	.file	"if_complex_expr.c"
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
	subq	$32, %rsp
	movl	$5, -20(%rbp)
	movl	$5, -16(%rbp)
	movl	$1, -12(%rbp)
	movl	$-2, -8(%rbp)
	movl	$2, -4(%rbp)
	cmpl	$-1, -8(%rbp)
	jl	.L2
	movl	-20(%rbp), %eax
	cmpl	-16(%rbp), %eax
	je	.L3
	movl	-12(%rbp), %eax
	addl	$3, %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L3
.L2:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
.L3:
	movl	-8(%rbp), %eax
	cltd
	shrl	$31, %edx
	addl	%edx, %eax
	andl	$1, %eax
	subl	%edx, %eax
	cmpl	$-67, %eax
	jl	.L4
	movl	-20(%rbp), %eax
	cmpl	-16(%rbp), %eax
	je	.L5
	movl	-12(%rbp), %eax
	addl	$2, %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L5
.L4:
	movl	$4, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
.L5:
	movl	$0, %eax
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
