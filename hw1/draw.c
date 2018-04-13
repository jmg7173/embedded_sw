#include <stdio.h>
#include <string.h>

#include "draw.h"
#include "push_helper.h"

char mod_draw(char* buf, char* job, char is_time, char chg){
    static int cursor = -1;
    static char map[MAP_SIZE+1] = {0};
    static char flicking = 1;
    static int cnt = 0;
    static char data_at_cursor = '0';
    int i;

    // init
    if(cursor == -1 || chg){
        if(cursor == -1){
            memset(map, '0', MAP_SIZE);
            cursor = 0;
        }
        sprintf(job,"2 init 0 draw %s", map);
        return 1;
    }

    if(is_time && flicking){
        map[cursor] ^= 0x01;
    }

    else if(!is_time){
        int btn_a, btn_b;
        char is_multi;
        get_push_id(buf, &btn_a, &btn_b, &is_multi);
        if(is_multi)
            return 0;

        switch(btn_a){
            case 1: // Reset
                memset(map, '0', MAP_SIZE);
                data_at_cursor = '0';
                cursor = 0;
                flicking = 1;
                cnt = -1;
                break;

            case 2: // Up
                if(cursor > 6){
                    map[cursor] = data_at_cursor;
                    cursor -= 7;
                    data_at_cursor = map[cursor];
                }
                break;

            case 4: // Left
                if((cursor % 7) > 0){
                    map[cursor] = data_at_cursor;
                    cursor -= 1;
                    data_at_cursor = map[cursor];
                }
                break;

            case 6: // Right
                if((cursor % 7) < 6){
                    map[cursor] = data_at_cursor;
                    cursor += 1;
                    data_at_cursor = map[cursor];
                }
                break;

            case 8: // Down
                if(cursor < 63){
                    map[cursor] = data_at_cursor;
                    cursor += 7;
                    data_at_cursor = map[cursor];
                }
                break;

            case 5:
                data_at_cursor ^= 0x01;
                break;

            case 7: // Clear
                memset(map, '0', MAP_SIZE);
                data_at_cursor = '0';
                break;

            case 9: // Upside down
                for(i = 0; i<MAP_SIZE; ++i)
                    map[i] ^= 0x01;
                data_at_cursor ^= 0x01;
                break;

            case 3:
                flicking = 1 - flicking;
                if(!flicking)
                    map[cursor] = data_at_cursor;
                break;
        }
        cnt = (cnt + 1) % 10000;

    }
    if(!flicking)
        map[cursor] = data_at_cursor;
    sprintf(job, "2 fnd %04d draw %s", cnt, map);
    return 1;
}
