/* Func_801776c  --  0x0801776c, split out of asm/rom_15000/rom_15e8c_c_c_a.s;
 * Func_8017658, still parked, stays in _a.s.
 *
 * Parked at 6 of 137 as "sched2 alone". It was ALLOCATION first: the zero
 * stored to x and y must be a GLOBAL-alloc allocno, not a local-alloc
 * quantity, so global.c's find_reg pass 0 (r0/r1 preferred by the parameters,
 * r2 taken by the constant 1, r3/r4 by the long long zero) lands it in r5 and
 * leaves r2 for the 1, as the ROM has it. Pinning it to r5 went 6 -> 3, which
 * is what identified the class. The plain-C route is to REUSE `box` for the
 * zero: a variable assigned twice is never a local-alloc quantity, and r5 is
 * box's register anyway. A fresh named local stays local. Moving
 * `m = flags & 1;` below the stores then fixed the last 3 (sched2).
 */
extern unsigned char *iwram_3001e8c;
extern unsigned char gState[];
extern void TextBox(int id, int *x, int *y, int *w, int *h);
extern void _Func_8094154(int a, int *q);
extern void *Func_8017658(int id, int x, int y, int m);
extern int Func_8017364(void);
extern int Func_8017394(void *box);
extern void CloseUIBox(void *box, int m);
extern void WaitFrames(int n);

void Func_801776c(int id, unsigned int flags)
{
    unsigned char *p;
    long long s;
    int h;
    int w;
    int y;
    int x;
    int m;
    int t;
    int z;
    unsigned char *gsb;
    void *box;

    p = iwram_3001e8c;
    box = 0;
    x = (int)box;
    s = 0;
    y = (int)box;
    m = flags & 1;
    if (flags & 2)
        *(char *)(p + 0x12f9) = 1;
    TextBox(id, &x, &y, &w, &h);
    x = (0x1e - w) >> 1;
    y = (0xc - h) >> 1;
    if (flags & 8) {
        y = y + 4;
    } else if (flags & 0x40) {
        y = y + 0xc;
    } else {
        gsb = gState;
        _Func_8094154(*(int *)(gsb + 0x1f4), (int *)&s);
        t = ((int *)&s)[1] >> 3;
        if (t > 9)
            y = t - 5;
        else
            y = t + 4;
    }
    box = Func_8017658(id, x, y, m);
    if (box != 0) {
        while (Func_8017364() == 0)
            WaitFrames(1);
        if (flags & 0x20)
            *(char *)(iwram_3001e8c + 0xea6) = 1;
        if (!(flags & 4)) {
            CloseUIBox(box, m);
            while (Func_8017394(box) == 0)
                WaitFrames(1);
        }
    }
    z = 0;
    *(char *)(p + 0x12f9) = z;
    *(short *)(p + 0x12f4) = z;
    *(short *)(p + 0x12f6) = z;
    WaitFrames(3);
}
