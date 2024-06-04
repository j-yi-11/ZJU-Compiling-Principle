	.file	"merge_sort.c"
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
	subq	$112, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$5, -84(%rbp)
	movl	$0, -100(%rbp)
	jmp	.L2
.L3:
	movl	$0, %eax
	call	getint@PLT
	movl	-100(%rbp), %edx
	movslq	%edx, %rdx
	movl	%eax, -64(%rbp,%rdx,4)
	addl	$1, -100(%rbp)
.L2:
	movl	-100(%rbp), %eax
	cmpl	-84(%rbp), %eax
	jl	.L3
	movl	$1, -96(%rbp)
	jmp	.L4
.L30:
	movl	$0, -100(%rbp)
	jmp	.L5
.L15:
	movl	-100(%rbp), %eax
	movl	%eax, -80(%rbp)
	movl	-80(%rbp), %edx
	movl	-96(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -76(%rbp)
	movl	-80(%rbp), %eax
	movl	%eax, -92(%rbp)
	movl	-76(%rbp), %eax
	movl	%eax, -72(%rbp)
	movl	-72(%rbp), %edx
	movl	-96(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -68(%rbp)
	movl	-72(%rbp), %eax
	movl	%eax, -88(%rbp)
	jmp	.L6
.L10:
	movl	-92(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-88(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %eax
	cmpl	%eax, %edx
	jge	.L7
	movl	-92(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -92(%rbp)
	jmp	.L8
.L7:
	movl	-88(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -88(%rbp)
.L8:
	addl	$1, -100(%rbp)
.L6:
	movl	-92(%rbp), %eax
	cmpl	-76(%rbp), %eax
	jge	.L11
	movl	-88(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jl	.L10
	jmp	.L11
.L12:
	movl	-92(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -92(%rbp)
	addl	$1, -100(%rbp)
.L11:
	movl	-92(%rbp), %eax
	cmpl	-76(%rbp), %eax
	jl	.L12
	jmp	.L13
.L14:
	movl	-88(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -88(%rbp)
	addl	$1, -100(%rbp)
.L13:
	movl	-88(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jl	.L14
.L5:
	movl	-96(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-84(%rbp), %eax
	subl	%edx, %eax
	cmpl	%eax, -100(%rbp)
	jle	.L15
	movl	-100(%rbp), %edx
	movl	-96(%rbp), %eax
	addl	%edx, %eax
	cmpl	%eax, -84(%rbp)
	jle	.L26
	movl	-100(%rbp), %eax
	movl	%eax, -80(%rbp)
	movl	-80(%rbp), %edx
	movl	-96(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -76(%rbp)
	movl	-80(%rbp), %eax
	movl	%eax, -92(%rbp)
	movl	-76(%rbp), %eax
	movl	%eax, -72(%rbp)
	movl	-84(%rbp), %eax
	movl	%eax, -68(%rbp)
	movl	-72(%rbp), %eax
	movl	%eax, -88(%rbp)
	jmp	.L17
.L20:
	movl	-92(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-88(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %eax
	cmpl	%eax, %edx
	jge	.L18
	movl	-92(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -92(%rbp)
	addl	$1, -100(%rbp)
	jmp	.L17
.L18:
	movl	-88(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -88(%rbp)
	addl	$1, -100(%rbp)
.L17:
	movl	-92(%rbp), %eax
	cmpl	-76(%rbp), %eax
	jge	.L21
	movl	-88(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jl	.L20
	jmp	.L21
.L22:
	movl	-92(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -92(%rbp)
	addl	$1, -100(%rbp)
.L21:
	movl	-92(%rbp), %eax
	cmpl	-76(%rbp), %eax
	jl	.L22
	jmp	.L23
.L24:
	movl	-88(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -88(%rbp)
	addl	$1, -100(%rbp)
.L23:
	movl	-88(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jl	.L24
	jmp	.L25
.L27:
	movl	-100(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -32(%rbp,%rax,4)
	addl	$1, -100(%rbp)
.L26:
	movl	-100(%rbp), %eax
	cmpl	-84(%rbp), %eax
	jl	.L27
.L25:
	movl	$0, -100(%rbp)
	jmp	.L28
.L29:
	movl	-100(%rbp), %eax
	cltq
	movl	-32(%rbp,%rax,4), %edx
	movl	-100(%rbp), %eax
	cltq
	movl	%edx, -64(%rbp,%rax,4)
	addl	$1, -100(%rbp)
.L28:
	movl	-100(%rbp), %eax
	cmpl	-84(%rbp), %eax
	jl	.L29
	sall	-96(%rbp)
.L4:
	movl	-96(%rbp), %eax
	cmpl	-84(%rbp), %eax
	jl	.L30
	movl	$0, -100(%rbp)
	jmp	.L31
.L32:
	movl	-100(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	addl	$1, -100(%rbp)
.L31:
	movl	-100(%rbp), %eax
	cmpl	-84(%rbp), %eax
	jl	.L32
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L34
	call	__stack_chk_fail@PLT
.L34:
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
