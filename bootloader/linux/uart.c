#include "const.h"
#include "types.h"

#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

#define RHR 0
#define THR 0
#define IER 4
#define FCR 8
#define ISR 8
#define LCR 12
#define LSR 20

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

void
uartinit(void)
{
  WriteReg(IER, 0x00);
  WriteReg(LCR, 0x80);  
  WriteReg(LCR, 0x03);
  WriteReg(FCR, 0x07);
  WriteReg(IER, 0x01);
}

void
uartputc(int c){
  while((ReadReg(LSR) & (1 << 5)) == 0)
    ;
  WriteReg(THR, c);
}

int
uartgetc(void)
{
  if(ReadReg(LSR) & 0x01){
    return ReadReg(RHR);
  } else {
    return -1;
  }
}

#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

void
consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else {
    uartputc(c);
  }
}
