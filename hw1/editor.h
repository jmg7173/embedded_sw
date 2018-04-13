#ifndef _EDITOR_H_
#define _EDITOR_H_
#define MAX_TEXT 32

static char alphas[9][3] = {
    {'.', 'Q', 'Z'}, {'A', 'B', 'C'}, {'D', 'E', 'F'},
    {'G', 'H', 'I'}, {'J', 'K', 'L'}, {'M', 'N', 'O'},
    {'P', 'R', 'S'}, {'T', 'U', 'V'}, {'W', 'X', 'Y'}
};

char mod_editor(char* buf, char* job, char is_time, char chg);
#endif
