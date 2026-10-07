#include "gba/types.h"
#include "gba/io.h"

extern unsigned char ewram_2002240[];

int Func_8005fcc(void)
{
    unsigned char *p;
    vu32 *sio;
    unsigned int s;
    unsigned int v;
    unsigned int ie;
    int r;
    int t;

    p = ewram_2002240;
    sio = (vu32 *)&REG_SIOCNT;
    s = *sio;
    if (p[1] == 0) {
        t = s & 0x88;
        if (t == 8) {
            v = (unsigned char)(s & 4);
            if (v == 0 && *(int *)(p + 0x14) == -1) {
                int one = 1;
                REG_IME = v;
                ie = ~0x80;
                ie &= REG_IE;
                ie |= 0x40;
                REG_IE = ie;
                REG_IME = one;
                ((vu8 *)sio)[1] &= ~0x40;
                REG_IF = 0xc0;
                REG_TM3CNT = 0xc963;
                p[0] = t;
            }
            p[1] = 1;
        }
        p[0xb]++;
    }
    r = (p[2] << 8) | p[3];
    if (p[0] == 8)
        r |= 0x80;
    if (p[9] != 0)
        r |= 0x80 << 5;
    if (((s << 26) >> 30) > 1)
        r |= 0x80 << 6;
    return r;
}
