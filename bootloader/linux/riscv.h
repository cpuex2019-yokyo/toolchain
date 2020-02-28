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
