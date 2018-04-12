#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "driver_helper.h"

void fnd_write(char* value){
    int dev, retval;
    int i;
    char data[4];

    memcpy(data, value, 4);
    for(i = 0; i < 4; ++i){
        data[i] -= 0x30;
    }
    dev = open(FND_DEVICE, O_WRONLY);
    if (dev < 0){
        printf("Device open error: %s\n", FPGA_FND_DEVICE);
        exit(1);
    }
    retval = write(dev, &data, 4);
    if(retval < 0){
        printf("Write Error: %s\n", FPGA_FND_DEVICE);
    }
    close(dev);
}

void lcd_write(char* value){
    int dev;
    unsigned char string[32];

    dev = open(FPGA_TEXT_LCD_DEVICE, WRONLY);
    if(dev < 0){
        printf("Device open error: %s\n",
                FPGA_TEXT_LCD_DEVICE);
        exit(1);
    }
    str_size = strlen(value);
    if(str_size > MAX_LCD_LEN){
        printf("Too long text at lcd.\n");
        exit(1);
    }
    write(dev, value, str_size);
    close(dev);
}

void dot_matrix_char(char* value){
}

