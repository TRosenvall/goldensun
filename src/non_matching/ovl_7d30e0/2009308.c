/* OvlFunc_948_2009308  --  0x02009308  [asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a.s]
 *
 * NOT MATCHING. Best 4 of 58, LENGTH EXACT, and ITS TWIN IS ELEVATED --
 * src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a_b.c
 * (OvlFunc_948_200941c) is the same forty lines of C with five constants
 * changed and it matches exactly. That is what makes this one worth keeping
 * rather than sweeping: the source is known good, so the residue isolates a
 * pure allocation question.
 *
 * WHAT THE TWIN ESTABLISHED, all of it needed here too and all of it landed:
 *
 *   - The two tile coordinates are SIGNED DIVISIONS, not shifts. The ROM's
 *     `cmp r3,#0 / bge / ldr r1,=0xfffff / add r3,r1 / asr r3,#20` is exactly
 *     what gcc emits for `x / 0x100000` on a signed int; writing `>> 20` gives
 *     the bias-free form and does not match.
 *   - A range test has to be spelled with the UNSIGNED OFFSET IDIOM.
 *     `tx >= 0x15 && tx <= 0x17` emits two compares and two branches;
 *     `(unsigned)(tx - 0x15) <= 2` gives the ROM's `sub / cmp #2 / bhi`. Note
 *     the OTHER range in this same function, `ty > 9 && ty <= 0xb`, IS two
 *     compares in the ROM -- so the two spellings are both real and the
 *     listing decides which. Two values wide takes compares, three takes the
 *     subtract.
 *   - The stored constant is pooled unless it is assigned to a local as its
 *     own statement, even though 0x5b fits an 8-bit immediate. Store width,
 *     not magnitude.
 *   - ONE VARIABLE SERVES THE OFFSET AND THEN THE VALUE. The ROM does
 *     `mov r3,#0xc1 / lsl r3,#1 / add r2,r7,r3 / mov r3,#0x5b / strh r3,[r2]`
 *     -- r3 is refilled one instruction after the `add` consumes it. Two source
 *     variables put the value's mov before the add; reusing one puts it after.
 *
 * WHAT REMAINS -- TWO CALLEE-SAVED LOCALS IN EACH OTHER'S REGISTERS:
 *
 *     rom   asr r6, r3, #0x14   /  ldr r7, [r3]      tx in r6, the iwram base in r7
 *     ours  asr r7, r3, #0x14   /  ldr r6, [r3]      swapped, and nothing else differs
 *
 * The twin allocates the SAME source shape the other way round (tx in r7, base
 * in r6) and matches, so this is not something the source expresses. The two
 * functions differ only in which variable carries the subtract-range test and
 * which carries the pair of compares, and that is apparently enough to reorder
 * local-alloc's preference.
 *
 * MEASURED, six forms:
 *
 *     declaration order swapped (w before g)              4   (inert)
 *     ty computed before tx                               4   (residue moves, same size)
 *     the range bound written 2u instead of 2             4   (inert)
 *     w assigned before both divisions                   18
 *     tx pinned to r6                                    24   and 57 lines
 *     tx pinned to r6 AND w pinned to r7                 51   and 51 lines
 *
 * THE PIN IS ACTIVELY DESTRUCTIVE HERE and that is the useful part. Pinning tx
 * to the register the ROM uses does not move tx there -- it collapses the whole
 * bias-and-shift computation into r6, so the intermediate that should live in
 * r3 is gone and the function comes out short. The pin names where a value is
 * WRITTEN; this value is written by a three-instruction sequence that gcc was
 * building in a scratch register, and naming the destination rewrites the
 * sequence rather than relocating its result. Compare
 * src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c_b.c, where pinning two
 * competing callee-saved locals DID settle them: there each was a single `mov`
 * of an immediate, with nothing upstream to disturb.
 *
 * NEXT: the class is "two callee-saved locals, correct source, allocator picks
 * the opposite assignment, and the pin cannot be applied because the value is
 * computed rather than moved". A handle would need to change local-alloc's
 * ORDER of consideration without touching the computation -- perhaps a third
 * short-lived local between them, which the preheader-constant entry in
 * docs/elevation.md says can shift a pair's register assignment. That was not
 * tried.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_948_2009308(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char *w;
    int tx, ty, v;
    short *q;
    register int p0 __asm__("r0");

    p = __MapActor_GetActor(0);
    tx = *(int *)(p + 8) / 0x100000;
    ty = *(int *)(p + 0x10) / 0x100000;
    w = iwram_3001ebc;
    p0 = 0x88; p0 <<= 2;
    if (__GetFlag(p0) == 0) {
        g = gState;
        if (*(short *)(g + (0x93 << 2)) == 0
            && *(short *)(g + 0x24a) != 8
            && (unsigned)(tx - 0x15) <= 2
            && ty > 9 && ty <= 0xb) {
            p0 = 0x88; p0 <<= 2;
            __SetFlag(p0);
            v = 0xc1;
            v <<= 1;
            q = (short *)(w + v);
            v = 0x5b;
            *q = v;
        }
    }
}
