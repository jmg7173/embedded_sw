#include <linux/module.h>
#include <asm/io.h>

#include "device_addr.h"
#include "fpga_dot_font.h"
#include "io_driver.h"

static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_text_lcd_addr;

void iom_mapping(void){
    iom_fpga_dot_addr = ioremap(FPGA_DOT_ADDRESS, 0x10);
    iom_fpga_fnd_addr = ioremap(FPGA_FND_ADDRESS, 0x4);
    iom_fpga_led_addr = ioremap(FPGA_LED_ADDRESS, 0x1);
    iom_fpga_text_lcd_addr = ioremap(FPGA_TEXT_LCD_ADDRESS, 0x32);
    printk("iomap successful!\n");
}

void iom_unmapping(void){
    iounmap(iom_fpga_dot_addr);
    iounmap(iom_fpga_fnd_addr);
    iounmap(iom_fpga_led_addr);
    iounmap(iom_fpga_text_lcd_addr);
    printk("iounmap successful!\n");
}

void iom_fpga_dot_write(int num){
    int length = 10;
    int i;
    if(num >= 0 && num < 10){
        for(i = 0; i<length; i++){
            outw(fpga_number[num][i], (unsigned int)(iom_fpga_dot_addr+i*2));
        }
    }
    else if(num == -1){ // for clear
        for(i = 0; i<length; i++){
            outw(fpga_set_blank[i], (unsigned int)(iom_fpga_dot_addr+i*2));
        }
    }
}

void iom_fpga_fnd_write(const char *nums){
    unsigned short int val;
    val = nums[0] << 12 | nums[1] << 8 |nums[2] << 4 |nums[3];
    outw(val, (unsigned int)iom_fpga_fnd_addr);
}

void iom_fpga_led_write(unsigned short int num){
    outw(num, (unsigned int)iom_fpga_led_addr);
}

void iom_fpga_text_lcd_write(const char* gdata){
    int i, length = 32;
    unsigned short int _s_value;

    for(i = 0; i<length; i++){
        _s_value = (gdata[i] & 0xFF) << 8 | (gdata[i+1] & 0xFF);
        outw(_s_value, (unsigned int)(iom_fpga_text_lcd_addr+i));
        i++;    
    }

    printk("lcd write: |%s|\n",gdata);
}

static char empty_lcd[33] = "                                ";
void iom_fpga_text_lcd_init(void){
    iom_fpga_text_lcd_write(empty_lcd);
}
