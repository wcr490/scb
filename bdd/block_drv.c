#include "block_drv.h"
#include "linux/bio.h"
#include "linux/blk-mq.h"
#include "linux/blk_types.h"
#include "linux/blkdev.h"
#include "linux/gfp.h"
#include "linux/kdev_t.h"
#include "linux/minmax.h"
#include "linux/mm.h"
#include "linux/printk.h"
#include "linux/slab.h"

static int test(void) {
  // full the buffer interface later
  dev.Buffer = "a";
  struct bio *bio = bio_build(REQ_OP_WRITE);
  bio_push(bio);
  int idx = 0;
  struct blk_mq_hw_ctx *hctx = xa_load(&dev.devReqQueue->hctx_table, idx);
  blk_mq_run_hw_queue(hctx, IS_ASYNC);
  return 0;
}
static const struct blk_mq_ops cz_mq_ops = {
    .queue_rq = cz_queue_rq,
    .complete = cz_complete,
    .init_request = cz_init_rq,
    .cleanup_rq = cz_cleanup_rq,
};
MODULE_LICENSE("Dual MIT/GPL");
void block_drv_shutdown(void) {
  usb_put_dev(dev.usbdev);
  return;
}
int block_drv_wake(struct usb_device *usbdev) {
  DBGMSG("block wakes\n");
  struct block_device *blkdev =
      blkdev_get_by_dev(MKDEV(BLK_MAJOR, dev.GenDisk->first_minor),
                        FMODE_READ | FMODE_WRITE, NULL, NULL);
  if (blkdev == NULL) {
    printk(KERN_ERR "get the blkdev: fail\n");
    return -1;
  }
  if (IS_ERR(blkdev)) {
    printk(KERN_ERR "get the blkdev: fail\n");
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
  dev.GenDisk->minors = 1;
  dev.GenDisk->fops = &block_dev_functions;
  dev.GenDisk->flags = GENHD_FL_NO_PART;
  dev.devReqQueue = register_queue();
  strcpy(dev.GenDisk->disk_name, MY_DEVICE_NAME "0");
  set_capacity(dev.GenDisk, 1024 * 512);
  int add_disk_res = add_disk(dev.GenDisk);
  if (add_disk_res < 0) {
    DBGMSG("add_disk failed\n");
    return -1;
  }
  dev.Buffer = kzalloc(DEVICE_BUFFER_SIZE, GFP_KERNEL);
  dev.DataLength = 0;

  DBGMSG("start to test\n");
  test();

  return 0;
}
EXPORT_SYMBOL(block_drv_wake);
EXPORT_SYMBOL(block_drv_shutdown);

static blk_status_t cz_queue_rq(struct blk_mq_hw_ctx *ctx,
                                const struct blk_mq_queue_data *data) {
  struct request *rq = data->rq;
  struct bio *bio;
  __rq_for_each_bio(bio, rq) {
    struct bio_vec bv;
    struct bvec_iter iter;
    bio_for_each_segment(bv, bio, iter) {
      char *addr = (char *)page_address(bv.bv_page) + bv.bv_offset;
      int len = min(dev.DataLength, bv.bv_len);
      memcpy(addr, dev.Buffer, len);
    }
  }
  return BLK_STS_OK;
}
static void cz_complete(struct request *rq) {
  struct cz_rq_ret *ret = blk_mq_rq_to_pdu(rq);
  switch (*ret->reply) {
  case COMPLETE:
    return;
  case TIMEOUT: {
    printk(KERN_ERR "an error occurred in the last request: Timeout\n");
    return;
  }
  case EXIT_ACC: {
    printk(KERN_ERR
           "an error occurred in the last request: exit accidentally\n");
    return;
  }
  }
  return;
}
static int cz_init_rq(struct blk_mq_tag_set *set, struct request *rq,
                      unsigned int hctx_idx, unsigned int node) {
  struct cz_rq_ret *ret = blk_mq_rq_to_pdu(rq);
  ReplyType *reply = kzalloc(sizeof(ReplyType), GFP_KERNEL);
  memset(ret, 0, sizeof(*ret));
  ret->reply = reply;
  return 0;
}
static void cz_cleanup_rq(struct request *rq) {
  if (rq != NULL) {
    kfree(blk_mq_rq_to_pdu(rq));
    kfree(rq);
  }
  return;
}
static int __init block_drv_init(void) {
  DBGMSG("block_drv_init called\n");
  int res = register_blkdev(BLK_MAJOR, MY_DEVICE_NAME);
  if (res < 0) {
    printk(KERN_ERR "register a blkdev:fail\n");
  }
  DBGMSG("block_drv_init comleted\n");
  return 0;
}

static void __exit block_drv_exit(void) {
  DBGMSG("block_drv_exit called\n");
  kfree(dev.Buffer);
  unregister_queue(dev.devReqQueue);
  put_disk(dev.GenDisk);
  unregister_blkdev(BLK_MAJOR, MY_DEVICE_NAME);

  DBGMSG("block_drv_exit comleted\n");
}

static struct request_queue *register_queue(void) {
  return kmalloc(sizeof(struct request_queue), GFP_KERNEL);
}
static void unregister_queue(struct request_queue *queue) {
  kfree(queue);
  return;
}
static int bio_push(struct bio *bio) {
  submit_bio(bio);
  if (DEBUG) {
    printk(KERN_INFO "push a bio:successs\n");
  }
  return 0;
}
static struct bio *bio_build(blk_opf_t opf) {
  struct block_device *bdev =
      blkdev_get_by_dev(MKDEV(BLK_MAJOR, dev.GenDisk->first_minor),
                        FMODE_READ | FMODE_WRITE, NULL, NULL);
  struct bio *bio = bio_alloc(bdev, NUM_OF_VECTORS, opf, GFP_KERNEL);
  // consider to change the Buffer into a page in struct dev
  struct page *page = alloc_page(GFP_KERNEL);
  if (!page) {
    printk(KERN_ERR "alloc a page:fail\n");
    return NULL;
  }
  // don't have offset yet
  unsigned int offset = 0;
  memcpy(page, dev.Buffer, min(PAGE_SIZE, dev.DataLength));
  bio_add_page(bio, page, min(PAGE_SIZE, dev.DataLength), offset);
  if (DEBUG) {
    printk(KERN_INFO "alloc a bio: success\n");
  }
  return bio;
}
static int block_drv_open(struct gendisk *Gendisk, fmode_t Mode) {
  DBGMSG("block_drv_open called\n");
  return 0;
}
static void block_drv_release(struct gendisk *GenDisk) {
  DBGMSG("block_drv_release called\n");
}

module_init(block_drv_init);
module_exit(block_drv_exit);
