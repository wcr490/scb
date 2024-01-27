#ifndef __BLK_DRV_H__
#define __BLK_DRV_H__
#endif // !__BLK_DRV_H__
#include "block_drv_ioctl.h"
#include "main.h"
#define MY_DEVICE_NAME "CZ880"
#define BLK_MAJOR 114
#define BLK_MINOR 0
#define DBGMSG(fmt, ...) printk(MY_DEVICE_NAME ": " fmt, ##__VA_ARGS__)

static int deviceMajorNumb = 0;
static int __init block_drv_init(void);
static void __exit block_drv_exit(void);

static int block_drv_open(struct gendisk *Gendisk, fmode_t Mode);
static void block_drv_release(struct gendisk *GenDisk);
static int block_drv_ioctl(struct block_device *Device, fmode_t Mode,
                           unsigned int Cmd, unsigned long Arg);

static struct block_device_operations block_dev_functions = {
    .owner = THIS_MODULE,
    .open = block_drv_open,
    .release = block_drv_release,
    .ioctl = block_drv_ioctl};

#define DEVICE_BUFFER_SIZE 1024
static struct _block_drv {
  struct usb_device *usbdev;
  char *Buffer;
  int DataLength;
  struct request_queue *devReqQueue;
  struct gendisk *GenDisk;
} dev;
