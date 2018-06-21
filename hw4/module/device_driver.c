#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include <mach/gpio.h>

#include "io_driver.h"

#define INIT_MODE 0
#define COUNT_MODE 1
#define TIMER_MODE 2
#define DOT_CLEAR -1
#define DEV_MAJOR_NUM 242
#define DEV_NAME "dev_driver"


static int driver_open(struct inode *, struct file *);
static int driver_release(struct inode *, struct file *);

static void timer_handler(unsigned long);
irqreturn_t inter_mode_chg(int, void*, struct pt_regs*);
irqreturn_t inter_timer_up(int, void*, struct pt_regs*);
irqreturn_t inter_timer_start(int, void*, struct pt_regs*);

static int dev_usage = 0;
static int mode = 1;
static int count = 0;

static struct file_operations fops = {
    .open = driver_open,
    .release = driver_release,
};

static struct timer_data{
    struct timer_list timer;
    int seconds;
}data;


irqreturn_t inter_mode_chg(int irq, void* dev_id, struct pt_regs* reg){
    char fnd[5] = {0};
    int tmp;
    if(mode == INIT_MODE)
        return IRQ_HANDLED;

    mode = 3 - mode;
    printk("Mode changed! %d -> %d\n", 3 - mode, mode);

    del_timer_sync(&data.timer);
    data.seconds = 0;

    if(mode == COUNT_MODE){
        tmp = count;
        fnd[0] = tmp / 1000;
        tmp = tmp % 1000;
        fnd[1] = tmp / 100;
        tmp = tmp % 100;
        fnd[2] = tmp / 10;
        fnd[3] = tmp % 10;

        iom_fpga_fnd_write(fnd);
        iom_fpga_dot_write(count % 10);
    }
    else if(mode == TIMER_MODE){
        iom_fpga_fnd_write(fnd);
        iom_fpga_dot_write(-1);
    }
    return IRQ_HANDLED;
}

irqreturn_t inter_timer_up(int irq, void* dev_id, struct pt_regs* reg){
    char fnd[5] = {0};
    if(mode != TIMER_MODE)
        return IRQ_HANDLED;
    del_timer_sync(&data.timer);
    data.seconds += 1;
    if(data.seconds > 60)
        data.seconds = 60;
    
    fnd[2] = data.seconds / 10;
    fnd[3] = data.seconds % 10;
    iom_fpga_fnd_write(fnd);

    iom_fpga_dot_write(DOT_CLEAR);

    return IRQ_HANDLED;
}

irqreturn_t inter_timer_start(int irq, void* dev_id, struct pt_regs* reg){
    if(mode != TIMER_MODE)
        return IRQ_HANDLED;
    del_timer_sync(&data.timer);

    data.timer.expires = get_jiffies_64() + DEFAULT_SEC * HZ;
    data.timer.function = timer_handler;
    data.timer.data = (unsigned long)&data;

    add_timer(&data.timer);
    return IRQ_HANDLED;
}

static void timer_handler(unsigned long timeout){
    struct timer_data *p_data = (struct timer_data*)timeout;
    char fnd[5] = {0};
    
    p_data->seconds--;
    fnd[2] = p_data->seconds / 10;
    fnd[3] = p_data->seconds % 10;
    iom_fpga_fnd_write(fnd);

    if(p_data->seconds < 10){
        iom_fpga_dot_write(p_data->seconds);
        // TODO: something
    }

    if(!p_data->seconds){
        // TODO: something to do when expired
        iom_fpga_dot_write(DOT_CLEAR);
        mode = INIT_MODE;
        data.seconds = 0;
        count = 0;
        // TODO: change activity?
        return;
    }

    data.timer.expires = get_jiffies_64() + DEFAULT_SEC * HZ;
    data.timer.data = (unsigned long)&data;
    data.timer.function = timer_handler;
    add_timer(&data.timer);
}

static int driver_open(struct inode *inode, struct file *file){
    int irq;
    if(dev_usage)
        return -EBUSY;

    dev_usage = 1;
    mode = 1;
    printk("Open module!\n");

    // interrupt - home
    gpio_direction_input(IMX_GPIO_NR(1, 11));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
    ret = request_irq(irq, inter_mode_chg, IRQF_TRIGGER_FALLING, "home", 0);

    // interrupt - volup
    gpio_direction_input(IMX_GPIO_NR(2, 15));
    irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
    ret = request_irq(irq, [function name], IRQF_TRIGGER_FALLING, "volup", 0);

    // interrupt - volup
    gpio_direction_input(IMX_GPIO_NR(5, 14));
    irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
    ret = request_irq(irq, [function name], IRQF_TRIGGER_FALLING, "voldown", 0);

    return 0;
}

static int driver_release(struct inode *inode, struct file *file){
    dev_usage = 0;

    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

    printk("Release module!\n");
    return 0;
}


int __init driver_init(void){
    int result;
    result = register_chrdev(DEV_MAJOR_NUM, DEV_NAME, &fops);
    if (result < 0){
        printk(KERN_WARNING"Can't get major number as %d\n", DEV_MAJOR_NUM);
        return result;
    }
    printk("init module, %s major number: %d\n", DEV_NAME, DEV_MAJOR_NUM);
    iom_mapping();

    init_timer(&(data.timer));
    return 0;
}

void __exit driver_exit(void){
    printk("dev_driver exit\n");
    init_devices();
    iom_unmapping();
    dev_usage = 0;
    del_timer_sync(&data.timer);
    unregister_chrdev(DEV_MAJOR_NUM, DEV_NAME);
}

module_init(driver_init);
module_exit(driver_exit);
MODULE_LICENSE("Dual BSD/GPL");

