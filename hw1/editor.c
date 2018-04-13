#include <stdio.h>
#include <string.h>

#include "editor.h"
#include "push_helper.h"

char mod_editor(char* buf, char* job, char is_time, char chg){
    static unsigned char text[MAX_TEXT+1] = {0};
    static int input_mod = -1;
    static int text_idx = -1;
    static int alpha_idx = 0;
    static int prev = -1;
    static int cnt = 0;

    int btn_a, btn_b;
    char is_multi = 0;

    if(input_mod == -1 || chg){
        if(input_mod == -1)
            input_mod = 'A';
        sprintf(job, "4 init 0 char %c fnd %04d lcd %s",
                input_mod, cnt, text);
        return 1;
    }
    if(is_time)
        return 0;

    get_push_id(buf, &btn_a, &btn_b, &is_multi);
    if(is_multi){
        if(btn_a == 5 && btn_b == 6){
            input_mod ^= 0x70; // toggle 'A', '1'
            cnt = (cnt + 1) % 10000;
            prev = -1;
            alpha_idx = 0;
            // mod change 'A' -> '1' -> 'A', then keep alpha?
        }
        else if(btn_a == 2 && btn_b == 3){
            cnt = (cnt + 1) % 10000;
            memset(text, 0, MAX_TEXT);
            alpha_idx = 0;
            text_idx = -1;
            prev = -1;
            cnt = 0;
            // clear
        }
        else if(btn_a == 8 && btn_b == 9){
            cnt = (cnt + 1) % 10000;
            prev = -1;
            if(text_idx == MAX_TEXT - 1){
                strcpy(text, text+1);
                text_idx--;
            }
            // Add spacebar
            text[++text_idx] = ' ';
        }
        else{
            return 0;
        }
    }
    else{
        if(input_mod == 'A'){
            if(prev == btn_a){
                alpha_idx = (alpha_idx + 1) % 3;
                text[text_idx] = alphas[btn_a-1][alpha_idx];
            }
            else{
                alpha_idx = 0;
                if(text_idx == MAX_TEXT - 1){
                    strcpy(text, text+1);
                    text_idx--;
                }
                text[++text_idx] = alphas[btn_a-1][alpha_idx];
            }
            prev = btn_a;
        }
        else{ // input mod is numeric
            if(text_idx == MAX_TEXT - 1){
                strcpy(text, text+1);
                text_idx--;
            }
            text[++text_idx] = '0' + btn_a;
        }
        cnt = (cnt + 1) % 10000;
    }

    sprintf(job, "3 char %c fnd %04d lcd %s",
            input_mod, cnt, text);
    return 1;
}
