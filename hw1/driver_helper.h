#ifndef _DRIVER_HELPER_H_
#define _DRIVER_HELPER_H_

#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"

#define MAX_LCD_LEN 32

#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16


unsigned char fpga_num[2][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3f,0x3f}  // 1
}

unsigned char fpga_char[1][10] = {
    {0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63}  // A
}

void led_write(char* value);
void fnd_write(char* value);
void lcd_write(char* value);
void dot_matrix_char(char* value);
void dot_matrix_draw(char* value);
#endif

