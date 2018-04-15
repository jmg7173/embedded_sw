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
#include "mod_proc.h"
#include "out_proc.h"

#define MOD_NUM 5

unsigned char signal_send = 0;

void handler(int sig){
    signal_send = 1;
}

int main(){
    key_t key_in, key_out;
    int shmid_in, shmid_out;
    pid_t pid_in, pid_out, pid_parent;
    sigset_t mask;

    key_in = ftok("/etc/passwd", 1);
    key_out = ftok("/etc/passwd", 2);

    // Make shared memory for input process - main process
    shmid_in = shmget(key_in, 1024, IPC_CREAT|0644);
    if(shmid_in == -1){
        perror("shmget btw in and main failed.\n");
        exit(1);
    }

    // Make shared memory for Main process - Output process
    shmid_out = shmget(key_out, 1024, IPC_CREAT|0644);
    if(shmid_out == -1){
        perror("shmget btw main and out failed.\n");
        exit(1);
    }

    // Make child - Input process
    pid_in = fork();
    if(pid_in == -1){
        perror("fork error on making input process.\n");
        exit(1);
    }
    if(pid_in){ // If it is parent,
        // Make child - Output process
        pid_out = fork();
        if(pid_out == -1){
            perror("fork error on making output process.\n");
        }
        if(!pid_out){
            // Output process
            output_main(getppid(), shmid_out);
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
        char proc_result[1024];
        char key[1024];
        int value;
        char mod = 0;
        char chg = 0;
        char time_cnt = 0;
        char flag = 0;

        // Init mod - clock
        flag = 0;
        flag = processing(
                mod, NULL, proc_result, 0, 1);
        if(flag){
            strcpy(shmaddr_out, proc_result);
            kill(pid_out, SIGUSR2);
        }

        while(1){
            // Set handler flag up when SIGUSR1 send from input proc
            signal(SIGUSR1, handler);

            // Get data from shared memory linked with input
            if(signal_send){
                signal_send = 0;
                strcpy(buf, shmaddr_in);
                sscanf(buf, "%s %d", key, &value);

                // Check if sended data is key or push
                if(!strcmp(key, "end"))
                    break;
                else if(!strcmp(key, "key")){
                    if(value == vol_up){
                        mod = (mod+1)%MOD_NUM;
                        chg = 1;
                    }
                    else if(value == vol_dn){
                        mod = (mod+MOD_NUM-1)%MOD_NUM;
                        chg = 1;
                    }
                    time_cnt = 0;
                    flag = 0;
                    flag = processing(
                            mod, buf, proc_result, 0, chg);
                    if(flag){
                        strcpy(shmaddr_out, proc_result);
                        kill(pid_out, SIGUSR2);
                    }
                    chg = 0;
                }

                // Processing when push btn input come
                else if(!strcmp(key, "push")){
                    flag = 0;
                    flag = processing(
                            mod, buf, proc_result, 0, chg);
                    if(flag){
                        strcpy(shmaddr_out, proc_result);
                        kill(pid_out, SIGUSR2);
                    }
                }
            }

            // sleep 50ms
            usleep(50000); // 1s = 1000*1000 = 50000*20
            time_cnt++;
            // If 1sec passed, do something
            if(time_cnt == 20){
                time_cnt = 0;
                flag = 0;
                flag = processing(
                        mod, NULL, proc_result, 1, chg);
                if(flag){
                    strcpy(shmaddr_out, proc_result);
                    kill(pid_out, SIGUSR2);
                }
            }
        }

        // send process end message
        strcpy(shmaddr_out, "end");
        kill(pid_out, SIGUSR2);

        // set free resources
        shmdt(shmaddr_in);
        shmdt(shmaddr_out);
        shmctl(shmid_in, IPC_RMID, NULL);
        shmctl(shmid_out, IPC_RMID, NULL);
    }

    return 0;
}
