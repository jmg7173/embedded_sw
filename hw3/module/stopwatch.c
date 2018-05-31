#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

#include <linux/version.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include <mach/gpio.h>

#define DEV_MAJOR_NUM 242
#define DEV_MINOR_NUM 0
#define DEFAULT_SEC 1

#define FPGA_FND_ADDRESS 0x08000004

void iom_fpga_fnd_write(const char *);
irqreturn_t inter_start(int, void*, struct pt_regs*);
irqreturn_t inter_pause(int, void*, struct pt_regs*);
irqreturn_t inter_reset(int, void*, struct pt_regs*);
irqreturn_t inter_quit(int, void*, struct pt_regs*);

static void timer_iterator(unsigned long timeout);
static void timer_quit(unsigned long timeout);
static int stopwatch_open(struct inode *, struct file *);
static int stopwatch_release(struct inode *, struct file *);
static int stopwatch_write(struct file *, const char *, size_t , loff_t *);


static dev_t stopwatch_dev;
static struct cdev stopwatch_cdev;

static unsigned char *iom_fpga_fnd_addr;
static int dev_usage = 0;

wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);

static struct file_operations stopwatch_fops = {
    .open = stopwatch_open,
    .write = stopwatch_write,
    .release = stopwatch_release,
};

static struct timer_data{
    struct timer_list timer;
    int seconds;
}data, data2;

void iom_fpga_fnd_write(const char *nums){
    unsigned short int val;
    val = nums[0] << 12 | nums[1] << 8 | nums[2] << 4 | nums[3];
    outw(val, (unsigned int)iom_fpga_fnd_addr);
}

irqreturn_t inter_start(int irq, void* dev_id, struct pt_regs* reg){
    del_timer_sync(&data.timer);

    data.timer.expires = get_jiffies_64() + DEFAULT_SEC * HZ;
    data.timer.function = timer_iterator;
    data.timer.data = (unsigned long)&data;

    add_timer(&data.timer);
    printk("Start stopwatch!\n");
    return IRQ_HANDLED;
}

irqreturn_t inter_pause(int irq, void* dev_id, struct pt_regs* reg){
    del_timer_sync(&data.timer);
    printk("Pause stopwatch!\n");
    return IRQ_HANDLED;
}

irqreturn_t inter_reset(int irq, void* dev_id, struct pt_regs* reg){
    del_timer_sync(&data.timer);
    data.seconds = 0;
    printk("Reset stopwatch!\n");
    iom_fpga_fnd_write("\0\0\0\0");
    return IRQ_HANDLED;
}


irqreturn_t inter_quit(int irq, void* dev_id, struct pt_regs* reg){
    if(!gpio_get_value(IMX_GPIO_NR(5, 14))){
        del_timer_sync(&data2.timer);
        data2.seconds = 2;
        
        data2.timer.expires = get_jiffies_64() + DEFAULT_SEC * HZ;
        data2.timer.function = timer_quit;
        data2.timer.data = (unsigned long)&data2;

        add_timer(&data2.timer);
    }
    else{
        del_timer_sync(&data2.timer);
    }

    return IRQ_HANDLED;
}

static void timer_iterator(unsigned long timeout){
    struct timer_data *p_data = (struct timer_data*)timeout;
    char fnd[5] = {0};
    int seconds, minutes;

    seconds = p_data->seconds % 60;
    minutes = p_data->seconds / 60;
    fnd[0] = minutes / 10;
    fnd[1] = minutes % 10;
    fnd[2] = seconds / 10;
    fnd[3] = seconds % 10;
    iom_fpga_fnd_write(fnd);

    p_data->seconds++;

    data.timer.expires = get_jiffies_64() + DEFAULT_SEC * HZ;
    data.timer.data = (unsigned long)&data;
    data.timer.function = timer_iterator;
    add_timer(&data.timer);
}

static void timer_quit(unsigned long timeout){
    struct timer_data *p_data = (struct timer_data*)timeout;
    printk("remain: %d", p_data->seconds);
    if(!gpio_get_value(IMX_GPIO_NR(5, 14)) && --(p_data->seconds) == 0){
        iom_fpga_fnd_write("\0\0\0\0");

        data.seconds = 0;
        del_timer_sync(&data.timer);
        
        printk("Quit button pressed 3 seconds\n");
        __wake_up(&wq_write, 1, 1, NULL);
        return;
    }

    if(!gpio_get_value(IMX_GPIO_NR(5, 14))){
        data2.timer.expires = get_jiffies_64() + DEFAULT_SEC * HZ;
        data2.timer.data = (unsigned long)&data2;
        data2.timer.function = timer_quit;
        add_timer(&data2.timer);
    }
}

static int stopwatch_open(struct inode *inode, struct file *file){
    int ret;
    int irq;

    if(dev_usage)
        return -EBUSY;

    dev_usage = 1;

    printk("Open module!\n");

    data.seconds = 0;

    // interrupt - home
    gpio_direction_input(IMX_GPIO_NR(1, 11));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
    printk("Home btn IRQ number: %d\n", irq);
    ret = request_irq(irq, inter_start, IRQF_TRIGGER_FALLING, "home", 0);

    // interrupt - back
    gpio_direction_input(IMX_GPIO_NR(1, 12));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 12));
    printk("Back btn IRQ number: %d\n", irq);
    ret = request_irq(irq, inter_pause, IRQF_TRIGGER_FALLING, "back", 0);

    // interrupt - vol up reset
    gpio_direction_input(IMX_GPIO_NR(2, 15));
    irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
    printk("Vol up btn IRQ number: %d\n", irq);
    ret = request_irq(irq, inter_reset, IRQF_TRIGGER_FALLING, "volup", 0);

    // interrupt - vol down quit
    gpio_direction_input(IMX_GPIO_NR(5, 14));
    irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
    printk("Vol dn btn IRQ number: %d\n", irq);
    ret = request_irq(irq, inter_quit, IRQF_TRIGGER_FALLING, "voldown", 0);

    return 0;
}

static int stopwatch_release(struct inode *inode, struct file *file){
    dev_usage = 0;

    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

    data.seconds = 0;

    printk("Release Module\n");
    return 0;
}

static int stopwatch_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos ){
    printk("sleep on\n");
    interruptible_sleep_on(&wq_write);

    printk("write\n");
    return 0;
}

static int stopwatch_register_cdev(void){
    int error;
    stopwatch_dev = MKDEV(DEV_MAJOR_NUM, DEV_MINOR_NUM);
    error = register_chrdev_region(stopwatch_dev, 1, "stopwatch");

    if(error < 0){
        printk(KERN_WARNING "stopwatch: can't get major %d\n", DEV_MAJOR_NUM);
        return error;
    }
    printk("major number = %d\n", DEV_MAJOR_NUM);

    cdev_init(&stopwatch_cdev, &stopwatch_fops);
    stopwatch_cdev.owner = THIS_MODULE;
    stopwatch_cdev.ops = &stopwatch_fops;
    error = cdev_add(&stopwatch_cdev, stopwatch_dev, 1);
    if(error){
        printk("stopwatch Register Error %d\n", error);
    }
    return 0;
}

int __init stopwatch_init(void){
    int result;
    if((result = stopwatch_register_cdev()) < 0)
        return result;
    printk("Init Module Success \n");
    printk("Device : /dev/stopwatch, Major num : %d\n", DEV_MAJOR_NUM);

    init_timer(&(data.timer));
    init_timer(&(data2.timer));

    iom_fpga_fnd_addr = ioremap(FPGA_FND_ADDRESS, 0x4);
    return 0;
}

void __exit stopwatch_exit(void){
    dev_usage = 0;

    iounmap(iom_fpga_fnd_addr);
    del_timer_sync(&data.timer);
    del_timer_sync(&data2.timer);

    cdev_del(&stopwatch_cdev);
    unregister_chrdev_region(stopwatch_dev, 1);
    printk("Remove module success! \n");
}

module_init(stopwatch_init);
module_exit(stopwatch_exit);
MODULE_LICENSE("GPL");

