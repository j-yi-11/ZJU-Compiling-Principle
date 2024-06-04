	.file	"sort_array.c"
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
	subq	$64, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -60(%rbp)
	jmp	.L2
.L3:
	movl	$0, %eax
	call	getint@PLT
	movl	-60(%rbp), %edx
	movslq	%edx, %rdx
	movl	%eax, -48(%rbp,%rdx,4)
	addl	$1, -60(%rbp)
.L2:
	cmpl	$4, -60(%rbp)
	jle	.L3
	movl	$0, -60(%rbp)
	jmp	.L4
.L8:
	movl	-60(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -56(%rbp)
	jmp	.L5
.L7:
	movl	-60(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %edx
	movl	-56(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	cmpl	%eax, %edx
	jle	.L6
	movl	-60(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	movl	%eax, -52(%rbp)
	movl	-56(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %edx
	movl	-60(%rbp), %eax
	cltq
	movl	%edx, -48(%rbp,%rax,4)
	movl	-56(%rbp), %eax
	cltq
	movl	-52(%rbp), %edx
	movl	%edx, -48(%rbp,%rax,4)
.L6:
	addl	$1, -56(%rbp)
.L5:
	cmpl	$4, -56(%rbp)
	jle	.L7
	addl	$1, -60(%rbp)
.L4:
	cmpl	$3, -60(%rbp)
	jle	.L8
	movl	$0, -60(%rbp)
	jmp	.L9
.L10:
	movl	-60(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	addl	$1, -60(%rbp)
.L9:
	cmpl	$4, -60(%rbp)
	jle	.L10
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L12
	call	__stack_chk_fail@PLT
.L12:
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
