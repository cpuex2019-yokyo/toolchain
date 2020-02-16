#!/bin/env python

import sys
import struct

template = '''memory_initialization_radix=16;
memory_initialization_vector={};
radix=16;'''

def bin2coe(fname):
   with open(fname, 'rb') as f:
      content = f.read()
   unpacked_content = ["{:02x}".format(ord(content[i])) for i in range(0,len(content))]
   print(template.format(','.join(unpacked_content)))

def main():
   if len(sys.argv) < 2:
      print("{} <bin file>".format(sys.argv[0]))
      exit(1)
   bin2coe(sys.argv[1])
      
if __name__ == '__main__':
   main()
