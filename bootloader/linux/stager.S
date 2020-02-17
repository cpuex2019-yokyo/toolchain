_entry:
    la a1, dtb
    csrr a0, mhartid
    li t0, 0x80000000    
    jr t0
dtb:    
    .incbin "board.dtb"
