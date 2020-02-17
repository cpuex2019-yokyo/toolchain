	.section .data
	.globl stack0
	.section .text
	.globl start
	.section .text
	.globl _entry
	.globl _j2k
_entry:
	la sp, stack0
	li a0, 1024*4
	csrr a1, mhartid
	addi a1, a1, 1
	mul a0, a0, a1
	add sp, sp, a0
	call start                  
_j2k:
    la a1, dtb
    csrr a0, mhartid
    li t0, 0x80000000    
    jr t0    
dtb:    
    .incbin "board.dtb"
junk:
	j junk
