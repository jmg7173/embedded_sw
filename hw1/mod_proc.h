#ifndef _MOD_PROC_H_
#define _MOD_PROC_H_
enum mod_type{
    CLOCK=0,
    COUNTER,
    EDITOR,
    DRAW
}

void processing(char mod, char* buf, char* job, char is_time);

#endif
