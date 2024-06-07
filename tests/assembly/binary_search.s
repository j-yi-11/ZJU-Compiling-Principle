	.file	"binary_search.c"
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
	subq	$80, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, %eax
	call	getint@PLT
	movl	%eax, -68(%rbp)
	cmpl	$10, -68(%rbp)
	jle	.L2
	movl	$10, -68(%rbp)
.L2:
	movl	$0, -64(%rbp)
	jmp	.L3
.L4:
	movl	$0, %eax
	call	getint@PLT
	movl	-64(%rbp), %edx
	movslq	%edx, %rdx
	movl	%eax, -48(%rbp,%rdx,4)
	addl	$1, -64(%rbp)
.L3:
	movl	-64(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jl	.L4
	movl	$0, %eax
	call	getint@PLT
	movl	%eax, -52(%rbp)
	movl	$0, -60(%rbp)
	movl	-68(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -56(%rbp)
	jmp	.L5
.L9:
	movl	-60(%rbp), %edx
	movl	-56(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	movl	%eax, -64(%rbp)
	movl	-64(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	cmpl	%eax, -52(%rbp)
	jne	.L6
	movl	-64(%rbp), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$0, %eax
	jmp	.L10
.L6:
	movl	-64(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	cmpl	%eax, -52(%rbp)
	jge	.L8
	movl	-64(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -56(%rbp)
	jmp	.L5
.L8:
	movl	-64(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -60(%rbp)
.L5:
	movl	-60(%rbp), %eax
	cmpl	-56(%rbp), %eax
	jl	.L9
	movl	$0, %eax
.L10:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L11
	call	__stack_chk_fail@PLT
.L11:
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
