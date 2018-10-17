	.file	"decode.c"
	.section	.rodata
.LC0:
	.string	"Ciphertext:\n%s\n"
.LC1:
	.string	"Plaintext:\n%s\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$20, %esp
	call	read_cipher_file
	movl	%eax, -20(%ebp)
	subl	$8, %esp
	pushl	-20(%ebp)
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	call	get_login_key
	movl	%eax, -16(%ebp)
	subl	$8, %esp
	pushl	-16(%ebp)
	pushl	-20(%ebp)
	call	decode
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	subl	$8, %esp
	pushl	-12(%ebp)
	pushl	$.LC1
	call	printf
	addl	$16, %esp
	movl	$0, %eax
	movl	-4(%ebp), %ecx
	.cfi_def_cfa 1, 0
	leave
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.section	.rodata
.LC2:
	.string	"r"
.LC3:
	.string	"ciphertext.txt"
	.align 4
.LC4:
	.string	"Cannot open file for reading.\n"
	.align 4
.LC5:
	.string	"Error reading ciphertext file.\n"
	.text
	.globl	read_cipher_file
	.type	read_cipher_file, @function
read_cipher_file:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$8, %esp
	pushl	$.LC2
	pushl	$.LC3
	call	fopen
	addl	$16, %esp
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	jne	.L4
	movl	stderr, %eax
	pushl	%eax
	pushl	$30
	pushl	$1
	pushl	$.LC4
	call	fwrite
	addl	$16, %esp
	subl	$12, %esp
	pushl	$1
	call	exit
.L4:
	subl	$12, %esp
	pushl	$1000
	call	malloc
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	subl	$4, %esp
	pushl	-20(%ebp)
	pushl	$1000
	pushl	-16(%ebp)
	call	fgets
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L5
	movl	stderr, %eax
	pushl	%eax
	pushl	$31
	pushl	$1
	pushl	$.LC5
	call	fwrite
	addl	$16, %esp
	subl	$12, %esp
	pushl	$1
	call	exit
.L5:
	subl	$12, %esp
	pushl	-16(%ebp)
	call	strlen
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	leal	-1(%eax), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$10, %al
	jne	.L6
	movl	-12(%ebp), %eax
	leal	-1(%eax), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
.L6:
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	read_cipher_file, .-read_cipher_file
	.section	.rodata
.LC6:
	.string	"Enter your CS login: "
.LC7:
	.string	"Error reading user input.\n"
	.text
	.globl	get_login_key
	.type	get_login_key, @function
get_login_key:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$12, %esp
	pushl	$50
	call	malloc
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	subl	$12, %esp
	pushl	$.LC6
	call	printf
	addl	$16, %esp
	movl	stdin, %eax
	subl	$4, %esp
	pushl	%eax
	pushl	$50
	pushl	-16(%ebp)
	call	fgets
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L9
	movl	stderr, %eax
	pushl	%eax
	pushl	$26
	pushl	$1
	pushl	$.LC7
	call	fwrite
	addl	$16, %esp
	subl	$12, %esp
	pushl	$1
	call	exit
.L9:
	subl	$12, %esp
	pushl	-16(%ebp)
	call	strlen
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	leal	-1(%eax), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$10, %al
	jne	.L10
	movl	-12(%ebp), %eax
	leal	-1(%eax), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
.L10:
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	get_login_key, .-get_login_key
	.globl	decode
	.type	decode, @function
decode:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$12, %esp
	pushl	12(%ebp)
	call	calculate_shifts
	addl	$16, %esp
	movl	%eax, -20(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, -24(%ebp)
	jmp	.L13
.L17:
	movl	-24(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$96, %al
	jle	.L19
	movl	-24(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$122, %al
	jg	.L19
	movl	-24(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	subl	$97, %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	-20(%ebp), %eax
	leal	(%edx,%eax), %ecx
	movl	$1321528399, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$3, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	imull	$26, %eax, %eax
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	addl	$97, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	movb	%dl, (%eax)
	jmp	.L16
.L19:
	nop
.L16:
	addl	$1, -24(%ebp)
.L13:
	movl	-24(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L17
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	decode, .-decode
	.globl	calculate_shifts
	.type	calculate_shifts, @function
calculate_shifts:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L21
.L22:
	movl	-8(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	xorl	%eax, -4(%ebp)
	addl	$1, -8(%ebp)
.L21:
	movl	-8(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L22
	movl	-4(%ebp), %ecx
	movl	$1321528399, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$3, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	imull	$26, %eax, %eax
	subl	%eax, %ecx
	movl	%ecx, %eax
	cltd
	xorl	%edx, %eax
	movl	%eax, -4(%ebp)
	subl	%edx, -4(%ebp)
	cmpl	$0, -4(%ebp)
	jne	.L23
	movl	$1, -4(%ebp)
.L23:
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	calculate_shifts, .-calculate_shifts
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.6) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
