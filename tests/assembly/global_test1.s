	.file	"global_test1.c"
	.text
	.globl	a
	.bss
	.align 4
	.type	a, @object
	.size	a, 4
a:
	.zero	4
	.globl	b
	.align 8
	.type	b, @object
	.size	b, 8
b:
	.zero	8
	.globl	c
	.align 32
	.type	c, @object
	.size	c, 48
c:
	.zero	48
	.globl	i
	.data
	.align 4
	.type	i, @object
	.size	i, 4
i:
	.long	2024
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
	subq	$16, %rsp
	jmp	.L2
.L3:
	movl	a(%rip), %edx
	movl	a(%rip), %eax
	leal	(%rdx,%rdx), %ecx
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	b(%rip), %rax
	movl	%ecx, (%rdx,%rax)
	movl	a(%rip), %eax
	addl	$1, %eax
	movl	%eax, a(%rip)
.L2:
	movl	a(%rip), %eax
	cmpl	$1, %eax
	jle	.L3
	movl	b(%rip), %edx
	movl	a(%rip), %eax
	addl	%eax, %edx
	movl	4+b(%rip), %eax
	addl	%edx, %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	i(%rip), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	$0, -4(%rbp)
	jmp	.L4
.L5:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1431655766, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$31, %eax
	movl	%edx, %esi
	subl	%eax, %esi
	movl	-4(%rbp), %edx
	movslq	%edx, %rax
	imulq	$1431655766, %rax, %rax
	shrq	$32, %rax
	movl	%edx, %ecx
	sarl	$31, %ecx
	subl	%ecx, %eax
	movl	%eax, %ecx
	addl	%ecx, %ecx
	addl	%eax, %ecx
	movl	%edx, %eax
	subl	%ecx, %eax
	movslq	%eax, %rcx
	movslq	%esi, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	c(%rip), %rax
	movl	$0, (%rdx,%rax)
	addl	$1, -4(%rbp)
.L4:
	cmpl	$11, -4(%rbp)
	jle	.L5
	movl	$1, c(%rip)
	movl	$1, 4+c(%rip)
	movl	$4, 8+c(%rip)
	movl	$5, 12+c(%rip)
	movl	$1, 24+c(%rip)
	movl	$4, 36+c(%rip)
	movl	$0, -4(%rbp)
	jmp	.L6
.L7:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1431655766, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$31, %eax
	movl	%edx, %esi
	subl	%eax, %esi
	movl	-4(%rbp), %edx
	movslq	%edx, %rax
	imulq	$1431655766, %rax, %rax
	shrq	$32, %rax
	movl	%edx, %ecx
	sarl	$31, %ecx
	subl	%ecx, %eax
	movl	%eax, %ecx
	addl	%ecx, %ecx
	addl	%eax, %ecx
	movl	%edx, %eax
	subl	%ecx, %eax
	movslq	%eax, %rcx
	movslq	%esi, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	c(%rip), %rax
	movl	(%rdx,%rax), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	addl	$1, -4(%rbp)
.L6:
	cmpl	$11, -4(%rbp)
	jle	.L7
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
