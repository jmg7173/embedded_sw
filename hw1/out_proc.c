#include <stdio.h>
#include <string.h>

#include <signal.h>

#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "out_proc.h"

#include "device_helper.h"

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
        // Wait for signal
        sigsuspend(&mask);
        
        strcpy(buf, shmaddr);
        sscanf(buf, "%s", key);
        if(!strcmp(key, "end")) // Back key pressed
            break;

        sscanf(buf, "%d", &write_num);
        skip = 2;
        for(i = 0; i < write_num; ++i){
            sscanf(buf + skip, "%s %s", key, value);
            if(!strcmp(key, "lcd")){
                sscanf(buf + skip, "%s %[^\n]", key, value);
            }
            skip += strlen(key) + strlen(value) + 2;
            write_to_device(key, value);
            value[0] = '\0';
        }
    }
    write_to_device("init", "");
    shmdt(shmaddr);
}

static void write_to_device(char* key, char* value){
    if(!strcmp(key, "fnd")){
        fnd_write(value);
    }
    else if(!strcmp(key, "led")){
        led_write(value);
    }
    else if(!strcmp(key, "lcd")){
        lcd_write(value);
    }
    else if(!strcmp(key, "char")){ // dot matrix as character
        dot_matrix_char(value);
    }
    else if(!strcmp(key, "draw")){
        dot_matrix_draw(value);
    }
    else if(!strcmp(key, "init")){
        init_device();
    }
}
