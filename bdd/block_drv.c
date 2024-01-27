#include "block_drv.h"
#include "linux/blkdev.h"
#include "linux/export.h"
#include "linux/module.h"
#include "linux/printk.h"
#include "linux/slab.h"
#include "linux/usb.h"

MODULE_LICENSE("Dual MIT/GPL");
void block_drv_shutdown(void) {
  usb_put_dev(dev.usbdev);
  return;
}
int block_drv_wake(struct usb_device *usbdev) {
  DBGMSG("block wakes\n");
  struct block_device *blkdev = blkdev_get_by_dev(
      MKDEV(BLK_MAJOR, BLK_MINOR), FMODE_READ | FMODE_WRITE, NULL, NULL);
  if (blkdev == NULL) {
    return -1;
  }
  if (IS_ERR(blkdev)) {
    return -1;
  }
  dev.usbdev = usbdev;
  dev.GenDisk = blk_alloc_disk(1);

  if (dev.GenDisk == NULL) {
    DBGMSG("alloc_disk failed\n");
    return -1;
  }
  dev.GenDisk->major = BLK_MAJOR;
  dev.GenDisk->first_minor = 0;
  dev.GenDisk->minors = BLK_MINOR;
  dev.GenDisk->fops = &block_dev_functions;
  dev.GenDisk->flags = GENHD_FL_NO_PART;

  strcpy(dev.GenDisk->disk_name, MY_DEVICE_NAME "0");
  set_capacity(dev.GenDisk, 1024 * 512);

  int add_disk_res = add_disk(dev.GenDisk);
  if (add_disk_res < 0) {
    DBGMSG("add_disk failed\n");
    return -1;
  }

  dev.Buffer = kzalloc(DEVICE_BUFFER_SIZE, GFP_KERNEL);

  dev.DataLength = 0;

  return 0;
}
EXPORT_SYMBOL(block_drv_wake);
EXPORT_SYMBOL(block_drv_shutdown);

static int __init block_drv_init(void) {
  DBGMSG("block_drv_init called\n");
  DBGMSG("block_drv_init comleted\n");
  return 0;
}

static void __exit block_drv_exit(void) {
  DBGMSG("block_drv_exit called\n");
  kfree(dev.Buffer);
  printk(KERN_INFO "kfree is fine");
  put_disk(dev.GenDisk);
  unregister_blkdev(deviceMajorNumb, MY_DEVICE_NAME);

  DBGMSG("block_drv_exit comleted\n");
}

static int block_drv_open(struct gendisk *Gendisk, fmode_t Mode) {
  DBGMSG("block_drv_open called\n");
  return 0;
}

static void block_drv_release(struct gendisk *GenDisk) {
  DBGMSG("block_drv_release called\n");
}

static int block_drv_ioctl(struct block_device *Device, fmode_t Mode,
                           unsigned int Cmd, unsigned long Arg) {
  DBGMSG("block_drv_ioctl called\n");

  block_drv_ioctl_data *data = (block_drv_ioctl_data *)Arg;

  switch (Cmd) {
  case IOCTL_BLOCK_DRV_GET:
    DBGMSG("IOCTL_BLOCK_DRV_GET\n");
    {
      size_t length = dev.DataLength > data->OutputLength ? data->OutputLength
                                                          : dev.DataLength;
      int copy_to_res = copy_to_user(data->OutputData, dev.Buffer, length);
      if (copy_to_res < 0) {
        return -1;
      }
    }
    break;

  case IOCTL_BLOCK_DRV_SET:
    DBGMSG("IOCTL_BLOCK_DRV_SET\n");
    {
      size_t length = DEVICE_BUFFER_SIZE > data->InputLength
                          ? data->InputLength
                          : DEVICE_BUFFER_SIZE;
      dev.DataLength = length;

      memset(dev.Buffer, 0, DEVICE_BUFFER_SIZE);
      int copy_from_res = copy_from_user(dev.Buffer, data->InputData, length);
      if (copy_from_res < 0) {
        return -1;
      }
      DBGMSG("Data from user: %s", dev.Buffer);
    }
    break;

  case IOCTL_BLOCK_DRV_GET_AND_SET:
    DBGMSG("IOCTL_BLOCK_DRV_GET_AND_SET\n");
    {
      size_t outputLength, inputLength;

      outputLength = dev.DataLength > data->OutputLength ? data->OutputLength
                                                         : dev.DataLength;
      inputLength = DEVICE_BUFFER_SIZE > data->InputLength ? data->InputLength
                                                           : DEVICE_BUFFER_SIZE;

      int copy_to_res = copy_to_user(data->OutputData, dev.Buffer, inputLength);
      if (copy_to_res < 0) {
        return -1;
      }
      memset(dev.Buffer, 0, DEVICE_BUFFER_SIZE);
      int copy_from_res =
          copy_from_user(dev.Buffer, data->InputData, inputLength);
      if (copy_from_res < 0) {
        return -1;
      }
      DBGMSG("Data from user: %s", dev.Buffer);

      dev.DataLength = inputLength;
    }
    break;

  case IOCTL_BLOCK_DBG_MESSAGE:
    DBGMSG("IOCTL_BLOCK_DBG_MESSAGE\n");
    { DBGMSG("Device buffer conten: %s\n", dev.Buffer); }
    break;
  }
  return 0;
}

module_init(block_drv_init);
module_exit(block_drv_exit);
