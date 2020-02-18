#pragma once

#define SECTOR_SIZE 512

#define UART0 0x10000000
#define UART0_IRQ 10

#define VIRTIO0 0x10001000
#define VIRTIO0_IRQ 1

#define PLIC 0x0c000000

#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)

#define PGSIZE 4096
#define PGSHIFT 12
