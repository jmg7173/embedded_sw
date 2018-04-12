#include <stdio.h>

void get_push_id(char* buf, int* a, int* b, char* is_multi){
    *b = -1;
    sscanf(buf,"%s %d %d", a, b);
    if(*b != -1)
        *is_multi = 1;
}
