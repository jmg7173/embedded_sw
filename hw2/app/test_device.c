#include <stdio.h>
#include <stdlib.h>

#define DEV_DRIVER "/dev/dev_driver"
#define INPUT_SYSCALL_NUM 376

struct dev_data{
    int gap;
    int times;
    int option;
};

int main(int argc, char **argv){
    int gap, times;
    int option;
    struct dev_data syscall_input;
    int write_val;
    unsigned char retval;

    if(argc != 4){
        printf("please input the parameter\n");
        printf("./app 1 14 0700\n");
        return -1;
    }
    gap = atoi(argv[1]);
    if(gap < 0 || gap > 100){
        printf("Invalid time gap range.\n");
        exit(1);
    }

    times = atoi(argv[2]);
    if(times < 0 || times > 100){
        printf("Invalid n-times range.\n");
        exit(1);
    }

    option = atoi(argv[3]);
    if(option <= 0 || option > 8000){
        printf("Invalid option range.\n");
        exit(1);
    }
    syscall_input.gap = gap;
    syscall_input.times = times;
    syscall_input.option = option;

    write_val = syscall(INPUT_SYSCALL_NUM, &syscall_input);
    dev = open(DEV_DEVICE, O_RDWR);
    retval = write(dev, (char*)&write_val, sizeof(int));
    close(dev);
}
