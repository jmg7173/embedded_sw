#include <linux/kernel.h>
#include <linux/uaccess.h>

struct dev_data{
    int gap;
    int times;
    int option;
};

asmlinkage int sys_get_input(struct dev_data *data){
    struct dev_data ker_data;
    int gap, times, option;
    int result = 0;
    copy_from_user(&ker_data, data, sizeof(ker_data));

    if(ker_data.option % 10){
        result |= 1 << 24; // start position
        result |= ker_data.option << 16; //start pattern
    }
    else if(ker_data.option % 100){
        result |= 2 << 24;
        result |= (ker_data.option / 10) << 16;
    }
    else if(ker_data.option % 1000){
        result |= 3 << 24;
        result |= (ker_data.option / 100) << 16;
    }
    else if(ker_data.option % 10000){
        result |= 4 << 24;
        result |= (ker_data.option / 1000) << 16;
    }

    result |= ker_data.gap << 8;
    result |= ker_data.times;

    return result;
}
