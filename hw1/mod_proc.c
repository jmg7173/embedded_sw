#include "mod_proc.h"

#include "clock.h"
#include "counter.h"
#include "editor.h"
#include "draw.h"

char processing(char mod, char*buf, char* job, char is_time){
    switch(mod){
        case CLOCK:
            return mod_clock(buf, job, is_time);
        case COUNTER:
            return mod_counter(buf, job, is_time);
        case EDITOR:
            return mod_editor(buf, job, is_time);
        case DRAW:
            return mod_draw(buf, job, is_time);
        // TODO: EXTRA mod
    }
}
