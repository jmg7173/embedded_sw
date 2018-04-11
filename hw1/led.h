#ifndef _LED_H_
#define _LED_H_

#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

void led_write(int value);
int  led_read(void);

#endif
