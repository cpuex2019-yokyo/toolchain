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

#pragma once
#include "types.h"

#define VIRTIO_MMIO_MAGIC_VALUE		0x000
#define VIRTIO_MMIO_VERSION		0x004
#define VIRTIO_MMIO_DEVICE_ID		0x008
#define VIRTIO_MMIO_VENDOR_ID		0x00c
#define VIRTIO_MMIO_DEVICE_FEATURES	0x010
#define VIRTIO_MMIO_DRIVER_FEATURES	0x020
#define VIRTIO_MMIO_GUEST_PAGE_SIZE	0x028
#define VIRTIO_MMIO_QUEUE_SEL		0x030
#define VIRTIO_MMIO_QUEUE_NUM_MAX	0x034
#define VIRTIO_MMIO_QUEUE_NUM		0x038
#define VIRTIO_MMIO_QUEUE_ALIGN		0x03c
#define VIRTIO_MMIO_QUEUE_PFN		0x040
#define VIRTIO_MMIO_QUEUE_READY		0x044
#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050
#define VIRTIO_MMIO_INTERRUPT_STATUS	0x060
#define VIRTIO_MMIO_INTERRUPT_ACK	0x064
#define VIRTIO_MMIO_STATUS		0x070

#define VIRTIO_CONFIG_S_ACKNOWLEDGE	1
#define VIRTIO_CONFIG_S_DRIVER		2
#define VIRTIO_CONFIG_S_DRIVER_OK	4
#define VIRTIO_CONFIG_S_FEATURES_OK	8

#define VIRTIO_BLK_F_RO              5
#define VIRTIO_BLK_F_SCSI            7
#define VIRTIO_BLK_F_CONFIG_WCE     11
#define VIRTIO_BLK_F_MQ             12
#define VIRTIO_F_ANY_LAYOUT         27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX     29

#define NUM 8

#define VIRTIO_BLK_T_IN  0
#define VIRTIO_BLK_T_OUT 1

#define VRING_DESC_F_NEXT  1
#define VRING_DESC_F_WRITE 2

#define BSIZE 1024

void virtio_disk_init(void);
void virtio_disk_rw(int, int, char*);

struct VRingDesc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 next;
};

struct VRingUsedElem {
  uint32 id;
  uint32 len;
};

struct UsedArea {
  uint16 flags;
  uint16 id;
  struct VRingUsedElem elems[NUM];
};

struct buf {
  int valid;
  int disk;
  uint dev;
  uint blockno;
  uint refcnt;
  struct buf *prev;
  struct buf *next;
  struct buf *qnext;
  uchar data[1024];
};

typedef struct _disk_t {
  char pages[2*PGSIZE];
  struct VRingDesc *desc;
  uint16 *avail;
  struct UsedArea *used;

  char free[NUM];
  uint16 used_idx;

  struct {
    struct buf *b;
    char status;
  } info[NUM];
    
} __attribute__ ((aligned (PGSIZE))) disk_t;

int virtio_used_updated(void);
