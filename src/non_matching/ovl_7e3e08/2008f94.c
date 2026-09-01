/* OvlFunc_957_2008f94 (0x02008f94) -- NON-MATCHING.
 * Blocker class: gcc reaches for HIGH REGISTERS where the ROM spills to the
 * frame.
 *
 * 136 lines against the ROM's 118 -- eighteen over, and all eighteen are the
 * cost of two extra callee-saved registers:
 *
 *     rom    ldr r4, =.L3f6c ... str r4, [sp] / bl ... / ldr r4, [sp]
 *     ours   mov r7, r10 / mov r6, r8 / push {r6, r7} ... mov r8, r1 ...
 *            pop {r6, r7} / mov r8, r6 / mov r10, r7
 *
 * The ROM holds the state pointer's ADDRESS in r4 -- which `-fcall-used-r4`
 * makes caller-saved, so it spills and reloads it around each of the three
 * calls. gcc instead parks it in r8, and something else in r10, paying two
 * saves, two restores and a `mov` at every use.
 *
 * MEASURED (rom 118 lines):
 *   baseline                                          136, 135
 *   the halfword at +0 read separately in each arm
 *     that uses it, instead of once before the
 *     state chain (batch 177's live-range rule)       136, 135 (inert -- the
 *                          value is already dead across every call, so
 *                          shortening its range frees nothing)
 *
 * The inert result is worth keeping: tightening a live range only helps when
 * the range actually crosses a call. Here it does not, and the two high
 * registers come from values that genuinely do.
 *
 * WHAT IS RIGHT: the five-way state chain with the s==1 arm CROSS-JUMPING into
 * the s==2 arm's tail (`goto bump`); the unsigned range check written exactly
 * as the ROM has it, `(unsigned)((d << 16) + 0xc2ff0000) <= 0x5fe0000`; the
 * `(t << 16) / 5` through _divsi3_RAM; and the trailing flag block guarded by a
 * variable set to 1 at the top and cleared only in the s==0x63 arm.
 *
 * NEXT: nothing source-level. This is the same wall as rom_9000/800bbc0.c --
 * spill versus high register -- and neither has yielded.
 */
extern unsigned char *L3f6c __asm__(".L3f6c");
extern signed char ewram_2001002;
extern char *__MapActor_GetActor(int slot);
extern void OvlFunc_957_2008ee0(void);
extern void OvlFunc_957_2008f6c(int a);
extern void __PlaySound(int id);

void OvlFunc_957_2008f94(void)
{
    unsigned char **b;
    unsigned char *e;
    char *a;
    int s;
    int h;
    int f;
    int v;
    int t;
    int q;
    int d;

    b = &L3f6c;
    e = *b;
    s = *(short *)e;
    f = 1;
    h = *(unsigned short *)e;
    if (s == 0) {
        v = *(unsigned short *)(e + 8) + 0x10;
        *(unsigned short *)(e + 8) = v;
        if ((unsigned int)(v << 16) > 0xbff0000) {
            *(unsigned short *)e = h + 1;
            *(unsigned short *)(e + 2) = s;
        }
    } else if (s == 1) {
        if (*(short *)(e + 2) == 0x1e)
            goto bump;
    } else if (s == 2) {
        v = *(unsigned short *)(e + 8) + 0xfff8;
        *(unsigned short *)(e + 8) = v;
        if ((unsigned int)(v << 16) <= 0x2ff0000) {
        bump:
            *(unsigned short *)e = h + 1;
        }
    } else if (s == 3) {
        t = ewram_2001002;
        q = (t << 16) / 5;
        d = *(unsigned short *)(e + 6) - q;
        if ((unsigned int)((d << 16) + 0xc2ff0000) <= 0x5fe0000) {
            *(unsigned short *)(e + 6) = q + (0x80 << 7);
            *(unsigned short *)e = 0x63;
            *(unsigned short *)(e + 8) = 0;
            a = __MapActor_GetActor(t + 0xb);
            *(int *)(a + 0x6c) = (int)OvlFunc_957_2008ee0;
        }
    } else if (s == 0x63) {
        f = 0;
    }
    if (f != 0) {
        e = *b;
        *(unsigned short *)(e + 6) += *(unsigned short *)(e + 8);
        OvlFunc_957_2008f6c(*(unsigned short *)(e + 6));
        e = *b;
        v = *(unsigned short *)(e + 0xa) + *(unsigned short *)(e + 8);
        *(unsigned short *)(e + 0xa) = v;
        if ((unsigned int)(v << 16) > (unsigned int)(0xc0 << 22)) {
            *(unsigned short *)(e + 0xa) = 0;
            __PlaySound(0x87);
        }
    }
    e = *b;
    *(unsigned short *)(e + 2) += 1;
}
