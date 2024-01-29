#ifndef __BLK_DRV_H__
#define __BLK_DRV_H__
#include "linux/blk_types.h"
#endif // !__BLK_DRV_H__
#include "main.h"
#define MY_DEVICE_NAME "CZ880"
#define BLK_MAJOR 114
#define BLK_MINOR 0
#define IS_ASYNC false
#define DEBUG 1
#define DBGMSG(fmt, ...) printk(MY_DEVICE_NAME ": " fmt, ##__VA_ARGS__)

static int __init block_drv_init(void);
static void __exit block_drv_exit(void);

static int block_drv_open(struct gendisk *Gendisk, fmode_t Mode);
static void block_drv_release(struct gendisk *GenDisk);

typedef enum {
  COMPLETE,
  EXIT_ACC,
  TIMEOUT,
} ReplyType;
/* a set of ret from the action of queue
 * which is always attached to the tail of the request that it describe
 * related to the function: blk_mq_rq_to_pdu()
 */
struct cz_rq_ret {
  /* req data */
  unsigned int timeout;
  ReplyType *reply;
  /* dev data */
  struct bio *bio;
  struct device *dev;
};

static struct block_device_operations block_dev_functions = {
    .owner = THIS_MODULE,
    .open = block_drv_open,
    .release = block_drv_release,
};

#define DEVICE_BUFFER_SIZE 1024
#define NUM_OF_VECTORS 1
static struct _block_drv {
  struct usb_device *usbdev;
  char *Buffer;
  int DataLength;
  struct request_queue *devReqQueue;
  struct gendisk *GenDisk;
} dev;

static struct bio *bio_build(blk_opf_t opf);

static blk_status_t cz_queue_rq(struct blk_mq_hw_ctx *ctx,
                                const struct blk_mq_queue_data *data);
static void cz_complete(struct request *rq);

static int cz_init_rq(struct blk_mq_tag_set *set, struct request *rq,
                      unsigned int hctx_idx, unsigned int node);
static void cz_cleanup_rq(struct request *);
static struct bio_data *register_bio(void);
static struct request_queue *register_queue(void);
static void unregister_queue(struct request_queue *queue);
static int bio_push(struct bio *bio);
/* enum req_op {
        REQ_OP_READ = 0,
        REQ_OP_WRITE = 1,
        REQ_OP_FLUSH = 2,
        REQ_OP_DISCARD = 3,
        REQ_OP_SECURE_ERASE = 5,
        REQ_OP_WRITE_ZEROES = 9,
        REQ_OP_ZONE_OPEN = 10,
        REQ_OP_ZONE_CLOSE = 11,
        REQ_OP_ZONE_FINISH = 12,
        REQ_OP_ZONE_APPEND = 13,
        REQ_OP_ZONE_RESET = 15,
        REQ_OP_ZONE_RESET_ALL = 17,
        REQ_OP_DRV_IN = 34,
        REQ_OP_DRV_OUT = 35,
        REQ_OP_LAST = 36,
};
 */
