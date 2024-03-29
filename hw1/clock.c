#include <stdio.h>
#include <time.h>

#include "clock.h"
#include "push_helper.h"

static int get_board_time();

char mod_clock(char* buf, char* job, char is_time, char chg){
    static int timer = 0;
    static int mod_changing = 0;
    static int cur_time = -1;
    static int modified_time = -1;
    static int led = 0b10000000;
    char modified = 0;

    // Initialize when first run and mod change
    // When mod change, keep previous data
    if(cur_time == -1 || chg){
        if(cur_time == -1)
            cur_time = get_board_time();
        led = 0b10000000;
        mod_changing = 0;
        sprintf(job, "3 init 0 led %d fnd %02d%02d",
                led, cur_time/60, cur_time%60);
        return 1;
    }
    if(is_time){
        // If mod changing, toggle led
        if(mod_changing){
            if(led == 128)
                led = 0b00010000;
            led ^= 0b00110000;
            sprintf(job, "1 led %d", led);
            modified = 1;
        }
        else{ // not mod changing
            timer++;
            if(timer == 60){
                // increase minute
                cur_time++;
                cur_time %= 1440;
                sprintf(job, "1 fnd %02d%02d",
                        cur_time/60, cur_time%60);
                modified = 1;
                timer = 0;
            }
        }
    }
    else{ // When push button pressed
        int btn_a, btn_b;
        char is_multi;
        get_push_id(buf, &btn_a, &btn_b, &is_multi);
        if(is_multi){
            return 0;
        }
        switch(btn_a){ // only one button pressed
            case 1: // mod changing toggle
                mod_changing = 1-mod_changing;
                timer = 0;
                if(!mod_changing){ // If fix mode
                    cur_time = modified_time;
                    led = 0b10000000;
                }
                else{ // If change mode
                    modified_time = cur_time;
                    led = 0b00100000;
                }
                break;
            case 2: // reset
                modified_time = get_board_time();
                break;
            case 3: // increase hour
                modified_time =
                    (modified_time + 60) % 1440;
                break;
            case 4: // increase minute
                modified_time =
                    (modified_time + 1) % 1440;
                break;
            default: // Other button
                return 0;
        }
        if(mod_changing){
            sprintf(job, "2 led %d fnd %02d%02d",
                    led,
                    modified_time/60,
                    modified_time%60);
            modified = 1;
        }
        else{
            sprintf(job, "2 led %d fnd %02d%02d",
                    led, cur_time/60, cur_time%60);
            modified = 1;
        }
    }
    return modified;
}

static int get_board_time(){
        time_t T = time(NULL);
        struct tm tm = *localtime(&T);
        return tm.tm_hour * 60 + tm.tm_min;
}
