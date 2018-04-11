#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <fcntl.h>
#include <dirent.h>
#include <terminos.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <linut/input.h>

void input_main(int parent, int shmid){
    // Read key home/back/prog/vol+/vol- and push switch
    void* shmaddr = shmat(shmid, NULL, 0);
    
    struct input_event ev[BUFF_SIZE];
    int fd_key, fd_push;
    int rd, size = sizeof (struct input_event);
    int i;

    int key_code = 0, value;
    unsigned char push_sw_buff[MAX_BUTTON];
    unsigned char prev_push[MAX_BUTTON];
    int push_bf_size = sizeof(push_sw_buff);
    int status = false;

    if((fd_key=open(DEVICE_KEY, O_RDONLY|O_NONBLOCK))==-1){
        printf("%s is not valid device.\n", DEVICE_KEY);
        exit(1);
    }
    if((fd_push=open(DEVICE_PUSH, O_RDONLY|O_NONBLOCK))==-1){
        printf("%s is not valid device.\n", DEVICE_PUSH);
        exit(1);
    }

    while(1){
        // Read key
        if((rd = read(fd_key, ev, size * BUFF_SIZE)) >= size){
            // When previous key toggled
            if(key_code == ev[0].code && 
                    value = 1 - ev[0].value){
                // If it is KEY RELEASE, do nothing.
                if(value == KEY_RELEASE)
                    printf("Key released.\n");
            }
            else{
            }
            key_code = ev[0].code;
            value = ev[0].value;
            // TODO: work when button press
        }
        else{ // Read Push button
            rd = read(fd_push, &push_sw_buff, push_bf_size);
            // Check Button Released
            for(i = 0; i<MAX_BUTTON; ++i){
                prev_push[i] = push_sw_buff[i];
                // TODO: button press recognition
            }
        }
    }
}
