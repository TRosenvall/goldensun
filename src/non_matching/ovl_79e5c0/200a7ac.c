/* OvlFunc_911_200a7ac  [overlays/rom_79e5c0]
 *
 * Source asm: goldensun/asm/overlays/rom_79e5c0/ovl_30_c_c_a.s
 *
 * State: 159 lines against 160, 35 differing ALIGNED, first difference at
 * instruction 24. Everything before that matches, and everything after the
 * case arms matches -- the residue is ONE register swap and its cascade.
 *
 * WORTH 2 FUNCTIONS: dupfuncs.py pairs it with OvlFunc_913_200a974 in
 * asm/overlays/rom_7a04ac/ovl_30_c_c_c_c_a.s.
 *
 * PROGRESS THIS ROUND: 157 -> 76 -> 36 -> 35. Three readings did it, and each
 * is reusable:
 *
 *   1. THE SECOND GLOBAL IS READ INSIDE EACH CASE ARM, not at the join. The
 *      ROM duplicates `ldr r2, =.L368c / ldr r3, [r2]` in both arms and shares
 *      only the compare-and-clamp. Joining early -- reading it once after the
 *      switch -- is 157 differing; reading it per arm is 76. This is the
 *      "put the work in every arm and let gcc cross-jump" rule applied to a
 *      LOAD rather than a call.
 *   2. THE ACTOR POINTER IS INITIALISED TO ZERO at the top. The ROM's
 *      `mov r5, #0` before the switch is that initialiser, and the same
 *      register later holds the created actor; case 3's store to .L3694 reuses
 *      it by value numbering. Adding `a = 0;` is 76 -> 36 and moves the first
 *      difference from instruction 3 to 7. Whether case 3 then writes `0` or
 *      `(int)a` makes no difference.
 *   3. THE DISPATCH IS UNSIGNED. The ROM's `bhi` needs
 *      `switch ((unsigned int)L3694)`. Typing the GLOBAL `unsigned int` does
 *      NOT do it -- only the cast at the switch does. 36 -> 35.
 *
 * WHAT IS LEFT, and it is one thing: in both case arms the ROM puts the
 * address of .L368c in r2 and the limit constant in r1; we use r1 and r2 the
 * other way round. Every one of the 35 lines is that swap or follows from it.
 *
 *     rom    ldr r2, =.L368c / mov r1, #0xf0 / ldr r3, [r2] / lsl r1, #0xe
 *     ours   ldr r1, =.L368c / mov r2, #0xf0 / ldr r3, [r1] / lsl r2, #0xe
 *
 * The instruction ORDER is already the ROM's; only the two register names are
 * exchanged.
 *
 * MEASURED AND NEGATIVE, all still 35 or 36:
 *   swapping the declaration order of the value and the limit          36
 *   naming the pointer the clamp stores through (`int *pc = &L368c;`)  35
 *   a named `int z = 0;` for case 3's store instead of a literal       76
 *   typing the global `unsigned int` rather than casting at the switch 76
 *
 * NEXT: this is the register-allocation coin flip in its two-register form,
 * and per batch 153 it is the kind that disappears under pressure -- so the
 * thing worth trying is whatever makes the arms need one more live value, not
 * another spelling of these two.
 */
struct Sub {
    unsigned char pad00[9];
    unsigned char f9;
    unsigned char pad0a[0x1c];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Sub *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern int L368c __asm__(".L368c");
extern int L3690 __asm__(".L3690");
extern int L3694 __asm__(".L3694");
extern unsigned char gScript_911__0200b610[];

extern unsigned int iwram_3001e40;
extern int **iwram_3001e70;
extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetScript(struct Actor *a, void *s);

void OvlFunc_911_200a7ac(void)
{
    struct Actor *a;
    struct Sub *s;
    int *b;
    int v;
    int lim;
    int x;
    int y;

    a = 0;
    switch ((unsigned int)L3694) {
    case 1:
        if (L3690 <= 0x3a97)
            L3690 += 0x32;
        v = L368c;
        lim = 0xf0 << 14;
        goto clamp;
    case 2:
        if (L3690 <= 0x752f)
            L3690 += 0x32;
        v = L368c;
        lim = 0xc0 << 13;
    clamp:
        if (v > lim)
            L368c = v + 0xffffc000;
        break;
    case 3:
        if (L368c < (int)0xff800000) {
            L3694 = 0;
        } else {
            L3690 += 0x32;
            L368c += 0xffffc000;
        }
        break;
    }
    if ((iwram_3001e40 & 7) != 0)
        return;
    a = __CreateActor(0x11d, 0, 0, 0);
    if (a == 0)
        return;
    b = *iwram_3001e70;
    if ((iwram_3001e40 & 0x3f) == 0)
        __PlaySound(0xf6);
    if (L3694 != 0)
        x = b[0] + ((L3690 * __Random() >> 16) << 8) + L368c;
    else
        x = b[0] + (__Random() << 8) + 0xff800000;
    y = b[2] + (__Random() << 8) + 0xff800000;
    a->f55 = 0;
    a->fc = 0xa0 << 16;
    s = a->f50;
    a->f18 = 0xe666;
    a->f1c = 0xe666;
    a->f8 = x;
    a->f10 = y;
    s->f26 = 0;
    a->f23 &= 0xfe;
    s->f9 = (s->f9 & ~0xd) | 4;
    __Actor_SetAnim(a, 1);
    __Actor_SetScript(a, gScript_911__0200b610);
}
