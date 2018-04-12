#ifndef _DRIVER_HELPER_H_
#define _DRIVER_HELPER_H_

#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"

#define MAX_LCD_LEN 32

void fnd_write(char* value);
void lcd_write(char* value);
void dot_matrix_char(char* value);
#endif
