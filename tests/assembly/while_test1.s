	.file	"while_test1.c"
	.text
	.globl	doubleWhile
	.type	doubleWhile, @function
doubleWhile:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$5, -8(%rbp)
	movl	$7, -4(%rbp)
	jmp	.L2
.L5:
	addl	$30, -8(%rbp)
	jmp	.L3
.L4:
	addl	$6, -4(%rbp)
.L3:
	cmpl	$99, -4(%rbp)
	jle	.L4
	subl	$100, -4(%rbp)
.L2:
	cmpl	$99, -8(%rbp)
	jle	.L5
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	doubleWhile, .-doubleWhile
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
	call	doubleWhile
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