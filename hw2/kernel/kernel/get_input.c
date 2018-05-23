#include <linux/kernel.h>
#include <linux/uaccess.h>

struct dev_data{
    int gap;
    int times;
    int option;
};

asmlinkage int sys_get_input(struct dev_data *data){
}
