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
