/* OvlFunc_911_200a7ac  [overlays/rom_79e5c0]
 *
 * Source asm: goldensun/asm/overlays/rom_79e5c0/ovl_30_c_c_a.s
 *
 * State: 159 lines against 160, 25 differing ALIGNED, first difference at
 * instruction 26. Carried across three rounds: 157 -> 76 -> 36 -> 35 -> 25.
 *
 * WORTH 2 FUNCTIONS: dupfuncs.py pairs it with OvlFunc_913_200a974 in
 * asm/overlays/rom_7a04ac/ovl_30_c_c_c_c_a.s.
 *
 * READINGS THAT GOT IT HERE, all reusable:
 *
 *   1. THE SECOND GLOBAL IS READ INSIDE EACH CASE ARM, not at the join. The
 *      ROM duplicates `ldr r2, =.L368c / ldr r3, [r2]` in both arms and shares
 *      only the compare-and-clamp. 157 -> 76. This is the
 *      put-the-work-in-every-arm rule applied to a LOAD rather than a call.
 *   2. THE ACTOR POINTER IS INITIALISED TO ZERO at the top; the ROM's
 *      `mov r5, #0` before the switch is that initialiser, and the same
 *      register later holds the created actor. 76 -> 36.
 *   3. THE DISPATCH IS UNSIGNED, and only a cast AT THE SWITCH gives the ROM's
 *      `bhi`. Typing the global `unsigned int` does nothing. 36 -> 35.
 *   4. IN EACH ARM THE LIMIT IS COMPUTED BEFORE THE GLOBAL IS READ. That one
 *      statement swap fixes the r1/r2 assignment throughout both arms and the
 *      shared clamp. 35 -> 25. Birth order again, and the third time it has
 *      decided a two-register split in this corpus.
 *
 * A CORRECTNESS FIX, not just a diff: the mask is `-0xd` (0xfffffff3), NOT
 * `~0xd` (0xfffffff2). The ROM builds it `mov r3, #0xd / neg r3, r3`. The
 * earlier version of this park had `~0xd` and was simply wrong about the
 * value; it happened not to change the diff count, which is a good reminder
 * that the count does not validate semantics.
 *
 * WHAT IS LEFT: 25 lines in four small clusters, every one an ordering swap of
 * two adjacent instructions, plus ONE MISSING INSTRUCTION -- we are 159 against
 * 160. The missing one is at the mask: the ROM spends `mov #0xd / neg` to build
 * -13 in int width, we emit the byte literal `mov r3, #0xf3`.
 *
 *   MEASURED AND NEGATIVE: forcing that AND into int width with a temporary
 *   (`t = s->f9; s->f9 = (t & -0xd) | 4;`) does add the instruction but costs
 *   two others elsewhere -- 161 lines, 41 differing. The int temp is not the
 *   route; something has to keep the FIELD's read in SImode without
 *   introducing a new pseudo.
 *
 * The other three clusters are pairs of adjacent stores/loads exchanged
 * (`str [r5,#0xc]` vs `ldr [r5,#0x50]`; the f8/f10 stores against a byte
 * store; the byte store against the f9 read). All are schedule, not shape.
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
        lim = 0xf0 << 14;
        v = L368c;
        goto clamp;
    case 2:
        if (L3690 <= 0x752f)
            L3690 += 0x32;
        lim = 0xc0 << 13;
        v = L368c;
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
    s->f9 = (s->f9 & -0xd) | 4;
    __Actor_SetAnim(a, 1);
    __Actor_SetScript(a, gScript_911__0200b610);
}
