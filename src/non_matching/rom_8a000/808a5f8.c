/* Func_808a5f8 (0x0808a5f8) -- NON-MATCHING, 26 differing of 113.
 * Blocker class: register allocation, on a function whose structure is settled.
 * Never attempted before batch 276.
 *
 * asm/rom_8a000/rom_8a5f8_a_a.s (2 functions, so landing needs a split).
 *
 * 103 instructions -- the other non-converging target in batch 276's size
 * experiment, and the one that produced the round's most useful negative result
 * about HOW to search at this size.
 *
 * THE FIRST 0x54 BYTES MATCH IN LENGTH: relocations carry the same four symbols in
 * the same order, and `_call_via_r0` (0x1c) and `_GetFlag` (0x54) are at IDENTICAL
 * offsets. Divergence is entirely in the tail -- 0xd4 against the ROM's 0xcc, so
 * we are 8 bytes long.
 *
 * FOUR STRUCTURAL LEVERS, each worth 2-3 instructions (35 -> 26):
 *   c1 baseline                                                    35
 *   c3 the skip loop as a BARE `goto` loop, so there is no
 *      NOTE_INSN_LOOP_BEG and the `=0xfffff000` pool load stays
 *      inside the loop                                             32
 *   c5 `extern void *__start_overlay[]` indexed `[5]`, to stop the
 *      `symbol+20` pool fold                                       30
 *   c9 both gState builds as explicit `unsigned int` with `<<= 1`
 *      steps -- the PlayMapMusic idiom                             28
 *   p04 field-extraction order `cur, w, hi, fl`                     26   <- best, below
 *
 * THE INTERACTION IS NON-ADDITIVE AND SIGN-FLIPPING, and this is the finding that
 * matters more than the number. Against the 30 baseline:
 *     the first gState explicit build alone      31   (WORSE)
 *     the second alone                           78   (MUCH worse)
 *     both together                              28   (better)
 * A one-lever-at-a-time hill climb rejects each half and never finds the pair.
 * That is the real cost of the 100+ band: not "too many independent residues",
 * which is what tools/pickable.py's cut-off asserts, but more levers that have to
 * be right SIMULTANEOUSLY. See reports/batch-276.md.
 *
 * PLATEAUS ARE WIDE AND FLAT HERE TOO: four declaration-order positions for `v`
 * all produced identical output. About 12 distinct ideas over 47 files.
 *
 * REMAINING: `mov r8, r0` placement; `out`'s register; the `=0x1ff` pool load
 * hoisted above `and r5, r3` instead of reusing r3; `=0xfff` and `lsr r2, #0x14`
 * scheduling; `mov r0, r10` against `mov r2, r10`.
 *
 * NEXT: read `.18.greg` against the four hi-register values rather than sweeping
 * more field orders -- and if a lever measures worse, PAIR IT with another before
 * discarding it, on the evidence above.
 */
extern unsigned char gState[];
extern void *__start_overlay[];
extern int _GetFlag(int id);

void Func_808a5f8(int arg)
{
    unsigned int *p;
    unsigned int *save;
    unsigned int hdr;
    unsigned int w;
    int cur;
    int hi;
    int fl;
    int v;
    int out;
    int z;
    unsigned int g;
    unsigned int o;
    unsigned int g2;
    unsigned int o2;

    g = (unsigned int)gState;
    o = 0xe0;
    o <<= 1;
    g += o;
    z = 0;
    cur = *(short *)((char *)g + z);
    p = ((unsigned int *(*)(void))__start_overlay[5])();
    v = 0x3e7;
    out = 0;
    if (arg == 0x3e7)
        return;
    w = *p++;
    if ((w & 0xfffff000) != 0)
        goto skip;
    w &= 0xfff;
    if (w == 0x1ff)
        goto store;
 match:
    if (w != cur)
        goto skip;
    save = p;
    goto entry;
    do {
        if (hi == 0xff || hi == arg) {
            if (fl == 0 || _GetFlag(fl) == 0) {
                v = w;
                out = cur;
                goto store;
            }
        }
 entry:
        hdr = *p++;
        cur = (hdr & (0xff << 12)) >> 12;
        w = hdr & 0xfff;
        hi = (hdr & (0xff << 20)) >> 20;
        fl = hdr & (0x80 << 21);
        if (fl != 0)
            fl = *p++;
    } while (w != 0x1ff && hi != 0);
    v = *save & 0x1ff;
    goto store;
 skip:
    w = *p++;
    if ((w & 0xfffff000) != 0)
        goto skip;
    w &= 0xfff;
    if (w != 0x1ff)
        goto match;
 store:
    if (v != 0x3e7) {
        g2 = (unsigned int)gState;
        o2 = 0xe0;
        o2 <<= 1;
        *(short *)(g2 + o2) = v;
        o2 = 0xe1;
        o2 <<= 1;
        *(short *)(g2 + o2) = out;
    }
}
