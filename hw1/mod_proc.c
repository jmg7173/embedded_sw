#include "mod_proc.h"

#include "clock.h"
#include "counter.h"
#include "editor.h"
#include "draw.h"
#include "binary_game.h"

char processing(
        char mod,
        char* buf,
        char* job,
        char is_time,
        char chg
){
    switch(mod){
        case CLOCK:
            return mod_clock(buf, job, is_time, chg);
        case COUNTER:
            return mod_counter(buf, job, is_time, chg);
        case EDITOR:
            return mod_editor(buf, job, is_time, chg);
        case DRAW:
            return mod_draw(buf, job, is_time, chg);
        case GAME:
            return mod_game(buf, job, is_time, chg);
    }
}
