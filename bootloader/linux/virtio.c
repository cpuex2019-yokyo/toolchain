#include "const.h"
#include "virtio.h"
#include "types.h"
#include "uart.h"
#include "printf.h"
#include "riscv.h"
#include "virtio.h"

#define R(r) ((volatile uint32 *)(VIRTIO0 + (r)))
disk_t *disk = (disk_t *) 0x82004000;

int
virtio_used_updated(void){
  if ((disk->used_idx % NUM) != (disk->used->id % NUM)){
    disk->used_idx++;
    return 1;
  } else {
    return 0;
  }
}

void
virtio_disk_init(void)
{
  uint32 status = 0;

  if(*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
     *R(VIRTIO_MMIO_VERSION) != 1 ||
     *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
     *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551){
    panic("could not find virtio disk");
  }
  
  status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_STATUS) = status;

  status |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_STATUS) = status;

  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  status |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  *R(VIRTIO_MMIO_GUEST_PAGE_SIZE) = PGSIZE;

  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;
  uint32 max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
  if(max == 0)
    panic("virtio disk has no queue 0");
  if(max < NUM)
    panic("virtio disk max queue too short");
  *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;

  // memset
  uint32 *cdst = (uint32 *) disk->pages;
  int i;
  for(i = 0; i < (2 * PGSIZE)/sizeof(uint32); i++){
    cdst[i] = 0;
  }
  
  *R(VIRTIO_MMIO_QUEUE_PFN) = ((uint32)disk->pages) >> PGSHIFT;

  disk->desc = (struct VRingDesc *) disk->pages;
  disk->avail = (uint16 *)(((char* )disk->desc) + NUM * sizeof(struct VRingDesc));
  disk->used = (struct UsedArea *) (disk->pages + PGSIZE);

  for(int i = 0; i < NUM; i++)
    disk->free[i] = 1;
}

void
virtio_disk_rw(int sector, int write, char* buffer)
{
  int idx[3];
  for(int i = 0; i < 3; i++){
    disk->free[i] = 0;
    idx[i] = i;
  }

  struct virtio_blk_outhdr {
    uint32 type;
    uint32 reserved;
    uint64 sector;
  } buf0;

  if(write)
    buf0.type = VIRTIO_BLK_T_OUT;
  else
    buf0.type = VIRTIO_BLK_T_IN;
  buf0.reserved = 0;
  buf0.sector = sector;

  disk->desc[idx[0]].addr = (uint64) (uint32) &buf0;
  disk->desc[idx[0]].len = sizeof(buf0);
  disk->desc[idx[0]].flags = VRING_DESC_F_NEXT;
  disk->desc[idx[0]].next = idx[1];

  disk->desc[idx[1]].addr = (uint64) (uint32) buffer;
  disk->desc[idx[1]].len = SECTOR_SIZE;
  if(write)
    disk->desc[idx[1]].flags = 0;
  else
    disk->desc[idx[1]].flags = VRING_DESC_F_WRITE;
  disk->desc[idx[1]].flags |= VRING_DESC_F_NEXT;
  disk->desc[idx[1]].next = idx[2];

  disk->info[idx[0]].status = 0;
  disk->desc[idx[2]].addr = (uint64) (uint32) &disk->info[idx[0]].status;
  disk->desc[idx[2]].len = 1;
  disk->desc[idx[2]].flags = VRING_DESC_F_WRITE;
  disk->desc[idx[2]].next = 0;
  
  disk->avail[2 + (disk->avail[1] % NUM)] = idx[0];
  disk->avail[1] = disk->avail[1] + 1;
  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0;
}
