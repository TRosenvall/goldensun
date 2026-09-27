/* FindMapActorEvent  --  0x0808d48c, was asm/rom_8a000/rom_8ba38_c_a.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * Case 0x600 repeats both tests and tail merging folds it into case 0x400 --
 * the same code, but it reproduced the ROM's r9/r10/r11 order where a
 * fall-through switch was 6 off (the extra use presumably wins the 0x3ffe
 * constant a register first; not traced).
 */
struct Rec {
    int flags;
    unsigned char id;
    unsigned char pad5;
    short flag;
    int arg;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned char *GetFieldActor(int id);
extern unsigned int Func_808d428(int x);
extern unsigned int Func_808d458(unsigned int a, unsigned int b);

struct Rec *FindMapActorEvent(int kind, int id)
{
    unsigned char *m = iwram_3001ebc;
    struct Rec *rec = *(struct Rec **)(m + 0x10);
    unsigned char *g = gState;
    int dir;
    int f;
    int ok;
    int lim;
    int n;

    dir = *(unsigned short *)(GetFieldActor(*(int *)(g + 0x1f4)) + 6);
    for (;;) {
        n = rec->flags;
        f = n;
        if (n == -1)
            break;
        if ((f & 0xf) != kind)
            goto next;
        if (rec->id != id)
            goto next;
        if (Func_808d458(f, rec->arg) == 0) {
            if (Func_808d428(rec->flag) == 0)
                goto next;
            f = rec->flags;
        }
        ok = 0;
        lim = 0xc;
        if (f & 0x800)
            lim = 2;
        switch (f & 0x600) {
        case 0:
            ok = 1;
            break;
        case 0x200:
            if (*(short *)(m + 0x19c) > lim)
                ok = 1;
            break;
        case 0x400:
            if ((unsigned short)((f &= 0xf000) - dir + 0x1fff) <= 0x3ffe)
                ok = 1;
            break;
        case 0x600:
            if (*(short *)(m + 0x19c) > lim
                && (unsigned short)((f &= 0xf000) - dir + 0x1fff) <= 0x3ffe)
                ok = 1;
            break;
        }
        if (ok)
            return rec;
    next:
        rec++;
    }
    return 0;
}
