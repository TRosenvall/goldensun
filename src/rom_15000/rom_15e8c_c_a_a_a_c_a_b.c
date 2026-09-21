/* Cluster Func_8016f2c..Func_8016f2c extracted from goldensun/asm/rom_15000/rom_15e8c_c_a_a_a_c_a.s.
 *
 * Total .text for this TU = 216 bytes (= 0xd8). Never attempted before batch 276.
 * No pins, no flags.
 *
 * Steps all eight UI windows one frame: opening windows count `f18` up to `f1a` and
 * closing ones count it back down, and the fully-closed case zeroes the record.
 *
 * TWO LEVERS, AND THE SECOND IS A NEW READING OF THE `goto` LOOP.
 *
 * 1. A `goto` LOOP SUPPRESSES LICM, AND HERE THAT IS WHAT THE ROM WANTS.
 *    `for (i = 0; i < 8; i++)` over `w[i]` is 103 lines and 100 differing: with a
 *    real loop note, check_dbra_loop reverses the counter to `mov r3, #7` AND LICM
 *    hoists `base + 0xea3` out, where the ROM recomputes it inside. The `goto`
 *    form with an explicit `w++` restores both -- 104 lines, 33 differing.
 *
 *    Read against the file-mate Func_80ab21c (parked this round) that is the SAME
 *    lever with the OPPOSITE sign: there the ROM HOISTS its invariant address, so
 *    the pointer has to be named in source order to get the pool load into the
 *    preheader first. So the question to ask of a ROM invariant address is not
 *    "which loop form" but WHETHER IT IS INSIDE OR OUTSIDE, and then pick.
 *
 * 2. A NAMED `int one` AT EXACTLY ONE OF THE TWO DIRTY-BYTE STORES. Both sites
 *    store 1 to `base[0xea3]`; naming the value at BOTH is 32 differing, naming it
 *    at NEITHER is 33, and naming it at only the `f18 != f1a` site is exact. The
 *    residue it closes is a CROSS-JUMP -- the two tails were being merged -- and
 *    that is the general point:
 *
 *    A CROSS-JUMP RESIDUE IS DOWNSTREAM OF REGISTER ALLOCATION. gcc-2.96
 *    cross-jumps only in the post-reload pass, so two tails merge exactly when
 *    their ALLOCATION is identical. Do not look for a control-flow spelling; make
 *    the two blocks allocate differently and the cross-jump goes away on its own.
 *
 * Also measured: a typed `struct Gfx` with a `win[8]` member array is 159 lines --
 * `g->win[i]` was not reduced to one giv and r9-r11 were pushed. That is a second
 * negative for the struct lever, and it has the same shape as Func_80a6794's: the
 * lever is for strength_reduce and register class, not for every typing question.
 */
typedef unsigned char u8;
typedef unsigned short u16;

struct Win {
    int f0;
    int f4;
    u16 f8;
    u16 fa;
    u16 fc;
    u16 fe;
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    short f18;
    short f1a;
    short f1c;
    short f1e;
    short f20;
    short f22;
};

extern u8 *iwram_3001e8c;
extern void Func_8017004(struct Win *p, int a);
extern void Func_8016230(struct Win *p);
extern void ClearUIRegion(int x, int y, int w, int h);

void Func_8016f2c(void)
{
    u8 *base;
    struct Win *w;
    int i;
    int one;

    base = iwram_3001e8c;
    w = (struct Win *)(base + 0xa0 * 8);
    i = 0;
top:
    if (w->f16 != 0) {
        if (w->f18 != 0) {
            Func_8017004(w, 0);
            w->f18 = w->f18 - 1;
        } else if (w->f1a != 0) {
            Func_8016230(w);
        }
    } else if (w->f1a != 0) {
        if (w->f18 != w->f1a) {
            ClearUIRegion(w->f1c, w->f1e, w->f20, w->f22);
            Func_8017004(w, 1);
            w->f18 = w->f18 + 1;
            one = 1;
            base[0xea3] = one;
        } else {
            ClearUIRegion(w->f1c, w->f1e, w->f20, w->f22);
            w->f0 = 0;
            w->f4 = 0;
            w->f8 = 0;
            w->fa = 0;
            w->fc = 0;
            w->fe = 0;
            w->f10 = 0;
            w->f12 = 0;
            w->f14 = 0;
            w->f16 = 0;
            w->f18 = 0;
            w->f1a = 0;
            w->f1c = 0;
            w->f1e = 0;
            w->f20 = 0;
            w->f22 = 0;
            base[0xea3] = 1;
        }
    }
    i++;
    w++;
    if (i != 8)
        goto top;
}
