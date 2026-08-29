#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001eec;

void Func_80c9138(void)
{
    char *b;
    int *ctr;
    int *xp;
    int *yp;
    int n;

    b = iwram_3001eec;
    ctr = (int *)(b + 0x7790);
    n = *ctr + 1;
    *ctr = n;
    if (n == *(int *)(b + 0x7794)) {
        xp = (int *)(b + 0x77d0);
        REG_BG2X = *xp;
        yp = (int *)(b + 0x77d4);
        REG_BG2Y = *yp;
        *xp = *xp + *(int *)(b + 0x7798);
        *yp = *yp + *(int *)(b + 0x779c);
        *ctr = 0;
    }
}
