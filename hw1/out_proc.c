#include <stdio.h>
#include <string.h>

#include "out_proc.h"

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
        }
    }
    shmdt(shmaddr);
}
