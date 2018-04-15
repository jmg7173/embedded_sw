#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>

#include "device_helper.h"

void led_write(char* value){
    int fd, i, data;
    unsigned long *fpga_addr = 0;
    unsigned char *led_addr = 0;

    data = atoi(value);
    if(data < 0 || data > 255){
        fprintf(stderr, "Invalid range! data: %d", data);
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
        perror("mmap error!");
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
    dev = open(FPGA_FND_DEVICE, O_WRONLY);
    if (dev < 0){
        fprintf(stderr, "Device open error: %s\n",
                FPGA_FND_DEVICE);
        exit(1);
    }
    retval = write(dev, &data, 4);
    if(retval < 0){
        fprintf(stderr, "Write Error: %s\n", FPGA_FND_DEVICE);
    }
    close(dev);
}

void lcd_write(char* value){
    int dev, str_size;
    unsigned char string[MAX_LCD_LEN+1] = {0};

    dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
    if(dev < 0){
        fprintf(stderr, "Device open error: %s\n",
                FPGA_TEXT_LCD_DEVICE);
        exit(1);
    }

    str_size = strlen(value);
    strcpy(string, value);
    // Set ' ' remain string
    memset(string+str_size, ' ', MAX_LCD_LEN-str_size);
    if(str_size > MAX_LCD_LEN){
        fprintf(stderr, "Too long text at lcd. size: %d\n",
                str_size);
        exit(1);
    }
    write(dev, string, MAX_LCD_LEN);
    close(dev);
}

void dot_matrix_char(char* value){
    // Implement only 1 or A
    int dev, idx;
    int str_size, len;

    dev = open(FPGA_DOT_DEVICE, O_WRONLY);
    if(dev < 0){
        fprintf(stderr, "Device open error: %s\n",
                FPGA_DOT_DEVICE);
        exit(1);
    }

    len = strlen(value);
    if(len > 1){
        // Use when initialize dot matrix
        if(!strcmp(value, "blank")){
            str_size = sizeof(fpga_blank);
            write(dev, fpga_blank, str_size);
        }
    }
    else{
        // In case of alphabet, implemented only 'A'
        if(value[0] == 'A'){
            idx = value[0] - 'A';
            str_size = sizeof(fpga_char[idx]);
            write(dev, fpga_char[idx], str_size);
        }

        else if(value[0] >= '0' && value[0] <= '9'){
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
        perror("Invalid dot matrix draw string!");
        exit(1);
    }
    // Cutting string to bit
    for(i = 0; i < 10; i++){
        for(j = 0; j < 7; j++){
            if(value[i*7+j] == '1'){
                picture[i] |= (1 << (6-j));
            }
        }
    }

    dev = open(FPGA_DOT_DEVICE, O_WRONLY);
    if(dev < 0){
        fprintf(stderr, "Device open error: %s\n",
                FPGA_DOT_DEVICE);
        exit(1);
    }
    write(dev, picture, sizeof(picture));
    close(dev);
}

void init_device(){
    printf("Init led...");
    led_write("0");
    printf("OK!\n");
    printf("Init fnd...");
    fnd_write("0000");
    printf("OK!\n");
    printf("Init lcd...");
    lcd_write(empty_lcd);
    printf("OK!\n");
    printf("Init dot matrix...");
    dot_matrix_char("blank");
    printf("OK!\n");
}

