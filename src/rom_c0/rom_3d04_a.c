/* Func_8003d28  --  0x08003d28, was asm/rom_c0/rom_3d04_a.s (this function
 * alone), so it converts whole. Matched from scratch.
 *
 * The division goes through a pointer local to divsi3_RAM, which is what gives
 * `bl _call_via_r3`; a plain `/` emits `bl __divsi3`. The index must be
 * `unsigned int` -- `int` gives `bgt` where the ROM has `bhi`.
 */
extern int sin(int a);
extern int cos(int a);
extern int divsi3_RAM(int a, int b);
extern unsigned char iwram_3001d00;
extern unsigned char iwram_3001d40[];

int Func_8003d28(short *p)
{
    int sx, sy, ang;
    unsigned int idx;
    short *m;

    idx = iwram_3001d00;
    sx = p[0];
    sy = p[1];
    ang = (unsigned short)p[2];
    if (idx > 31)
        return 0;
    m = (short *)(iwram_3001d40 + idx * 8);
    if ((sx == sy || -sx == sy) && ang == 0) {
        int (*fp)(int, int) = divsi3_RAM;
        int q = fp(0x10000, sy);
        int a = q;
        if (-sx == sy)
            a = -q;
        ((unsigned int *)m)[0] = (unsigned short)a;
        ((unsigned int *)m)[1] = q << 16;
    } else {
        int s = sin(ang);
        int c = cos(ang);
        *m++ = c / sx;
        *m++ = s / sx;
        *m++ = -s / sy;
        *m = c / sy;
    }
    iwram_3001d00 = idx + 1;
    return idx;
}
