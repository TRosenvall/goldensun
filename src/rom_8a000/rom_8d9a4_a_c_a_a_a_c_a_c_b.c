/* Func_808e4b4  --  0x0808e4b4, split out of
 * asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c_a_c.s; Func_808e23c (parked) stays in _a.s.
 * Written from the landed sibling Func_808e14c. The reloaded flags go back into
 * the loop's own `flags`, and the mask test is `if ((flags & 0x7000000f) ==
 * type) goto ok; goto next;` for the ROM's bne/b pair.
 */
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern unsigned char *GetFieldActor(int a);
extern int Func_808df1c(int a, int b);
extern int Func_808bd24(void);
extern unsigned int Func_808d428(int x);
extern unsigned char *_GetMoveInfo(int id);

char *Func_808e4b4(int type, int id, int *out)
{
    unsigned char *gs;
    char *e;
    int r8;
    int r11;
    int actorHi;
    int r10;
    int flags;
    int n;
    int hi;
    short bit;
    int lo;

    e = *(char **)(*(unsigned char **)iwram_3001ebc + 0x10);
    gs = gState;
    gs += 0xfa << 1;
    actorHi = *(unsigned short *)(GetFieldActor(*(int *)gs) + 6);
    r8 = Func_808df1c(*(int *)gs, id);
    *out = r8;
    r11 = 0;
    r10 = Func_808bd24();
    if (type == 0x70000005) {
        r11 = 1;
    }
    for (;;) {
        n = *(int *)e;
        flags = n;
        if (n == -1) {
            break;
        }
        hi = *(short *)(e + 4) & 0xf000;
        bit = *(short *)(e + 4) & (short)0x800;
        lo = *(short *)(e + 4) & 0xff;
        if ((flags & 0xf) == 5 && Func_808d428(*(short *)(e + 6)) != 0) {
            if (bit == 0 || (unsigned short)((hi - actorHi) + 0x17ff) <= 0x2ffe) {
                if (_GetMoveInfo(*(unsigned char *)(e + 1))[0xc] == id) {
                    if (r11 == 0) {
                        flags = *(int *)e;
                        if ((flags & 0x7000000f) == type)
                            goto ok;
                        goto next;
                    } else {
                        flags = *(int *)e;
                    }
                ok:
                    if (flags & 0x80)
                        return e;
                    if ((flags & 0x10) != 0) {
                        if (lo == r8) {
                            return e;
                        }
                    } else {
                        if (lo == r10) {
                            return e;
                        }
                    }
                }
            }
        }
    next:
        e += 0xc;
    }
    return 0;
}
