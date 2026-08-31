/* OvlFunc_911_200a7ac  [overlays/rom_79e5c0]  -- FIRST PASS, NOT CLOSE
 *
 * Source asm: goldensun/asm/overlays/rom_79e5c0/ovl_30_c_c_a.s
 * State: 157 lines against 160, 157 differing ALIGNED. This is a decoded
 * reading, not a near-miss. It is filed so the next attempt starts from the
 * structure rather than from the assembly.
 *
 * WORTH 2 FUNCTIONS: tools/dupfuncs.py pairs it with OvlFunc_913_200a974 in
 * asm/overlays/rom_7a04ac/ovl_30_c_c_c_c_a.s.
 *
 * WHAT IT DOES, decoded and believed right: a spawner. It dispatches on a
 * three-state global, nudges two more globals toward limits, then -- every
 * eighth tick -- creates actor kind 0x11d, plays a sound every 64th, and
 * scatters it around a base point read through iwram_3001e70 using __Random.
 *
 * THE THREE GLOBALS are the local data labels .L368c, .L3690 and .L3694,
 * reached with the `extern int X __asm__(".LNNNN");` alias form.
 *
 * THE DISPATCH IS A `switch`, and the first dozen instructions confirm it: the
 * ROM's `cmp #2 / beq / cmp #2 / bhi / cmp #1 / beq / b` is gcc's balanced
 * case tree, reproduced exactly by cases 1, 2 and 3 with a default.
 *
 * TWO CONCRETE DIFFERENCES ARE IDENTIFIED and neither is yet fixed:
 *   1. The ROM compares the state UNSIGNED (`bhi`); we emit `bgt`. Typing the
 *      global `unsigned int` does NOT change it -- measured, no effect -- so
 *      the signedness comes from somewhere else, most likely the case labels'
 *      type or an intervening cast.
 *   2. The ROM hoists `mov r5, #0` to the very top, before the switch. That
 *      zero is the one stored to .L3694 in case 3, and gcc keeps it in the
 *      register that later holds the created actor.
 *
 * BE CAREFUL WITH THE COUNT. 157 of 160 is the ALIGNED count, so the body
 * really does differ; an earlier read of mine that "the dispatch matches so the
 * rest is cascade" was wrong, and only the first dozen lines are right.
 *
 * The struct layout below places f50 at 0x50 and f55 at 0x55, which the tail's
 * `ldr r1, [r5, #0x50]` and `add r3, #0x55` confirm; an earlier version had
 * them transposed and it made no difference to the count, which is itself a
 * hint that the body's problem is upstream of field offsets.
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
    int lim;
    int x;
    int y;

    switch (L3694) {
    case 1:
        if (L3690 <= 0x3a97)
            L3690 += 0x32;
        lim = 0xf0 << 14;
        goto clamp;
    case 2:
        if (L3690 <= 0x752f)
            L3690 += 0x32;
        lim = 0xc0 << 13;
    clamp:
        if (L368c > lim)
            L368c += 0xffffc000;
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
