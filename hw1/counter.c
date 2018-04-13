#include <stdio.h>

#include "counter.h"
#include "push_helper.h"

static void digit_increase(
        int numeric,
        int* num,
        char (*nums_char)[5],
        int digit
);

char mod_counter(
        char* buf,
        char* job,
        char is_time,
        char chg
){
    static int numeric = -1; // 0: 10s, 1: 8s, 2: 4s, 3: 2s
    static char nums_char[4][5] = {0};
    static int num = 0;
    static int led = 0b01000000;
    
    int btn_a, btn_b;
    int i;
    char is_multi;

    // Initialize when first run and mod change
    // Keep data when mod change
    if(numeric == -1 || chg){
        printf("numeric: %d, chg: %d\n", numeric, chg);
        if(numeric == -1){
            numeric = 0;
            for(i = 0; i < 4; ++i)
                sprintf(nums_char[i], "%04d", 0); 
        }
        sprintf(job, "3 init 0 led %d fnd %s",
                led, nums_char[numeric]); 
        return 1;
    }

    if(is_time){
        return 0;
    }

    get_push_id(buf, &btn_a, &btn_b, &is_multi);
    if(is_multi){
        return 0;
    }
    switch(btn_a){
        case 1: // numeric change
            numeric = (numeric + 1) % 4;
            led = (1 << (7-((numeric+1)%4)));
            break;
        case 2: // increase 3rd num
            digit_increase(numeric, &num, nums_char, 3);
            break;
        case 3: // increase 2nd num
            digit_increase(numeric, &num, nums_char, 2);
            break;
        case 4: // incrase 1st num
            digit_increase(numeric, &num, nums_char, 1);
            break;
        default:
            return 0;
    }
    sprintf(job, "2 led %d fnd %s", led, nums_char[numeric]);
    return 1;
}

static void digit_increase(
        int numeric,
        int* num,
        char (*nums_char)[5],
        int digit
        ){
    int i, j, base[4] = {10, 8, 4, 2};
    int add = 1;
    int tmp, rem;

    for(i = 1; i < digit; ++i){
        add *= base[numeric];
    }
    *num = ((*num) + add) % 1000;
    for(i = 0; i < 4; ++i){
        j = 3;
        tmp = (*num);
        while(j >= 0){
            rem = tmp % base[i];
            tmp = tmp / base[i];
            nums_char[i][j--] = '0' + rem;
        }
    }
}
