#ifndef _IO_DRIVER_H_
#define _IO_DRIVER_H_
void iom_fpga_dot_write(int num);
void iom_fpga_fnd_write(unsigned short int num);
void iom_fpga_led_write(unsigned short int num);
void iom_fpga_text_lcd_write(const char* gdata);
void iom_fpga_text_lcd_init(void);

#endif
