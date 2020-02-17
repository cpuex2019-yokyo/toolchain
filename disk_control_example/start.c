#include "const.h"
#include "types.h"
#include "uart.h"
#include "printf.h"
#include "riscv.h"
#include "virtio.h"

#define READ 0
#define WRITE 1

static char buffer[BSIZE];
static char buffer2[BSIZE];
static int phase = 0;

// in U mode ...
int wait_for_interrupt(){
  printf("U\n");  
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

void pass(){
  printf("P\n");
  while (1);
}

void fail(){
  printf("F\n");
  while (1);
}

void init_buffer(){
  for (int i = 0; i < BSIZE; i++){
    buffer[i] = (char) (i % 0xFF);
  }                     
}

void write_test(){
  // write to sector 1 from buffer
  virtio_disk_rw(1, WRITE, buffer);  
}

void read_test(){
  // read from sector 0 to buffer2
  virtio_disk_rw(1, READ, buffer2);
}

int verify_result(){
  for (int i=0; i < BSIZE; i++)
    if(buffer[i] != buffer2[i])
      return 0;
  return 1;
}

void interrupt(){
  uint32 scause = r_scause();
  uint32 irq = *(uint32*)PLIC_SCLAIM(0);
  if((scause & 0x80000000)
     && (scause & 0xff) == 9
     && irq == VIRTIO0_IRQ){
    while(virtio_used_updated()){
      if (phase == 0){
        read_test();
        printf("5\n");
        phase++;
      } else {
        printf("7\n");
        if(verify_result())
          pass();
        else
          fail();
      }
    }
    *(uint32*)PLIC_SCLAIM(0) = irq;
  }
  jump_to_user((uint32) wait_for_interrupt);
}

// in supervisor mode...  
void smain(){
  // enable S-mode interrupt
  w_sie(r_sie() | SIE_SEIE);
  w_sstatus(r_sstatus() | SSTATUS_SIE);
  w_stvec((uint32)interrupt);
  
  *(uint32*)(PLIC + VIRTIO0_IRQ*4) = 1;
  *(uint32*)PLIC_SENABLE(0) = (1 << VIRTIO0_IRQ);
  *(uint32*)PLIC_SPRIORITY(0) = 0;

  printf("0\n");
  uartinit();

  printf("1\n");
  init_buffer();
  
  printf("2\n");
  virtio_disk_init();
  
  printf("3\n");
  write_test();
  
  printf("4\n");
  jump_to_user((uint32) wait_for_interrupt);
}

void start(){  
  // delegate all interrupts to S-mode
  w_medeleg(0xffff);
  w_mideleg(0xffff);

  // disable paging
  w_satp(0);

  // jump to write_test and go to S-mode
  uint32 x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);
  w_mepc((uint32)smain);  
  asm volatile("mret");  
}
