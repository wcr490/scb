#include "linux/module.h"
#include "linux/printk.h"
#include "linux/usb.h"
MODULE_LICENSE("GPL");
extern int block_drv_wake(struct usb_device *usbdev);
extern void block_drv_shutdown(void);
static int cz_probe(struct usb_interface *interface,
                    const struct usb_device_id *id) {
  int res = block_drv_wake(interface_to_usbdev(interface));
  if (res < 0) {
    printk(KERN_INFO "fail to wake");
  }
  printk(KERN_INFO "cz880 inserts\n");
  return 0;
}

static void cz_disconnect(struct usb_interface *interface) {
  printk(KERN_INFO "CZ880 driver removed\n");
}

static struct usb_device_id device_table[] = {{USB_DEVICE(0x0781, 0x5588)}, {}};
MODULE_DEVICE_TABLE(usb, device_table);

static struct usb_driver cz_driver = {
    .name = "cz880_driver",
    .id_table = device_table,
    .probe = cz_probe,
    .disconnect = cz_disconnect,
};

static int __init cz_init(void) {
  printk(KERN_INFO "CZ880 driver init");
  return usb_register(&cz_driver);
}

static void __exit cz_exit(void) {
  block_drv_shutdown();
  usb_deregister(&cz_driver);
}

module_init(cz_init);
module_exit(cz_exit);
