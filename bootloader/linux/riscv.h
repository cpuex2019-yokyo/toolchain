
static inline void 
w_mepc(uint32 x)
{
  asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline void 
w_satp(uint32 x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline void 
w_mideleg(uint32 x)
{
  asm volatile("csrw mideleg, %0" : : "r" (x));
}

static inline void 
w_medeleg(uint32 x)
{
  asm volatile("csrw medeleg, %0" : : "r" (x));
}

static inline void 
w_stvec(uint32 x)
{
  asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline void 
w_mtvec(uint32 x)
{
  asm volatile("csrw mtvec, %0" : : "r" (x));
}

#define MSTATUS_MPP_MASK (3 << 11) // previous mode.
#define MSTATUS_MPP_M (3 << 11)
#define MSTATUS_MPP_S (1 << 11)
#define MSTATUS_MPP_U (0 << 11)
#define MSTATUS_MIE (1 << 3)    // machine-mode interrupt enable.

static inline uint32
r_mstatus()
{
  uint32 x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void 
w_mstatus(uint32 x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

static inline uint32
r_scause()
{
  uint32 x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

#define SIE_SEIE (1 << 9) // external
static inline uint32
r_sie()
{
  uint32 x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void 
w_sie(uint32 x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

#define SSTATUS_SIE (1 << 1)  // Supervisor Interrupt Enable
static inline uint32
r_sstatus()
{
  uint32 x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void 
w_sstatus(uint32 x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

#define SSTATUS_SPP (1 << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1 << 5) // Supervisor Previous Interrupt Enable

static inline void 
w_sepc(uint32 x)
{
  asm volatile("csrw sepc, %0" : : "r" (x));
}
