	.section .data
	.section .text
	.globl start
	.section .text
	.globl _entry
_entry:
	la sp, 0x82007000
	li a0, 1024*4
	csrr a1, mhartid
	addi a1, a1, 1
	mul a0, a0, a1
	add sp, sp, a0
    // jump to kernel
	call start
junk:
	j junk
