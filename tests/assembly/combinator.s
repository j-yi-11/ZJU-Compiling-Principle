	.file	"combinator.c"
	.text
	.globl	factorial
	.type	factorial, @function
factorial:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	$1, -4(%rbp)
	jmp	.L2
.L3:
	movl	-4(%rbp), %eax
	imull	-20(%rbp), %eax
	movl	%eax, -4(%rbp)
	subl	$1, -20(%rbp)
.L2:
	cmpl	$1, -20(%rbp)
	jg	.L3
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	factorial, .-factorial
	.globl	cal_combiniation
	.type	cal_combiniation, @function
cal_combiniation:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	factorial
	movl	%eax, %ebx
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	call	factorial
	movl	%eax, %r12d
	movl	-20(%rbp), %eax
	subl	-24(%rbp), %eax
	movl	%eax, %edi
	call	factorial
	movl	%r12d, %edx
	imull	%eax, %edx
	movl	%edx, %ecx
	movl	%ebx, %eax
	cltd
	idivl	%ecx
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	cal_combiniation, .-cal_combiniation
	.globl	cal_permutation
	.type	cal_permutation, @function
cal_permutation:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	factorial
	movl	%eax, %ebx
	movl	-12(%rbp), %eax
	subl	-16(%rbp), %eax
	movl	%eax, %edi
	call	factorial
	movl	%eax, %ecx
	movl	%ebx, %eax
	cltd
	idivl	%ecx
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	cal_permutation, .-cal_permutation
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, %eax
	call	getint@PLT
	movl	%eax, -8(%rbp)
	movl	$0, %eax
	call	getint@PLT
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	cal_combiniation
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	cal_permutation
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
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
