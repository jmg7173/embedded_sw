#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/mann.h>

#include "driver_helper.h"

void led_write(char* value){
    int fd, i, data;
    unsigned long *fpga_addr = 0;
    unsigned char *led_addr = 0;

    data = atoi(value);
    if(data < 0 || data > 255){
        printf("Invalid range!\n");
        exit(1);
    }
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd < 0){
        perror("/dev/mem open error");
        exit(1);
    }

    fpga_addr = (unsigned long *)mmap(
            NULL, 4096, PROT_WRITE,
            MAP_SHARED, fd, FPGA_BASE_ADDRESS);
    if (fpga_addr == MAP_FAILED){
        printf("mmap error!\n");
        close(fd);
        exit(1);
    }

    led_addr = (unsigned char*)((void*)fpga_addr + LED_ADDR);

    *led_addr = data;

    munmap(led_addr, 4096);
    close(fd);
}

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
    // Implement only 1 or A
    int dev, idx;
    int str_size, len;

    dev = open(FPGA_DOT_DEVICE, O_WRONLY);
    if(dev < 0){
        printf("Device open error: %s\n", FPGA_DOT_DEVICE);
        exit(1);
    }

    len = strlen(value);
    if(len > 1){
        if(!strcmp(value, "empty")){
            str_size = sizeof(fpga_blank);
            write(dev, fpga_blank, str_size);
        }
    }
    else{
        if(value[0] == 'A'){
            idx = value[0] - 'A';
            str_size = sizeof(fpga_char[idx]);
            write(dev, fpga_char[idx], str_size);
        }

        else if(value[0] == '1'){
            idx = value[0] - '0';
            str_size = sizeof(fpga_num[idx]);
            write(dev, fpga_num[idx], str_size);
        }
    }

    close(dev);
}

void dot_matrix_draw(char* value){
    int dev, len;
    char picture[10] = {0,};
    int i, j;

    len = strlen(value);
    if(len != 70){
        printf("Invalid dot matrix draw string!\n");
        exit(1);
    }
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 7; j++){
            if(value[i*7+j] == '1'){
                picture[i] |= (1 << (6-j));
            }
        }
    }

    dev = open(FPGA_DOT_DEVICE, O_WRONLY);
    if(dev < 0){
        printf("Device open error: %s\n", FPGA_DOT_DEVICE);
        exit(1);
    }
    write(dev, picture, sizeof(picture));
    close(dev);
}

void init_device(){
    led_write("0");
    fnd_write("0000");
    lcd_write(empty_lcd);
    dot_matrix_char("blank");
}

