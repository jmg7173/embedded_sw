#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>

#include "led.h"

void led_write(int value){
    int fd, i;
    unsigned long *fpga_addr = 0;
    unsigned char *led_addr = 0;
    if(value < 0 || data > 255){
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

int led_read(){
    int fd;
    int data = 0;

    fd = open("/dev/mem", O_RDONLY | O_SYNC);
    if(fd < 0){
        perror("/dev/mem open error");
        exit(1);
    }

    fpga_addr = (unsigned long *)mmap(
            NULL, 4096, PROT_READ,
            MAP_SHARED, fd, FPGA_BASE_ADDRESS);
    if(fpga_addr == MAP_FAILED){
        printf("mmap error!\n");
        close(fd);
        exit(1);
    }
    led_addr = (unsigned char*)((void*)fpga_addr+LED_ADDR);
    data = *led_addr;

    munmap(led_addr, 4096);
    close(fd);
    return data;

}
