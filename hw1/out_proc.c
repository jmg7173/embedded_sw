#include <stdio.h>
#include <string.h>

#include <signal.h>

#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "out_proc.h"

#include "led.h"

static void write_to_device(char* key, char* value);

static void handler(int dummy){
}

void output_main(int pid_parent, int shmid){
    sigset_t mask;
    void* shmaddr = shmat(shmid, NULL, 0);

    int write_num;
    char buf[1024];
    char key[1024];
    char value[1024];

    int i, skip;

    while(1){
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR2);
        sigset(SIGUSR2, handler);
        sigsuspend(&mask);
        
        strcpy(buf, shmaddr);
        
        sscanf(buf, "%s", key);
        if(!strcmp(key, "end"))
            break;

        sscanf(buf, "%d", &write_num);
        skip = 2;
        for(i = 0; i < write_num; ++i){
            sscanf(buf + skip, "%s %s", key, value);
            skip += strlen(key) + strlen(value) + 2;
            // TODO: write to device
            write_to_device(key, value);
        }
    }
    shmdt(shmaddr);
}

static void write_to_device(char* key, char* value){
    if(!strcmp(key, "fnd")){
    }
    else if(!strcmp(key, "led")){
        led_write(atoi(value));
    }
    else if(!strcmp(key, "text")){
    }
    else if(!strcmp(key, "char")){ // dot matrix as character
    }
    else if(!strcmp(key, "draw")){
    }
}
