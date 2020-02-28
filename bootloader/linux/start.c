/*
Copyright (c) 2006-2019 Frans Kaashoek, Robert Morris, Russ Cox,
                        Massachusetts Institute of Technology

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "const.h"
#include "types.h"
#include "uart.h"
#include "printf.h"
#include "riscv.h"
#include "virtio.h"

#define READ 0
#define WRITE 1

#define MAX_SECTOR_NUM (0xFFFFFF * 2)/512
#define KERNBASE 0x80000000

int *sector = (int *) 0x8200b000;
char *buffer = (char *) 0x82003000;

void _finalize();
void _trapvec();

// in U mode ...
int wait_for_interrupt(){  
  while(1);
}

// in S mode ...
void jump_to_user(uint32 dest){
  uint32 x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);
  w_sepc(dest);
  asm volatile("sret");    
}

void handle(){
  uint32 scause = r_scause();
  uint32 irq = *(uint32*)PLIC_SCLAIM(0);
  
  if((scause & 0x80000000)
     && (scause & 0xff) == 9
     && irq == VIRTIO0_IRQ){
    while(virtio_used_updated()){
      // copy to mem
      char *mem = (char*) (uint32) (uint64) (KERNBASE + (*sector) * SECTOR_SIZE);      
      for (int i=0; i < SECTOR_SIZE; i++){
        mem[i] = buffer[i];
      }
      if (((*sector)+1) % (MAX_SECTOR_NUM/10) == 0)
        printf("%d\n", ((*sector)+1) / (MAX_SECTOR_NUM/10));
      
      // request next
      if ((*sector) + 1 >= MAX_SECTOR_NUM) {
        if ((*sector) + 1 == MAX_SECTOR_NUM){
            *(uint32*)PLIC_SCLAIM(0) = irq;
            printf("\nBootloader: Running Kernel ...\n");
            asm volatile("ecall");
          } else {
            printf("Ha?\n");
            while(1);
          }
        }
    }
    *(uint32*)PLIC_SCLAIM(0) = irq;
    virtio_disk_rw(++(*sector), READ, buffer);
  }  
}

// in supervisor mode...  
void smain(){
  // enazoble S-mode interrupt
  w_sie(r_sie() | SIE_SEIE);
  w_sstatus(r_sstatus() | SSTATUS_SIE);
  w_stvec((uint32)_trapvec);
  
  *(uint32*)(PLIC + VIRTIO0_IRQ*4) = 1;
  *(uint32*)PLIC_SENABLE(0) = (1 << VIRTIO0_IRQ);
  *(uint32*)PLIC_SPRIORITY(0) = 0;
  
  uartinit();
  printf("Bootloader: Loading Kernel Image (Linux) ...\n");
  
  virtio_disk_init();  
  virtio_disk_rw(*sector, READ, buffer);  
  jump_to_user((uint32) wait_for_interrupt);
}

void start(){  
  // delegate all interrupts to S-mode
  w_medeleg(0x0000);
  w_mideleg(0xffff);  
  w_mtvec((uint32) _finalize);

  // disable paging
  w_satp(0);

  *sector = 0;

  // jump to write_test and go to S-mode
  uint32 x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);
  w_mepc((uint32)smain);  
  asm volatile("mret");  
}
