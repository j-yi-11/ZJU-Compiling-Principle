	.file	"while_if_test1.c"
	.text
	.globl	whileIf
	.type	whileIf, @function
whileIf:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L6:
	cmpl	$5, -8(%rbp)
	jne	.L3
	movl	$25, -4(%rbp)
	jmp	.L4
.L3:
	cmpl	$10, -8(%rbp)
	jne	.L5
	movl	$42, -4(%rbp)
	jmp	.L4
.L5:
	movl	-8(%rbp), %eax
	addl	%eax, %eax
	movl	%eax, -4(%rbp)
.L4:
	addl	$1, -8(%rbp)
.L2:
	cmpl	$99, -8(%rbp)
	jle	.L6
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	whileIf, .-whileIf
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
	movl	$0, %eax
	call	whileIf
	movl	%eax, %edi
	movl	$0, %eax
	call	putint@PLT
	movl	$0, %eax
	popq	%rbp
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
