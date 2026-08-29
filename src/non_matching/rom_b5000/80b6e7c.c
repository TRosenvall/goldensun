/* GetWeaponType  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b6e7c.s
 * Best screen: 18 instructions in disagreeing regions, of 27 (rom 27, ours 23).
 *
 * BLOCKER CLASS: the index is strength-reduced away.
 *
 * The ROM keeps a counter and recomputes the byte offset each iteration:
 *
 *      lsl r1, r4, #1 / ldrh r2, [r0, r1]      ... add r4, #1
 *
 * gcc turns the counter into a pointer walk and drops the shift, which is four
 * instructions cheaper over the loop. `j = i << 1;` inside the loop with `i++`
 * at the bottom -- written below, and exactly the ROM's shape -- is what gets
 * strength-reduced.
 *
 * `-fno-strength-reduce` was NOT tried here because it has been byte-identical
 * to the default on every function it has been tried on in this corpus
 * (Func_80a9d84, batch 63). If someone wants to spend a screen on it, that is
 * the reason it was skipped rather than an oversight.
 *
 * The rest is right: 0x1ff is legitimately pooled (it does not fit `mov #imm8`,
 * so this is NOT the pool tell and the constant needs no name), `m = 0x1ff;
 * m &= v;` makes the constant the AND's destination, and the sentinel is built
 * as `n = 1; n = -n;` to match `mov r2, #1 / neg r2, r2`.
 *
 * LATER: re-derived independently before the park file was noticed, which is
 * how tools/unparked_candidates.py came to match EVERY identifier in a park
 * rather than only Func_/OvlFunc_ shapes -- this function's real name is what
 * slipped through. The rediscovery reached the same conclusion, so the note
 * above is confirmed rather than revised. Three things it did add:
 *
 *   - AT -O1 the streams are the same LENGTH (27 against 27) and 20 differ,
 *     against 27 differing and a short stream at -O2. That is suggestive but
 *     NOT evidence of an -O1 rule: no Makefile rule covers rom_b6e7c, and 20
 *     differing is nowhere near a match, so nothing was changed. Recorded so
 *     the next reader does not re-run the same three flag screens.
 *     --no-rerun-cse and --no-sched2 are both identical to the default.
 *
 *   - The ROM sign-extends the ALREADY LOADED halfword (`lsl r3, r2, #16 /
 *     asr r3, #16`) instead of re-reading it with `ldrsh`. That is the tell
 *     for a variable WIDER than short holding the loaded value and being cast
 *     down -- with `unsigned short v`, gcc knows the cast is just a compare
 *     against 0xffff and never emits the shift pair.
 *
 *   - Acting on that and declaring `int v` makes it WORSE, not better: 22
 *     differing at -O1 against 20, and 25 at -O2 against 27 with a stream two
 *     lines short. So the sign-extension tell is real but is not reachable
 *     from the variable's type while the index is still being strength-reduced.
 *     The strength reduction is upstream of it and has to go first.
 */
extern unsigned char Lc593c[] __asm__(".Lc593c");

int GetWeaponType(int id)
{
    unsigned char *t;
    int key;
    int i;
    unsigned int j;
    int v;
    int m;
    int s;
    int n;

    key = id;
    i = 0;
    t = Lc593c;
    do {
        j = i << 1;
        v = *(unsigned short *)(t + j);
        m = 0x1ff;
        m &= v;
        if (key == m)
            return *(unsigned short *)(t + j) >> 9;
        s = (v << 16) >> 16;
        n = 1;
        n = -n;
        i++;
    } while (s != n);
    return 6;
}
