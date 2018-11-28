	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 11
	.globl	_main
	.align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	callq	_fork
	movl	%eax, -20(%rbp)
	cmpl	$0, -20(%rbp)
	jne	LBB0_2
## BB#1:
	leaq	L_.str(%rip), %rsi
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	movl	-20(%rbp), %edx
	movb	$0, %al
	callq	_fprintf
	movl	%eax, -24(%rbp)         ## 4-byte Spill
	jmp	LBB0_3
LBB0_2:
	leaq	L_.str.1(%rip), %rsi
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	movl	-20(%rbp), %edx
	movb	$0, %al
	callq	_fprintf
	movl	%eax, -28(%rbp)         ## 4-byte Spill
LBB0_3:
	movl	-4(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"[child_pid: %d]yourself\n"

L_.str.1:                               ## @.str.1
	.asciz	"[parent_pid: %d]kill\n"


.subsections_via_symbols
