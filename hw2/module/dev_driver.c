#include <linux/kernel.h>
#include <linux/module.h>
#include <linux.fs.h>
#include <asm/uaccess.h>

#include "io_driver.h"
#define DEV_DRIVER_MAJOR 242
#define DEV_DRIVER_NAME "dev_driver"

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .ioctl = device_ioctl
};


// driver init
int dev_driver_init(void){
    int result;
    result = register_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME, &fops);
    if (result < 0){
        printk(KERN_WARNING"Can't get major number as %d\n", DEV_DRIVER_MAJOR);
        return result;
    }
    printk("init module, %s major number: %d\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);

    return 0;
}

// driver exit
void dev_driver_exit(void){
    unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("Dual BSD/GPL");
