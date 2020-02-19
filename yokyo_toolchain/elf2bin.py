#!/bin/env python

from __future__ import print_function
import sys

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

def process_file(input_filename, output_filename):   
   with open(input_filename, 'rb') as f:
      elffile = ELFFile(f)
      sections_unsorted = []       
      for sec in filter(lambda x: 'text' in x.name or x['sh_addr'] != 0, elffile.iter_sections()):
         sections_unsorted.append((sec['sh_addr'], sec.data(), sec.data_size))
         
   with open(output_filename, 'wb') as f:
      sections = sorted(sections_unsorted, key=lambda x: x[0])      
      for i in range(0, len(sections)):
         current_sec_head, data, size = sections[i]
         if i < len(sections) - 1:
            next_sec_head = sections[i+1][0]
            f.write(data)
            f.write(b'\x00' * (next_sec_head - current_sec_head - size))
         else:
            f.write(data)

      
def main():
   if len(sys.argv) == 3:
      process_file(sys.argv[1], sys.argv[2])
   else:
      print("{} <input> <output>".format(sys.argv[0]))
       
if __name__ == '__main__':
   main()
