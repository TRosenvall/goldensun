/* OvlFunc_946_200add0  [overlays/rom_7ced6c] -- MATCHING (batch 338, brief B).
 *
 * Was NON-MATCHING at 13 of 72 encodings.  Now BYTE-IDENTICAL:
 *   objcmp --func : OK -- 160 bytes, 72 encodings and 8 relocations identical
 *   objcmp --whole: OK whole file -- 160 bytes, 72 encodings, 8 relocations
 * Instruction counts agreed on both sides throughout (64/64), so the park's
 * figure was a true distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_c_c_c_c.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_c_c_c_c.s \
 *     --whole
 *
 * Pin-free.  tools/piece.py: the reference holds this function ALONE, and
 * tools/datacheck.py is silent -- whole-file conversion, no split, no exports.
 *
 * ===================== WHAT THE 13 WERE, AND THE FIX =====================
 *
 * The park's symptom was exactly right: 80 asm lines against 80 and all
 * thirteen differing lines are one two-register rename, `a` in r6 and `b` in
 * r5 in the ROM against the reverse in ours.  It is the CALLEE-SAVED flip, not
 * the scratch-set flip -- both registers are in `push {r5, r6, r14}` and both
 * values cross calls, so no member of r0-r3 is a candidate for either.
 *
 * ITS DIAGNOSIS WAS RIGHT AND UNREACHABLE.  From .17.lreg / .18.greg:
 *     reg 32 (a)  8 refs / 61 insns, 1 set, crosses 10 calls  -> r5
 *     reg 33 (b)  6 refs / 58 insns, 2 sets, crosses  9 calls -> r6
 *     reg 34 (c)  5 refs /  8 insns, 2 sets                   -> r3
 *     ";; 3 regs to allocate: 34 32 33"
 * allocno_compare (global.c:598-620) is
 * `floor_log2(n_refs)*n_refs/live_length*10000*size`, giving 12500, 3934
 * (3*8/61) and 2069 (2*6/58) -- and the dump's own `regs to allocate` line
 * confirms that order.  But `a`'s 8 refs are 1 set + 5 `cmp` + 2 call
 * arguments and `b`'s 6 are 2 sets + 4 uses, and NEITHER COUNT CAN MOVE
 * without changing the instruction stream.  `pri(b) > pri(a)` would need
 * n_refs(a) <= 6, or n_refs(b) >= 8, or live_length(a) > 2*live_length(b);
 * all three are out of reach in a function this shape.
 *
 * THE LEVER IS A CONFLICT, NOT A PRIORITY.  Give `b - 1` its own local instead
 * of decrementing `b` in place:
 *
 *     -   b = b - 1;                              +   d = b - 1;
 *     -   __Func_8010704(a, b, 1, 3, c, b);       +   __Func_8010704(a, d, 1, 3, c, d);
 *     -   __Func_8010704(0, 0, 1, 3, a, b);       +   __Func_8010704(0, 0, 1, 3, a, d);
 *
 * `d` is then referenced only in the tail, so .17.lreg reports it as
 * "used 4 times across 10 insns IN BLOCK 12" -- a BLOCK-LOCAL quantity, which
 * local-alloc (local-alloc.c:1341-1420) places before global.c ever runs.  It
 * crosses one call, so the first callee-saved register in REG_ALLOC_ORDER
 * (arm.h:989 is `3,2,1,0,12,14,4,5,6,7`, with r4 call-used under
 * -fcall-used-r4) is r5, and .18.greg duly reports `35 in 5`.  `a` now
 * conflicts with hard reg 5 -- its conflict line gains a literal `5` -- so `a`
 * is pushed to r6, and `b`, whose range now ends at the subtract and never
 * reaches `d`, inherits r5:
 *
 *     park  ";; Register dispositions: 32 in 5  33 in 6  34 in 3"
 *     here  ";; Register dispositions: 32 in 6  33 in 5  34 in 3  35 in 5"
 *
 * THE ALLOCNO ORDER IS STILL `34 32 33`.  Nothing about the priority changed.
 * `b` and `d` share r5 because `b` dies at the subtract, which is why the
 * ROM's `sub r5, #1` is destructive on the same register.
 *
 * GENERAL FORM: a value whose only uses lie in one basic block is taken out of
 * global's hands entirely and claimed by local-alloc first, so SPLITTING A
 * REUSED LOCAL CAN REASSIGN A CALLEE-SAVED REGISTER THAT NO PRIORITY LEVER CAN
 * REACH.  Batch 337's OvlFunc_common1_588 needed "two locals instead of the
 * park's reused one" and read it as a -O1 crossing; this is the same device
 * acting alone, with the pass named.
 *
 * FOUR SPELLINGS OF IT, ALL FOUR BYTE-IDENTICAL: `d = b - 1;` after the `c`
 * re-read (this body), before it, `b - 1` written inline at all three sites,
 * and separate locals for both `d` and the tail re-read.  So neither the
 * placement of the decrement nor a separate local for the re-read matters --
 * only that `b - 1` is not `b`.
 *
 * MEASURED INERT, CARRIED FORWARD FROM THE PARK (all at 13): the call in every
 * arm; declaring the two values in the opposite order; swapping the two
 * computations (13, redistributed -- the two `ldr` offsets exchange instead);
 * a separate local for the tail's recomputed field.  WORSE: the call hoisted
 * after the join with the argument assigned per arm (66), both call arguments
 * assigned per arm (66), naming the actor pointer in a local (70).
 * autosweep (batch 338): both cheap axes CLOSED -- 8 flag groups (best 13;
 * -O1 and -fno-schedule-insns2 each 24 of 72) and 9 anchor placements (13).
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200add0(void)
{
    int a;
    int b;
    int c;
    int d;

    a = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0x10) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    if (a == 6) {
        if ((unsigned int)(c - 9) <= 2)
            OvlFunc_946_2009774(0x10, 0x20, 0);
        else
            OvlFunc_946_2009774(0x10, 0x70, 0);
    } else if (a == 8) {
        if ((unsigned int)(c - 9) <= 2)
            return;
        OvlFunc_946_2009774(0x10, 0x50, 0);
    } else if (a == 9) {
        OvlFunc_946_2009774(0x10, 0x40, 0);
    } else if (a == 0xc) {
        OvlFunc_946_2009774(0x10, 0x10, 0);
    } else if (a == 0xd) {
        return;
    }
    __WaitFrames(2);
    c = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    d = b - 1;
    __Func_8010704(a, d, 1, 3, c, d);
    __Func_8010704(0, 0, 1, 3, a, d);
}
