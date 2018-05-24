#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/times.h>
#include <fcntl.h>

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
    int dev;
    struct dev_data syscall_input;
    int write_val;
    unsigned char retval;
    sigset_t mask;

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
    printf("writeval: %x\n", write_val);
    dev = open(DEV_DRIVER, O_RDWR);
    printf("dev: %d\n", dev);
    retval = write(dev, (char*)&write_val, sizeof(int));
    printf("retval: %d\n", retval);

    close(dev);
    return 0;
}
