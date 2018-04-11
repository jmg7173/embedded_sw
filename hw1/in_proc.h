#ifndef _IN_PROC_H_
#define _IN_PROC_H_
#define KEY_RELEASE 0
#define KEY_PRESS 1
#define BUFF_SIZE 64
#define MAX_BUTTON 9

#define DEVICE_KEY "/dev/input/event0"
#define DEVICE_PUSH "/dev/fpga_push_switch"
enum Keys={
    vol_up=115, 
    vol_dn=114, 
    prog=116, 
    back=158, 
    home=102
};
#endif
