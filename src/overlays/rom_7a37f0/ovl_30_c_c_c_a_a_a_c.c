/* OvlFunc_916_2008194 -- asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a_c.s.
 *
 * Redraws the puzzle board for whichever of the two layouts *L12c8 selects, then
 * walks the -1-terminated record table setting each piece's actor animation and
 * blitting its tile.
 *
 * ONE LEVER, AND IT IS THE WHOLE FUNCTION: **NAME THE STACK ARGUMENTS.**
 *
 * Every call here is six arguments, so two go on the stack.  Left as literals
 * in the argument list gcc computes one, stores it, RE-USES THE SAME REGISTER for
 * the second and stores that -- `mov r3,#79 / str r3,[sp] / mov r3,#29 /
 * str r3,[sp,#4]`.  The ROM holds both in separate registers and stores them
 * together: `mov r3,#79 / mov r2,#29 / str r3,[sp] / str r2,[sp,#4]`.  Writing
 * the two values as block-scoped locals `e` and `f` immediately before the call
 * makes them two live pseudos and gives the ROM's form.
 *
 * 39 differing without it, EXACT with it -- and the 39 were not all at the
 * stack slots.  Handing the allocator two more pseudos re-sorted the whole
 * function: the zero-index register for the `ldrsh` (Thumb has no immediate
 * form, so the offset 0 must be materialised) moved from r2 to the ROM's r1 in
 * three places, `__Actor_SetAnim`'s two arguments swapped back at ONE of its two
 * sites, and a `mov`/`add` pair on the actor pointer re-ordered.  All of those
 * read as register-allocation and scheduling residue, which is the class the
 * corpus has the fewest levers for -- and here every one of them was downstream
 * of the argument-precompute spelling.
 *
 * So the batch-280 note that "eleven-argument calls want their stack arguments
 * NAMED" generalises: it is not about eleven arguments, it is about ANY call
 * with more than one stack slot, and the symptom is a stack slot filled from a
 * register that was just stored to the slot before it.
 *
 * Also in play, both already on file for this directory:
 *   * `OvlFunc_916_2008b3c` is declared `extern int` -- the callee's return type
 *     decides argument fill order (see ovl_30_c_c_c_a_c_a_a_a_c_a.c).
 *   * `.L12c0` and `.L12c8` are pointer VARIABLES in .bss, reached by their
 *     `__asm__` names, and the loop walks a LOCAL copy of .L12c0 while the two
 *     tail calls re-read the global -- which is why r6 is advanced and the
 *     epilogue reloads the pool word.
 *
 * VERIFIES: 472 bytes, 215 encodings and 21 relocations identical.
 */
struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x23 - 0x10];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

struct Rec {
    short f0;
    short f2;
    short f4;
    short f6;
    struct Actor *f8;
};

extern void *L12c0 __asm__(".L12c0");
extern short *L12c8 __asm__(".L12c8");

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern int OvlFunc_916_2008b3c(void *p, int n);
extern void OvlFunc_916_2008150(void);

void OvlFunc_916_2008194(void)
{
    struct Rec *r;
    struct Actor *a;

    r = (struct Rec *)L12c0;
    if (*L12c8 != 0) {
        {
            int e = 0x4f;
            int f = 0x1d;
            __Func_80105d4(0x41, 0x35, 2, 1, e, f);
        }
        {
            int e = 0xf;
            int f = 0x1c;
            __Func_80105d4(0x41, 0x28, 2, 4, e, f);
        }
    } else {
        {
            int e = 0x4f;
            int f = 0x19;
            __Func_80105d4(0x41, 0x32, 2, 5, e, f);
        }
    }
    if (*L12c8 != 0) {
        __Func_80105d4(0, 0x20, 0x20, 0x20, 0x20, 0);
        __Func_80105d4(0x20, 0x20, 0x20, 0x20, 0x40, 0);
        __Func_8010704(0, 0x20, 0x20, 0x20, 0, 0);
    } else {
        __Func_80105d4(0, 0x40, 0x20, 0x20, 0x20, 0);
        __Func_80105d4(0x20, 0x40, 0x20, 0x20, 0x40, 0);
        __Func_8010704(0, 0x40, 0x20, 0x20, 0, 0);
    }
    while (r->f0 != -1) {
        a = r->f8;
        if (*L12c8 == 1) {
            __Actor_SetAnim(a, 4);
            a->f23 = 3;
            a->f55 = 0;
            a->fc = 0xd0 << 13;
            if (r->f6 != 0) {
                int e = r->f2;
                int f = r->f4;
                __Func_80105d4(0x44, 0x28, 1, 4, e + 0x20, f);
            } else {
                int e = r->f2;
                int f = r->f4;
                __Func_80105d4(0x46, 0x28, 4, 1, e + 0x20, f);
            }
        } else {
            __Actor_SetAnim(a, 1);
            a->f23 = 1;
            a->f55 = 2;
            a->fc = 0;
        }
        r++;
    }
    {
        int e = 0xa;
        int f = 0x32;
        __Func_80105d4(0x46, 0x2a, 1, 1, e, f);
    }
    if (*L12c8 == 1) {
        __Func_8010704(0, 0x20, 0x20, 0x20, 0, 0);
        OvlFunc_916_2008b3c(L12c0, 0xfe);
    } else {
        __Func_8010704(0, 0x40, 0x20, 0x20, 0, 0);
        OvlFunc_916_2008b3c(L12c0, 0xff);
    }
    OvlFunc_916_2008150();
}
