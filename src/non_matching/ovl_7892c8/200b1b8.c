/* OvlFunc_888_200b1b8 -- 0x0200b1b8  [asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_c.s]
 *
 * NON-MATCHING, 20 of 79 encodings  (MEASURED, batch 333 brief E).
 *   COUNT DIFFERS (ref 79, ours 76) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   INSTRUCTIONS ref 65, ours 64; aligned-equal 82.3 per cent of the reference
 *   stream, nine hunks.  SIZE ref 184 bytes, ours 176.
 *
 *   THE RELOCATION SCARE THAT STOOD HERE WAS WRONG AND IS WITHDRAWN.  This park
 *   carried a banner saying the relocations differ in their SYMBOLS
 *   (the modulo helper) and that the figure is therefore not a distance.  The
 *   overlay's own linker script already aliases the two spellings -- see
 *   overlays/rom_7892c8/overlay.ld, the line that equates gcc's helper name to
 *   the overlay's RAM stub, which is line sixty-nine.  So the two objects link
 *   to one symbol and the only real relocation difference is the four-byte
 *   OFFSET shift that the length gap causes.  Five other parks in this tree
 *   record the same alias for their own banks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7892c8/200b1b8.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_c.s --func OvlFunc_888_200b1b8
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * NOT MATCHING, but the long-standing diagnosis was WRONG and is now corrected.
 * Best 74 lines against the ROM's 75, 17 differing, 176 bytes against 184.
 * It sat at 79 lines and 74 differing for three batches.
 *
 * Spawns a debris actor at a randomised offset from a map actor, gives it two
 * randomised timers and a callback, and merges two flag bits into the new
 * actor's sprite byte.
 *
 * WHAT THE PARK USED TO SAY, AND WHY IT WAS WRONG. It asserted the blocker was
 * that gcc needs FIVE callee-saved registers where the ROM needs four, and that
 * the ROM covers five values with four by SHARING r5 between two values of
 * different types. The sharing observation is true, but it was never the
 * blocker: gcc shares r5 quite happily. The fifth register was going to
 * something the park never looked at.
 *
 * THE ACTUAL BLOCKER WAS AN ADDRESS HOISTED ABOVE A CALL.
 *
 *     *(short *)(n + 0x64) = (__Random() % 0xa) + 5;
 *
 * 0x64 is past the halfword store's 5-bit scaled offset range, so the address
 * needs its own `mov`/`add`. gcc forms that address BEFORE evaluating the
 * right-hand side, which contains two calls -- so the address has to survive
 * them, and it takes a callee-saved register to do it. That was the fifth.
 *
 * THE FIX IS TO NAME THE RIGHT-HAND SIDE, which forces it to be evaluated into
 * a pseudo before the store address is formed:
 *
 *     r = (__Random() % 0x3c) + 0x1e;
 *     *(short *)(n + 0x66) = r;
 *
 * Doing this to the SECOND store alone took 79 lines -> 75, the ROM's length,
 * and 74 differing -> 52. GENERALISE IT: a store whose address needs an
 * explicit add, with a call in its right-hand side, will park that address in a
 * callee-saved register unless the value is named first.
 *
 * BUT DO NOT NAME BOTH. Naming the first store's right-hand side as well frees
 * r8 ENTIRELY -- 68 lines, 7 SHORT -- because the ROM genuinely spends r8 on a
 * value held across the calls. The two stores need opposite treatment, and that
 * is the whole shape of this function.
 *
 * A MEASUREMENT IS ONLY VALID IN THE SHAPE IT WAS TAKEN IN. This park recorded
 * pinning the map actor to r6 as WORSE (80 lines, 78 differing) and cited the
 * batch 210 hazard. That was measured BEFORE the hoist was fixed. With the
 * hoist gone the same pin is worth 16 differing (42 -> 26 in combination) and
 * is load-bearing. Re-measure a rejected lever after the shape changes.
 *
 * THE r5 PINS ARE NOW INERT AND HAVE BEEN REMOVED. The old best carried
 * `register ... q __asm__("r5")` and `register int x __asm__("r5")` to force
 * the sharing. With the hoist fixed, dropping both is byte-identical. Scaffolding
 * that was load-bearing under one diagnosis is dead weight under the next.
 *
 * THE POOLED ZERO: THE PARK'S "CLOSED, DO NOT RE-OPEN" VERDICT IS REFUTED, BUT
 * NOT IN THE DIRECTION IT EXPECTED.  The park wrote that the ROM's second zero
 * is an ordinary literal and that a symbol spelling is excluded because the
 * reference object carries no relocation at that pool word.  BOTH HALVES OF
 * THAT ARE UNSOUND.
 *
 *  - A SMALL SImode CONSTANT CAN NEVER REACH THE POOL.  `*thumb_movsi_insn`
 *    (arm.md:3838, operands at :3839-3840) lists constraint `I` -- an eight-bit
 *    immediate -- as ALTERNATIVE 1 and the memory-or-immediate `mi` pool path as
 *    ALTERNATIVE 6, and `constrain_operands` takes the FIRST alternative that
 *    needs no reload.  So an SImode zero into a low register is always a `mov`
 *    and the pool path is unreachable for it.  This is exactly the structural
 *    test the owner's standing ruling on symbol-table entries demands, read in
 *    the other direction: a pooled small value proves the source named
 *    something.  A high-register destination does not change it -- alternative 8
 *    pairs `*lh` with `*lh`, so no alternative accepts a constant at all and
 *    reload has to build the value in a low register with a `mov` first.
 *  - THE RELOCATION ARGUMENT IS ABOUT A RECONSTRUCTION, NOT THE ORIGINAL.  The
 *    reference .s is this project's own disassembly; its `.word 0` was written
 *    by the splitter from a word that read as zero, so the absence of a
 *    relocation in the ASSEMBLED reconstruction says nothing about what the
 *    original object had there.  objcmp compares two reconstructions of the same
 *    side on that word and will agree with whichever we write.
 *
 * CONTRAST THAT WITH A CASE WHERE THE POOLED ZERO *IS* A LITERAL, because the
 * distinction is a mode, not a value.  `*thumb_movhi_insn` (arm.md:4318,
 * operands at :4319-4320) lists the pool path `mn` as ALTERNATIVE 1 and the
 * eight-bit `I` as ALTERNATIVE 5 -- the OPPOSITE order -- so a HImode zero is
 * always pooled.  src/overlays/rom_7b9cb4/ovl_30_c_c_b.c is a landed function
 * that pools a zero for exactly that reason and says so.  THE RULE: a pooled
 * zero reached by `ldrh` is ordinary gcc output; a pooled zero reached by a full
 * `ldr` is not, and this function's is a full `ldr`.
 *
 * SO THE REMAINING EIGHT BYTES ARE NOT A SPELLING PROBLEM.  Which also settles
 * the second open piece:
 *
 *  - THE ROM'S `b .L3264` IS `create_fix_barrier` (arm.c:5240; the jump, the
 *    barrier and the label after it are emitted at :5305-5309).  Its only caller
 *    is :5572, reached from the branch at :5551-5556 whose own comment says it
 *    runs when a fix will not fit in this pool and "there [are] no natural
 *    barriers that we could use" -- so gcc MANUFACTURES one by jumping around
 *    it.  The park called this a translation-unit-level property; IT IS NOT.
 *    This reference .s holds ONE function and 184 bytes, and the corpus has 382
 *    mid-function pool dumps of exactly this shape in generated (byte-matching)
 *    asm, one of them in a 44-byte object -- so the shape is reachable in a tiny
 *    single-function TU.  What forces it there is `pool_range`: 1020 for the
 *    SImode pool alternative (arm.md:3856) against 64 for the HImode one
 *    (arm.md:4353).  A fix with the short range cannot wait for the natural
 *    barrier at the end of the function and gcc inserts the jump.  Our candidate
 *    has no such fix, which is why our pool sits after `bx` with nothing over it.
 *
 * THE TWO PIECES ARE THE SAME PIECE.  Identify the pooled word and the barrier
 * comes with it; keep writing a bare zero and neither can appear.  An owner
 * decision on a symbol-table entry is the gate, and this park now MEETS the
 * structural-impossibility bar the ruling sets -- it did not before, because the
 * argument offered was a line count.  It would still fail the ruling's second,
 * COMPLETION test while the reference .s itself spells the word as a literal.
 *
 * THE TAIL SWAP IS DOWN TO TWO ENCODINGS, AND THE PARK SEARCHED THE DIAGONAL.
 * It recorded four spellings of the two `&` expressions -- "fused, split, either
 * order" -- and concluded none swapped the registers.  The two masks are not
 * symmetric and want OPPOSITE treatment, which the diagonal cannot reach.
 * Varying them independently:
 *
 *     first mask   second mask   `|` order    encodings differing of 79
 *     ----------   -----------   ---------    -------------------------
 *     split        split         v | t                     23  (the old body)
 *     split        fused         v | t                     25
 *     fused        split         v | t                     23
 *     fused        fused         v | t                     25
 *     split        split         t | v                     24
 *     split        fused         t | v                     26
 *     fused        split         t | v                     20  <- this file
 *     second mask written before the first                 28
 *     an explicit trailing `return;`                       23  (inert)
 *
 * This file is the fused-first / split-second / reversed-`|` cell.  What is left
 * in the tail is one register pair: the ROM finishes `orr r3, r2` and stores r3
 * where we finish `orr r2, r3` and store r2.
 *
 * MEASURED IN BATCH 319'S ROUND (differ counts, rom 75 lines), kept for the
 * record; the `(int)&_CONST_0` row's verdict is the one withdrawn above:
 *
 *     name 2nd store's RHS only                     75 lines, 52
 *     ... + pin map actor to r6                     76, 42
 *     ... + assign the zero just before its use     74, 26
 *     ... + name 1st store's RHS too                74, 17
 *     ... with the r5 pins dropped                  74, 17   (identical)
 *     ... + `(int)&_CONST_0` for the zero           74, 16
 *     name BOTH RHSs, no r6 pin                     68, 74   (r8 freed entirely)
 *     both zeros written bare, letting CSE pool     73, 42
 *     zero assigned at the top of the function      79, 78
 *     nested positive ifs instead of early returns  74, 17   (inert)
 *     tail's two masks computed in the other order  74, 22
 *
 * NEXT: the pooled word's identity, which is an owner call, not a measurement.
 */
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__CreateActor(int id, int x, int y, int z);
extern void OvlFunc_888_200b144(void);

void OvlFunc_888_200b1b8(int slot)
{
    register unsigned char *a __asm__("r6");
    unsigned char *n;
    unsigned char *q;
    int x;
    int y;
    int t;
    int v;
    int z;
    int r;
    int u;

    a = __MapActor_GetActor(slot);
    if (a == 0)
        return;
    x = *(int *)(a + 8) + ((__Random() % 0x14) << 16) - 0xa0000;
    y = *(int *)(a + 0xc) + ((0xf & __Random()) << 16) - 0x80000;
    n = __CreateActor(0x8f << 1, x, y, *(int *)(a + 0x10));
    if (n == 0)
        return;
    q = *(unsigned char **)(n + 0x50);
    n[0x55] = 0;
    u = (__Random() % 0xa) + 5;
    *(short *)(n + 0x64) = u;
    z = 0;
    r = (__Random() % 0x3c) + 0x1e;
    *(short *)(n + 0x66) = r;
    *(void **)(n + 0x6c) = OvlFunc_888_200b144;
    q[0x26] = z;
    t = 0xc & (*(unsigned char **)(a + 0x50))[9];
    v = -13;
    v &= q[9];
    q[9] = t | v;
}
