#ifndef __MAIN_H__
#define __MAIN_H__
#endif

#include "linux/export.h"
#include <linux/blkdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/usb.h>

int block_drv_wake(struct usb_device *usbdev);
void block_drv_shutdown(void);
