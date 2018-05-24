#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>

#include <string.h>

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

static struct timer_data{
    struct timer_list timer;
    int start;          // pattern
    int cur;            // cur pattern
    int pos;            // fnd position
    int remain;         // iter remain
    int gap;            // time gap
    int first_lcd_gap;  // lcd first line space
    int first_lcd_move;
    int second_lcd gap; // lcd second line space
    int second_lcd_move;
};

struct timer_data data;

static int dev_usage = 0;
const static char empty_str[] = "                ";
const static char stu_num[] = "20141578"
const static char stu_name[] = "Min Gyo Jung"
const int stu_num_len = strlen(stu_num);
const int stu_name_len = strlen(stu_name);

// open
static int device_open(struct inode *inode, struct file *file){
    if(dev_usage){
        return -EBUSY;
    }
    dev_usage = 1;
    return 0;
}

// close
static int device_release(struct inode *inode, struct file *file){
    dev_usage = 0;
    return 0;
}

// read - nop
static ssize_t device_read(struct file *filp, char *buff, size_t len, loff_t *off){
    printk(KERN_ALERT "This operation isn't supported.\n");
    return -EINVAL;
}


static void timer_iterator(unsigned long timeout){
    struct timer_data *p_data = (struct timer_data*)timeout;
    int out_str;
    char lcd_str[33];
    // do something

    // pattern changing
    p_data->cur = (p_data->cur + 1) % 8 + 1;
    if(p_data->cur == p_data->start){
        p_data->pos = (p_data->pos + 3) % 4 + 1;
    }
    
    // lcd changing
    
    strcpy(lcd_str, empty_str);
    strcpy(lcd_str+p_data->first_lcd_gap, stu_num);
    strcpy(lcd_str+16, empty_str);
    strcpy(lcd_str+16+p_data->second_lcd_gap, stu_name);
    if(p_data->first_lcd_move == 1){
    }
    else{ // p_data->first_lcd_move == -1
    }

    if(p_data->second_lcd_move == -1){
    }
    else{ // p_data->second_lcd_move == -1
    }


    p_data->remain--;
    if(p_data->remain <= 0){
        // need initialize
        return;
    }

    data.timer.expires = get_jiffies_64() + (p_data->gap * HZ/10);
    data.timer.data = (unsigned long)&data;
    data.timer.function = timer_iterator;
    add_timer(&mydata.timer);
}


// write
static ssize_t device_write(struct file *filp, const char *gdata, size_t len, loff_t *offset){
    const char *tmp = gdata;
    int val; = *(int *)gdata;
    int start_pos;
    int start_val;
    int time_gap;
    int times;

    if(copy_from_user(&val, tmp, 4)){
        return -EFAULT;
    }

    start_pos = (val & 0xFF000000) >> 24;
    start_val = (val & 0xFF0000) >> 16;
    time_gap = (val & 0xFF00) >> 8;
    times = (val & 0xFF);

    printk("start position: %d\n", start_pos);
    printk("start pattern : %d\n", start_val);
    printk("time gap      : %d\n", time_gap);
    printk("iteration     : %d\n", times);

    del_timer_sync(&data.timer);

    data.start = start_val;
    data.pos = start_pos;
    data.remain = times;
    data.cur = start_val;
    data.gap = time_gap;
    data.first_lcd_gap = 0;
    data.second_lcd_gap = 0;
    data.first_lcd_move = 1;
    data.second_lcd_move = 1;

    data.timer.expires = get_jiffies_64() + (time_gap*HZ/10);
    data.timer.function = timer_iterator;
    data.timer.data = (unsigned long)&data;

    add_timer(&mydata.timer);

    return 1;
}

// driver init
int dev_driver_init(void){
    int result;
    result = register_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME, &fops);
    if (result < 0){
        printk(KERN_WARNING"Can't get major number as %d\n", DEV_DRIVER_MAJOR);
        return result;
    }
    printk("init module, %s major number: %d\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);

    init_timer(&(data.timer));
    return 0;
}

// driver exit
void dev_driver_exit(void){
    printk("dev_driver exit\n");
    dev_usage = 0;
    del_timer_sync(&data.timer);
    unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("Dual BSD/GPL");
