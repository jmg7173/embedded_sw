#include "mod_proc.h"

#include "clock.h"
#include "counter.h"
#include "editor.h"
#include "draw.h"

void processing(char mod, char*buf, char* job, char is_time){
    switch(mod){
        case CLOCK:
            clock(buf, job, is_time);
            break;
        case COUNTER:
            counter(buf, job, is_time);
            break;
        case EDITOR:
            editor(buf, job, is_time);
            break;
        case DRAW:
            draw(buf, job, is_time);
            break;
        // TODO: EXTRA mod
    }
}
