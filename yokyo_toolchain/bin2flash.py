#!/bin/env python

from __future__ import print_function
import sys

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

def chunks(l, n):
   for i in range(0, len(l), n):
      yield l[i:i + n]

def process_file(input_filename, output_filename, sector_size):
   data = b''
   with open(input_filename, 'rb') as f:
      data = f.read()

   i = 0
   with open(output_filename, 'wb') as f:
      for chunk in chunks(data, 512):
         f.write(chunk + b'\x00' * (sector_size - len(chunk)))
         i += 1
      
   print("#chunks: {}".format(i))         
      
def main():
   if len(sys.argv) == 4:
      process_file(sys.argv[1], sys.argv[2], int(sys.argv[3]))
   else:
      print("{} <input> <output>".format(sys.argv[0]))
       
if __name__ == '__main__':
   main()
