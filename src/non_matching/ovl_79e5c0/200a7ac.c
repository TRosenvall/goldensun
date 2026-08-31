/* OvlFunc_911_200a7ac  [overlays/rom_79e5c0]
 *
 * Source asm: goldensun/asm/overlays/rom_79e5c0/ovl_30_c_c_a.s
 *
 * State: 160 lines against 160, SEVEN differing aligned, first difference at
 * instruction 26. Carried across four rounds: 157 -> 76 -> 36 -> 35 -> 25 -> 7.
 *
 * WORTH 2 FUNCTIONS: dupfuncs.py pairs it with OvlFunc_913_200a974 in
 * asm/overlays/rom_7a04ac/ovl_30_c_c_c_c_a.s.
 *
 * THE FINDING THAT MATTERS, and it generalises well beyond this function:
 *
 *   AN `ldrb` LOAD DOES NOT PROVE THE FIELD IS UNSIGNED. This ROM reads the
 *   field at +9 with `ldrb` -- an unsigned byte load -- and then masks with
 *   `mov r3, #0xd / neg r3, r3`, i.e. -13 built in INT width. Declaring the
 *   field `unsigned char` makes gcc narrow the whole expression to QImode and
 *   emit the byte literal `mov r3, #0xf3`, one instruction short. Declaring it
 *   `signed char` keeps the arithmetic in SImode and produces the ROM's pair.
 *   25 differing -> 7, and the line count went 159 -> 160.
 *
 *   The existing signedness rule reads the LOAD (`ldrh` immediate = unsigned,
 *   `ldrsh` register-offset = signed). That rule is about halfwords and it does
 *   not extend to bytes: for a byte field, the load is `ldrb` either way and it
 *   is the ARITHMETIC WIDTH downstream that reveals the type. Look at how the
 *   mask is built, not at how the field is read.
 *
 * OTHER READINGS, all still required:
 *   - the second global is read INSIDE each case arm, not at the join (157->76)
 *   - the actor pointer is initialised to zero at the top (76->36)
 *   - the dispatch is unsigned, and only a cast AT THE SWITCH gives `bhi`;
 *     typing the global unsigned does nothing (36->35)
 *   - in each arm the LIMIT is computed before the global is read (35->25)
 *
 * A CORRECTNESS NOTE kept from the previous round: the mask is -0xd
 * (0xfffffff3), not ~0xd (0xfffffff2), and getting that wrong did NOT change
 * the diff count. The count does not validate semantics.
 *
 * WHAT IS LEFT: seven lines in three clusters, every one a pair of adjacent
 * instructions exchanged, with the line count now exact.
 *
 *     27/28   `ldr r3, [r2]` and `lsl r1, #0xe` -- the ROM reads the global
 *             BETWEEN the two halves of building the limit; we finish the
 *             limit first. This is the arg-interleave shape inside a switch
 *             arm, and the dominating-block lever cannot reach it because the
 *             two arms need DIFFERENT constants, so there is nothing to hoist.
 *     128/129 `str [r5,#0xc]` and `ldr [r5,#0x50]` exchanged.
 *     135-137 the two coordinate stores against the byte store.
 *
 * *** VERDICT CORRECTED (round 6). The paragraph below overstates. ***
 *
 * It says the arg-interleave shape is a wall. A corpus survey says otherwise:
 * gcc-2.96 emits an interleaved two-instruction constant build 1003 times in
 * already-matching code, and 851 of those are in ORDINARY C files with no
 * register pinning and no dma.h. The shape is routine, not unreachable.
 *
 * What is still true is the narrower claim: no spelling tried HERE has moved
 * these seven lines, and both scheduler flags are catastrophic. But "I have not
 * found it" is not "it cannot be done", and the earlier wording blurred the two.
 *
 * VERDICT (round 5), AS ORIGINALLY WRITTEN:
 *
 * The decisive test is the scheduler diagnostic. If the seven lines were a
 * pass that could be turned off, disabling it would help:
 *
 *     -fno-schedule-insns2     158 lines, 137 differing
 *     -fno-rerun-cse-after-loop 162 lines, 135 differing
 *
 * Both are catastrophic. sched2 is producing very nearly all of the correct
 * code -- 153 of 160 instructions -- and the seven differences are sched2
 * making a different LOCAL choice, not a pass acting where it should not. There
 * is no flag to remove and no pass to suppress.
 *
 * That combines with the shape of the residue. All seven are pairs of adjacent
 * instructions exchanged, the line count is exact, every register is the
 * ROM's, and the first cluster is the arg-interleave shape inside a switch arm
 * where the dominating-block lever provably cannot apply -- the two arms need
 * DIFFERENT constants, so there is nothing to hoist into a dominating block.
 *
 * Eleven source spellings have now been measured against these seven lines
 * across two rounds and not one has moved them; three made it worse. Writing
 * the interleave out literally -- `lim = 0xf0; v = L368c; lim <<= 14;`, which
 * is exactly what the ROM does -- is 17, worse than leaving it alone.
 *
 * SO: treat this as the interleave/scheduling wall, not as an unfinished
 * function. It is 153 of 160 correct with every reading settled and recorded
 * above, and it is a good specimen of the class BECAUSE everything else about
 * it is right. Anyone reopening it should have a new idea about sched2's
 * ordering, not a new spelling.
 *
 * MEASURED AND NEGATIVE this round:
 *   a fresh `int t` for the mask          161 lines, 41   (adds one, costs two)
 *   reusing the dead `v` for it           161 lines, 49
 *   reusing the dead `lim` for it         161 lines, 49
 *   an explicit `(int)` cast on the field 25   (no change)
 *   the mask written `~0xc`               25   (no change)
 *   loading `s` after the f18/f1c stores   7   (no change)
 *   the coordinate stores hoisted          7   (no change)
 *   `a->f55` moved down beside the other byte store  20 (worse)
 */
struct Sub {
    unsigned char pad00[9];
    signed char f9;
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
