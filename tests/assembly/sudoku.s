	.file	"sudoku.c"
	.text
	.globl	a
	.bss
	.align 32
	.type	a, @object
	.size	a, 324
a:
	.zero	324
	.text
	.globl	check_sudoku
	.type	check_sudoku, @function
check_sudoku:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%rdi, -88(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -68(%rbp)
	jmp	.L2
.L10:
	movl	$0, -64(%rbp)
	jmp	.L3
.L4:
	movl	-64(%rbp), %eax
	cltq
	movl	$0, -48(%rbp,%rax,4)
	addl	$1, -64(%rbp)
.L3:
	cmpl	$8, -64(%rbp)
	jle	.L4
	movl	$0, -64(%rbp)
	jmp	.L5
.L9:
	movl	-68(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	addq	%rax, %rdx
	movl	-64(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	testl	%eax, %eax
	je	.L6
	movl	-68(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	addq	%rax, %rdx
	movl	-64(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	subl	$1, %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	testl	%eax, %eax
	jne	.L7
	movl	-68(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	addq	%rax, %rdx
	movl	-64(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	subl	$1, %eax
	cltq
	movl	$1, -48(%rbp,%rax,4)
	jmp	.L6
.L7:
	movl	$0, %eax
	jmp	.L31
.L6:
	addl	$1, -64(%rbp)
.L5:
	cmpl	$8, -64(%rbp)
	jle	.L9
	addl	$1, -68(%rbp)
.L2:
	cmpl	$8, -68(%rbp)
	jle	.L10
	movl	$0, -68(%rbp)
	jmp	.L11
.L18:
	movl	$0, -64(%rbp)
	jmp	.L12
.L13:
	movl	-64(%rbp), %eax
	cltq
	movl	$0, -48(%rbp,%rax,4)
	addl	$1, -64(%rbp)
.L12:
	cmpl	$8, -64(%rbp)
	jle	.L13
	movl	$0, -64(%rbp)
	jmp	.L14
.L17:
	movl	-64(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	addq	%rax, %rdx
	movl	-68(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	testl	%eax, %eax
	je	.L15
	movl	-64(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	addq	%rax, %rdx
	movl	-68(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	subl	$1, %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	testl	%eax, %eax
	jne	.L16
	movl	-64(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	addq	%rax, %rdx
	movl	-68(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	subl	$1, %eax
	cltq
	movl	$1, -48(%rbp,%rax,4)
	jmp	.L15
.L16:
	movl	$0, %eax
	jmp	.L31
.L15:
	addl	$1, -64(%rbp)
.L14:
	cmpl	$8, -64(%rbp)
	jle	.L17
	addl	$1, -68(%rbp)
.L11:
	cmpl	$8, -68(%rbp)
	jle	.L18
	movl	$0, -68(%rbp)
	jmp	.L19
.L30:
	movl	$0, -64(%rbp)
	jmp	.L20
.L29:
	movl	$0, -60(%rbp)
	jmp	.L21
.L22:
	movl	-60(%rbp), %eax
	cltq
	movl	$0, -48(%rbp,%rax,4)
	addl	$1, -60(%rbp)
.L21:
	cmpl	$8, -60(%rbp)
	jle	.L22
	movl	$0, -60(%rbp)
	jmp	.L23
.L28:
	movl	$0, -56(%rbp)
	jmp	.L24
.L27:
	movl	-68(%rbp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%eax, %edx
	movl	-60(%rbp), %eax
	addl	%edx, %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	-88(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movl	-64(%rbp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%eax, %edx
	movl	-56(%rbp), %eax
	addl	%edx, %eax
	cltq
	movl	(%rcx,%rax,4), %eax
	movl	%eax, -52(%rbp)
	cmpl	$0, -52(%rbp)
	je	.L25
	movl	-52(%rbp), %eax
	subl	$1, %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	testl	%eax, %eax
	jne	.L26
	movl	-52(%rbp), %eax
	subl	$1, %eax
	cltq
	movl	$1, -48(%rbp,%rax,4)
	jmp	.L25
.L26:
	movl	$0, %eax
	jmp	.L31
.L25:
	addl	$1, -56(%rbp)
.L24:
	cmpl	$2, -56(%rbp)
	jle	.L27
	addl	$1, -60(%rbp)
.L23:
	cmpl	$2, -60(%rbp)
	jle	.L28
	addl	$1, -64(%rbp)
.L20:
	cmpl	$2, -64(%rbp)
	jle	.L29
	addl	$1, -68(%rbp)
.L19:
	cmpl	$2, -68(%rbp)
	jle	.L30
	movl	$1, %eax
.L31:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L32
	call	__stack_chk_fail@PLT
.L32:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	check_sudoku, .-check_sudoku
	.globl	solve_sudoku
	.type	solve_sudoku, @function
solve_sudoku:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$1, -4(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L34
.L40:
	movl	$0, -16(%rbp)
	jmp	.L35
.L38:
	movl	-16(%rbp), %eax
	movslq	%eax, %rcx
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L36
	movl	$0, -4(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -8(%rbp)
.L36:
	addl	$1, -16(%rbp)
.L35:
	cmpl	$8, -16(%rbp)
	jg	.L37
	cmpl	$0, -4(%rbp)
	jne	.L38
.L37:
	addl	$1, -20(%rbp)
.L34:
	cmpl	$8, -20(%rbp)
	jg	.L39
	cmpl	$0, -4(%rbp)
	jne	.L40
.L39:
	cmpl	$0, -4(%rbp)
	je	.L41
	leaq	a(%rip), %rax
	movq	%rax, %rdi
	call	check_sudoku
	jmp	.L42
.L41:
	movl	$1, -20(%rbp)
	jmp	.L43
.L45:
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	leaq	a(%rip), %rdx
	movl	-20(%rbp), %eax
	movl	%eax, (%rcx,%rdx)
	leaq	a(%rip), %rax
	movq	%rax, %rdi
	call	check_sudoku
	testl	%eax, %eax
	je	.L44
	movl	$0, %eax
	call	solve_sudoku
	testl	%eax, %eax
	je	.L44
	movl	$1, %eax
	jmp	.L42
.L44:
	addl	$1, -20(%rbp)
.L43:
	cmpl	$9, -20(%rbp)
	jle	.L45
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	$0, (%rdx,%rax)
	movl	$0, %eax
.L42:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	solve_sudoku, .-solve_sudoku
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
	subq	$16, %rsp
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L47
.L54:
	movl	$0, -8(%rbp)
	jmp	.L48
.L53:
	movl	$0, %eax
	call	getint@PLT
	movl	%eax, %edx
	movl	-8(%rbp), %eax
	movslq	%eax, %rsi
	movl	-12(%rbp), %eax
	movslq	%eax, %rcx
	movq	%rcx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	addq	%rsi, %rax
	leaq	0(,%rax,4), %rcx
	leaq	a(%rip), %rax
	movl	%edx, (%rcx,%rax)
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	js	.L49
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$9, %eax
	jle	.L50
.L49:
	movl	$0, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	movl	$0, %eax
	jmp	.L51
.L50:
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L52
	movl	$1, -4(%rbp)
.L52:
	addl	$1, -8(%rbp)
.L48:
	cmpl	$8, -8(%rbp)
	jle	.L53
	addl	$1, -12(%rbp)
.L47:
	cmpl	$8, -12(%rbp)
	jle	.L54
	cmpl	$0, -4(%rbp)
	je	.L55
	movl	$0, %eax
	call	solve_sudoku
	testl	%eax, %eax
	je	.L56
	movl	$0, -12(%rbp)
	jmp	.L57
.L60:
	movl	$0, -8(%rbp)
	jmp	.L58
.L59:
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	(%rdx,%rax), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	addl	$1, -8(%rbp)
.L58:
	cmpl	$8, -8(%rbp)
	jle	.L59
	addl	$1, -12(%rbp)
.L57:
	cmpl	$8, -12(%rbp)
	jle	.L60
	jmp	.L61
.L56:
	movl	$0, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	jmp	.L61
.L55:
	leaq	a(%rip), %rax
	movq	%rax, %rdi
	call	check_sudoku
	testl	%eax, %eax
	je	.L62
	movl	$1, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
	jmp	.L61
.L62:
	movl	$0, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$32, %edi
	movl	$0, %eax
	call	putch@PLT
.L61:
	movl	$0, %eax
.L51:
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
