#ifndef __BLK_DRV_IOCTL_H__
#define __BLK_DRV_IOCTL_H__

#include "main.h"
#endif // !__BLK_DRV_IOCTL_H__
#define BLOCK_DRV_MAGIC 0xB2

#define BLOCK_DRV_GET 0x70
#define BLOCK_DRV_SET 0x71
#define BLOCK_DRV_GET_AND_SET 0x72
#define BLOCK_DRV_DBG_MESSAGE 0x73

typedef struct _block_drv_ioctl_data {
  char *InputData;
  size_t InputLength;
  char *OutputData;
  size_t OutputLength;
} block_drv_ioctl_data;

#define IOCTL_BLOCK_DRV_GET _IOR(BLOCK_DRV_MAGIC, BLOCK_DRV_GET, char *)
#define IOCTL_BLOCK_DRV_SET _IOW(BLOCK_DRV_MAGIC, BLOCK_DRV_SET, char *)
#define IOCTL_BLOCK_DRV_GET_AND_SET                                            \
  _IOWR(BLOCK_DRV_MAGIC, BLOCK_DRV_GET_AND_SET, char *)
#define IOCTL_BLOCK_DBG_MESSAGE _IO(BLOCK_DRV_MAGIC, BLOCK_DRV_DBG_MESSAGE)
