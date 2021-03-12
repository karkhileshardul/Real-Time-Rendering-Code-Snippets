	.file	"Tempplate.c"
	.section	.rodata
.LC0:
	.string	"./libprogram.so"
.LC1:
	.string	"%s\n"
.LC2:
	.string	"Addition"
.LC3:
	.string	"Addition is %d\n"
.LC4:
	.string	"Subtraction"
.LC5:
	.string	"Subtraction is %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movl	$1, %esi
	movl	$.LC0, %edi
	call	dlopen
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L2
	call	dlerror
	movq	%rax, %rdx
	movq	stderr(%rip), %rax
	movl	$.LC1, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %edi
	call	exit
.L2:
	movq	-24(%rbp), %rax
	movl	$.LC2, %esi
	movq	%rax, %rdi
	call	dlsym
	movq	%rax, -16(%rbp)
	call	dlerror
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.L3
	movq	stderr(%rip), %rax
	movq	-8(%rbp), %rdx
	movl	$.LC1, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %edi
	call	exit
.L3:
	movq	-16(%rbp), %rax
	movl	$5, %esi
	movl	$5, %edi
	call	*%rax
	movl	%eax, %esi
	movl	$.LC3, %edi
	movl	$0, %eax
	call	printf
	movq	-24(%rbp), %rax
	movl	$.LC4, %esi
	movq	%rax, %rdi
	call	dlsym
	movq	%rax, -16(%rbp)
	call	dlerror
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.L4
	movq	stderr(%rip), %rax
	movq	-8(%rbp), %rdx
	movl	$.LC1, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %edi
	call	exit
.L4:
	movq	-16(%rbp), %rax
	movl	$2, %esi
	movl	$8, %edi
	call	*%rax
	movl	%eax, %esi
	movl	$.LC5, %edi
	movl	$0, %eax
	call	printf
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	dlclose
	movq	$0, -24(%rbp)
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.3) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
