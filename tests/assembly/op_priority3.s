	.file	"op_priority3.c"
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
	movl	$0, -24(%rbp)
	movl	$1, -20(%rbp)
	movl	$0, -16(%rbp)
	movl	$1, -12(%rbp)
	movl	$2, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-20(%rbp), %eax
	imull	-16(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%eax, %ecx
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	cmpl	%eax, %ecx
	jne	.L2
	movl	-20(%rbp), %edx
	movl	-16(%rbp), %eax
	addl	%edx, %eax
	imull	-20(%rbp), %eax
	movl	%eax, %edx
	movl	-12(%rbp), %eax
	addl	%eax, %edx
	movl	-8(%rbp), %ecx
	movl	-4(%rbp), %eax
	addl	%ecx, %eax
	cmpl	%eax, %edx
	jle	.L3
.L2:
	movl	-16(%rbp), %eax
	imull	-12(%rbp), %eax
	movl	%eax, %edx
	movl	-20(%rbp), %eax
	subl	%edx, %eax
	movl	%eax, %ecx
	movl	-20(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%eax, %edx
	movl	-8(%rbp), %eax
	subl	%edx, %eax
	cmpl	%eax, %ecx
	jne	.L4
.L3:
	movl	$1, -24(%rbp)
.L4:
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
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
