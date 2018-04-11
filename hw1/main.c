#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include "in_proc.h"

void handler(int dummy){
}

int main(){
    key_t key_in, key_out;
    int shmid_in, shmid_out;
    pid_t pid_in, pid_out, pid_parent;
    sigset_t mask;

    key_in = ftok("/etc/passwd", 1);
    key_out = ftok("/etc/passwd", 2);

    shmid_in = shmget(key_in, 1024, IPC_CREAT|0644);
    if(shmid_in == -1){
        perror("shmget btw in and main failed.\n");
        exit(1);
    }

    shmid_out = shmget(key_out, 1024, IPC_CREAT|0644);
    if(shmid_out == -1){
        perror("shmget btw main and out failed.\n");
        exit(1);
    }

    pid_in = fork();
    if(pid_in == -1){
        perror("fork error on making input process.\n");
        exit(1);
    }
    if(pid_in){
        pid_out = fork();
        if(pid_out == -1){
            perror("fork error on making output process.\n");
        }
        if(!pid_out){
            // Output process
            void* shmaddr = shmat(shmid_out, NULL, 0);
            char buf[1024];
            pid_parent = getppid();
            sigfillset(&mask);
            sigdelset(&mask, SIGUSR2);
            sigset(SIGUSR2, handler);
            sigsuspend(&mask);

            strcpy(buf, shmaddr);
            printf("[Output] From main, %s received.\n", buf);
            
            char* switch_name = strtok(buf, " ");
            int switch_num = atoi(strtok(NULL, " "));
            char* operate = strtok(NULL, " ");
            printf("%s %d %s. So LED %d turn on.\n",
                    switch_name, switch_num,
                    operate, switch_num);
            
            shmdt(shmaddr);
            printf("[Output] finished.\n");
        }
    }
    else{
        // Input process
        input_main(getppid(), shmid_in);
    }

    if(pid_in && pid_out){
        // Main process
        void* shmaddr_in = shmat(shmid_in, NULL, 0);
        void* shmaddr_out = shmat(shmid_out, NULL, 0);
        char buf[1024];
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigset(SIGUSR1, handler);
        sigsuspend(&mask);

        strcpy(buf, shmaddr_in);
        printf("[Main] From input, %s received.\n", buf);
        strcpy(shmaddr_out, buf);

        sleep(1);
        kill(pid_out, SIGUSR2);

        printf("[Main] send to output process finished.\n");
        sleep(2);
        shmdt(shmaddr_in);
        shmdt(shmaddr_out);
        shmctl(shmid_in, IPC_RMID, NULL);
        shmctl(shmid_out, IPC_RMID, NULL);
        printf("[Main] Program end.\n");
    }

    return 0;
}
