/* OvlFunc_common2_380  --  asm/overlays/common/common2_c_c_c_c_a.s
 *
 * SCREENS OK ONLY WITH -fcall-saved-r4.  See the report: this TU wants the
 * global -fcall-used-r4 turned back off, and it needs a Makefile rule the
 * coordinator has to add.  With the tree's current flags it is 12 differing
 * of 52, all of them r4-vs-r5 renames.
 *
 * Note this file already inherits the COMMON2_CFLAGS wildcard rule
 * (asm/overlays/common/common2_c%.o), which drops -mthumb-interwork.  That
 * group IS right here -- the ROM ends in `pop {r4, pc}`, not `bx`.
 *
 * THREE THINGS DECIDED IT.
 *
 * 1. THE FRAME IS ONE STRUCT.  Two separate locals (`int v[2]; struct W w;`)
 *    give `str r0, [sp, #0]` for the first store where the ROM has
 *    `str r0, [r3, #0]` -- gcc proves the pointer is sp and uses the SP-form
 *    encoding for offset 0 only.  Declaring one `struct F { int a; int b;
 *    struct W w; }` and taking `&f.a` / `&f.w` keeps both stores on r3.
 *
 * 2. BOTH POINTERS MUST BE NAMED.  Without them the ROM's opening
 *    `mov r3, sp / add r4, sp, #8` does not appear at all.
 *
 * 3. THE 0x7fffffff BLOCK COMES BEFORE THE 41c BLOCK.  `if (f8 > 0x1e)
 *    return BIG;` makes BIG the fall-through and puts the call block after it,
 *    which is the ROM's .L3/.L4 order; the natural `if (f8 <= 0x1e) { call }`
 *    inverts them.  The goto into the `if` body is what lets the two BIG exits
 *    share one block without writing the expression twice.
 *
 * The `neg / orr / lsr #31` is the ROM's own, produced by
 * `0x7fffffff + (w->f4 != 0)`.
 */
struct W { int f0; int f4; int f8; int fc; int f10; };
struct F { int a; int b; struct W w; };

extern void OvlFunc_common2_618(int *v, struct W *w);
extern unsigned int OvlFunc_common2_40c(struct W *w);
extern unsigned int OvlFunc_common2_3ec(struct W *w);
extern unsigned int OvlFunc_common2_3fc(struct W *w);
extern int OvlFunc_common2_41c(int a, int b, int c);

int OvlFunc_common2_380(int a, int b)
{
    struct F f;
    int *vp;
    struct W *wp;
    int r;

    vp = &f.a;
    wp = &f.w;
    vp[0] = a;
    vp[1] = b;
    OvlFunc_common2_618(vp, wp);
    if (OvlFunc_common2_40c(wp) != 0 || OvlFunc_common2_3ec(wp) != 0)
        return 0;
    if (OvlFunc_common2_3fc(wp) != 0)
        goto big;
    if (wp->f8 < 0)
        return 0;
    if (wp->f8 > 0x1e) {
big:
        return 0x7fffffff + (wp->f4 != 0);
    }
    r = OvlFunc_common2_41c(wp->fc, wp->f10, 0x3c - wp->f8);
    if (wp->f4 != 0)
        r = -r;
    return r;
}
