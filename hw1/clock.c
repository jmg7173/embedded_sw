#include <time.h>

#include "clock.h"
#include "led.h"
#include "push_helper.h"

static int get_board_time();

void clock(char* buf, char* job, char is_time){
    static int timer = 0;
    static int mod_changing = 0;
    static int cur_time = -1;
    static int modfied_time = -1;
    static int led = 0b10000000;
    // Initialize once
    if(cur_time == -1){
        cur_time = get_board_time();
        sprintf(job, "2 led %d fnd %02d%02d",
                led, cur_time/60, cur_time%60);
        return;
    }
    if(is_time){
        if(mod_changing){
            if(led == 128)
                led = 0b00010000;
            led ^= 0b00110000;
            sprintf(job, "led %d", led);
        }
        else{ // not mod changing
            timer++;
            if(timer == 60){
                // increase minute
                cur_time++;
                cur_time %= 1440;
                sprintf(job, "1 fnd %02d%02d",
                        cur_time/60, cur_time%60);
            }
        }
    }
    else{ // When push button pressed
        int btn_a, btn_b;
        char is_multi;

        get_push_id(buf, &btn_a, &btn_b, &is_multi);
        if(!is_multi){
            switch(btn_a){
                case 1:
                    mod_changing = 1-mod_changing;
                    if(!mod_changing){
                        cur_time = modified_time;
                        led = 0b10000000;
                    }
                    else{
                        modified_time = cur_time;
                        led = 0b00100000;
                    }
                    break;
                case 2:
                    modified_time = get_board_time();
                    break;
                case 3:
                    modified_time =
                        (modified_time + 60) % 1440;
                    break;
                case 4:
                    modified_time =
                        (modified_time + 1) % 1440;
                    break;
            }
            if(mod_changing){
                sprintf(job, "led %d fnd %02d%02d",
                        led,
                        modified_time/60,
                        modified_time%60);
            }
            else{
                sprintf(job, "led %d fnd %02d%02d",
                        led, cur_time/60, cur_time%60);
            }
        }
    }
}

static int get_board_time(){
        time_t T = time(NULL);
        struct tm tm = *localtime(&T);
        return tm.tm_hour * 60 + tm.tm_min;
}
