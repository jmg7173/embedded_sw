#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>

#include <linux/string.h>

#include "device_driver.h"
#include "io_driver.h"

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *, unsigned int, unsigned long);
static void init_devices(void);

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release,
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
    int second_lcd_gap; // lcd second line space
    int second_lcd_move;
}data;

static int dev_usage = 0;
const static char empty_str[] = "                ";
const static char stu_num[] = "20141578";
const static char stu_name[] = "Min Gyo Jung";

static void init_devices(void){
    iom_fpga_dot_write(-1);
    iom_fpga_fnd_write("\0\0\0\0");
    iom_fpga_led_write(0);
    iom_fpga_text_lcd_init();
}

// open
static int device_open(struct inode *inode, struct file *file){
    init_devices();
    if(dev_usage){
        return -EBUSY;
    }
    dev_usage = 1;
    printk("device open success!\n");
    return 0;
}

// close
static int device_release(struct inode *inode, struct file *file){
    // init_devices(); // <- is it necessary?
    dev_usage = 0;
    printk("device release success!\n");
    return 0;
}

// read - nop
static ssize_t device_read(struct file *filp, char *buff, size_t len, loff_t *off){
    printk(KERN_ALERT "This operation isn't supported.\n");
    return -EINVAL;
}


static void timer_iterator(unsigned long timeout){
    struct timer_data *p_data = (struct timer_data*)timeout;
    char lcd_str[33] = {0};
    char fnd[5] = {0};
    int stu_num_len = strlen(stu_num);
    int stu_name_len = strlen(stu_name);

    // fnd
    fnd[p_data->pos] = p_data->cur + 1;
    printk("fnd - %d%d%d%d writing...\n", fnd[0], fnd[1], fnd[2], fnd[3]);
    iom_fpga_fnd_write(fnd);

    // led
    iom_fpga_led_write(1 << (7 - p_data->cur));

    // dot
    iom_fpga_dot_write(p_data->cur + 1);

    // pattern changing
    p_data->cur = (p_data->cur + 1) % 8;
    if(p_data->cur == p_data->start){
        p_data->pos = (p_data->pos + 1) % 4;
    }

    // lcd changing
    strcpy(lcd_str, empty_str);
    strcpy(lcd_str + p_data->first_lcd_gap, stu_num);
    lcd_str[p_data->first_lcd_gap + stu_num_len] = ' ';
    strcpy(lcd_str+16, empty_str);
    strcpy(lcd_str+16 + p_data->second_lcd_gap, stu_name);
    lcd_str[16+p_data->second_lcd_gap + stu_name_len] = ' ';
    lcd_str[32] = '\0';
    iom_fpga_text_lcd_write(lcd_str);

    p_data->first_lcd_gap = p_data->first_lcd_gap + p_data->first_lcd_move;
    p_data->second_lcd_gap = p_data->second_lcd_gap + p_data->second_lcd_move;

    if(p_data->first_lcd_gap == 0)
        p_data->first_lcd_move = 1;
    else if(p_data->first_lcd_gap + stu_num_len == 16)
        p_data->first_lcd_move = -1;

    if(p_data->second_lcd_gap == 0)
        p_data->second_lcd_move = 1;
    else if(p_data->second_lcd_gap + stu_name_len == 16)
        p_data->second_lcd_move = -1;


    p_data->remain--;
    if(p_data->remain <= 0){
        init_devices();
        printk("iteration finished\n");
        return;
    }

    printk("remain: %d\n", p_data->remain);
    // re add to timer
    data.timer.expires = get_jiffies_64() + (p_data->gap * HZ/10);
    data.timer.data = (unsigned long)&data;
    data.timer.function = timer_iterator;
    add_timer(&data.timer);
}


// write
static ssize_t device_write(struct file *filp, const char *gdata, size_t len, loff_t *offset){
    const char *tmp = gdata;
    int val;
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

    data.start = start_val - 1;
    data.pos = start_pos;
    data.remain = times;
    data.cur = start_val - 1;
    data.gap = time_gap;
    data.first_lcd_gap = 0;
    data.second_lcd_gap = 0;
    data.first_lcd_move = 1;
    data.second_lcd_move = 1;

    data.timer.expires = get_jiffies_64() + (time_gap*HZ/10);
    data.timer.function = timer_iterator;
    data.timer.data = (unsigned long)&data;

    add_timer(&data.timer);
    printk("Add timer OK!\n");

    return 1;
}


static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    ssize_t retval = 0;
    printk("ioctl_num: %u\n", ioctl_num);
    printk("IOCTL_START_APP: %u\n", IOCTL_START_APP);
    printk("ioctl_param: %x\n", (unsigned int)ioctl_param);
    switch(ioctl_num){
        case IOCTL_START_APP:
            retval = device_write(file, (char *)ioctl_param, 4, NULL);
            break;
        default:
            return retval;
    }
    return retval;
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
    iom_mapping();

    init_timer(&(data.timer));
    return 0;
}

// driver exit
void dev_driver_exit(void){
    printk("dev_driver exit\n");
    init_devices();
    iom_unmapping();
    dev_usage = 0;
    del_timer_sync(&data.timer);
    unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("Dual BSD/GPL");
