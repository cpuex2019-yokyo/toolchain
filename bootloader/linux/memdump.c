#include "const.h"
#include "types.h"
#include "uart.h"
#include "printf.h"
#include "riscv.h"
#include "virtio.h"

#define READ 0
#define WRITE 1

#define MAX_SECTOR_NUM (0xFFFFFF/512)
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
      for (int i=0; i < SECTOR_SIZE; i++){
        uartputc(buffer[i]);
        buffer[i] = '\0';
      }
      
      // request next
      if ((*sector) + 1 >= MAX_SECTOR_NUM) {
        if ((*sector) + 1 == MAX_SECTOR_NUM){
            *(uint32*)PLIC_SCLAIM(0) = irq;
            asm volatile("ecall");
          } else {
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
