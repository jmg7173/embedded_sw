#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <dirent.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/input.h>

#include "in_proc.h"

void input_main(int pid_parent, int shmid){
    // Read key home/back/prog/vol+/vol- and push switch
    void* shmaddr = shmat(shmid, NULL, 0);
    char buffer[BUFFER] = {0};
    
    struct input_event ev[BUFF_SIZE];
    int fd_key, fd_push;
    int rd, size = sizeof (struct input_event);
    int i;

    int value;
    enum Keys key_code = 0;
    unsigned char push_sw_buff[MAX_BUTTON];
    unsigned char prev_push[MAX_BUTTON] = {0};
    int push_bf_size = sizeof(push_sw_buff);
    int is_changed = 0;
    int len = 0, flag;

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
                    value == 1 - ev[0].value){
                // If it is KEY RELEASE, do nothing.
                if(value == KEY_RELEASE)
                    printf("Key released.\n");
            }
            else{
            }
            key_code = ev[0].code;
            value = ev[0].value;
            if(value == KEY_PRESS){
                // write at shared memory and signal to main
                if(key_code == back){
                    strcpy(shmaddr, "end");
                    kill(pid_parent, SIGUSR1);
                    break;
                }
                sprintf(buffer,"key %d",key_code);
                strcpy(shmaddr, buffer);
                kill(pid_parent, SIGUSR1);
            }
        }
        else{ // Read Push button
            rd = read(fd_push, &push_sw_buff, push_bf_size);
            // Check Button status change 
            is_changed = 0;
            for(i = 0; i<MAX_BUTTON; ++i){
                flag = 0;
                if(prev_push[i] != push_sw_buff[i] &&
                        push_sw_buff[i] == KEY_PRESS){
                    is_changed = 1;
                }
                if(prev_push[i] == push_sw_buff[i] &&
                        push_sw_buff[i] == KEY_PRESS){
                    flag = 1;
                }
                prev_push[i] = push_sw_buff[i];
                if(flag)
                    push_sw_buff[i] = KEY_RELEASE;
            }
            if(is_changed){
                sprintf(buffer, "push");
                for(i = 0; i<MAX_BUTTON; ++i){
                    if(push_sw_buff[i] == KEY_PRESS){
                        len = strlen(buffer);
                        sprintf(buffer+len, " %d", i+1);
                    }
                }
                strcpy(shmaddr, buffer);
                kill(pid_parent, SIGUSR1);
            }
        }


        usleep(100000);
    }
    // back key pressed
    shmdt(shmaddr);
    close(fd_key);
    close(fd_push);
}
