/* OvlFunc_911_200a7ac  --  0x0200a7ac
 *   [asm/overlays/rom_79e5c0/ovl_30_c_c_a_c.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * THE FOURTH AND LAST TWIN found by tools/twins.py.  Identical opcode sequence
 * to the solved OvlFunc_913_200a974 (src/overlays/rom_7a04ac/ovl_30_c_c_c_c_a_c.c)
 * across all 147 instructions.  Unlike the other three twins, NOT ONE OPERAND
 * VALUE DIFFERS -- every difference is a NAME:
 *
 *   .L338c .L3388 .L3384          ->  .L3694 .L3690 .L368c
 *   gScript_913__0200b308         ->  gScript_911__0200b610
 *
 * THE TWO OVERLAYS ARE STRUCTURALLY PARALLEL, WHICH IS WHY THIS WORKS.  Each
 * declares its own trio of 4-byte `.global` + `.lcomm` cells and its own script
 * blob -- rom_7a04ac in ovl_30_c_c_c_c_c.s, rom_79e5c0 in ovl_30_c_c_c.s -- and
 * the two trios sit at the same relative offsets (0x200b38c/-4/-8 against
 * 0x200b694/-4/-8).  The mapping is fixed by that address ordering, not guessed
 * from the diff order, and the diff agrees with it.
 *
 * A `.L####` NAME IS AN ADDRESS, NOT AN IDENTITY.  The disassembler numbers
 * these labels by address, so the same name recurs in unrelated overlays: a
 * bare grep for `.L3694` finds a definition in rom_7ef4f4 that has nothing to
 * do with this function.  Resolve such a label INSIDE its own overlay.
 */
extern unsigned int L3694 __asm__(".L3694");
extern int L3690 __asm__(".L3690");
extern int L368c __asm__(".L368c");
extern unsigned int iwram_3001e40;
extern unsigned char **iwram_3001e70;
extern unsigned char *__CreateActor(int id, int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __Actor_SetAnim(void *a, int n);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char gScript_911__0200b610[];

void OvlFunc_911_200a7ac(void)
{
    unsigned char *n;
    unsigned char *b;
    register int lim __asm__("r1");
    register int v __asm__("r3");
    int px;
    int py;
    unsigned char *q;
    int t;
    int u;

    n = 0;
    switch (L3694) {
    case 1:
        if (L3690 <= 0x3a97)
            L3690 += 0x32;
        lim = 0xf0;
        v = L368c;
        lim <<= 14;
        goto step;
    case 2:
        if (L3690 <= 0x752f)
            L3690 += 0x32;
        lim = 0xc0;
        v = L368c;
        lim <<= 13;
    step:
        if (v > lim)
            L368c = v - 0x4000;
        break;
    case 3:
        if (L368c < (int)0xff800000) {
            L3694 = (unsigned int)n;
        } else {
            L3690 += 0x32;
            L368c -= 0x4000;
        }
        break;
    }
    if ((iwram_3001e40 & 7) != 0)
        return;
    n = __CreateActor(0x11d, 0, 0, 0);
    if (n == 0)
        return;
    b = iwram_3001e70[0];
    if ((iwram_3001e40 & 0x3f) == 0)
        __PlaySound(0xf6);
    if (L3694 != 0) {
        t = L3690 * __Random();
        px = *(int *)b + (((unsigned int)t >> 16) << 8) + L368c;
    } else {
        px = *(int *)b + (__Random() << 8) + 0xff800000;
    }
    py = *(int *)(b + 8) + (__Random() << 8) + 0xff800000;
    n[0x55] = 0;
    *(int *)(n + 0xc) = 0xa0 << 16;
    q = *(unsigned char **)(n + 0x50);
    *(int *)(n + 0x18) = 0xe666;
    *(int *)(n + 0x1c) = 0xe666;
    *(int *)(n + 8) = px;
    *(int *)(n + 0x10) = py;
    q[0x26] = 0;
    v = 0xfe;
    v &= n[0x23];
    n[0x23] = v;
    u = q[9];
    __asm__ volatile ("" : : "r" (u));
    v = -13;
    v &= u;
    v |= 4;
    q[9] = v;
    __Actor_SetAnim(n, 1);
    __Actor_SetScript(n, gScript_911__0200b610);
}
