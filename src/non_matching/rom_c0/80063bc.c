/* Func_80063bc  --  0x080063bc, asm/rom_c0/rom_5cf8_a_a_c_a_c.s
 *
 * NON-MATCHING, 3 of 33 encodings  (MEASURED, batch 332).
 *   Stream lengths now AGREE: ref 33, ours 33, instructions 27 and 27.
 *   So this figure IS a distance.  The aligned residue is TWO instructions
 *   in ONE hunk: the 0x80 constant, which the second scheduling pass puts in
 *   the load-delay stall after the pool load of the interrupt-enable address
 *   and the reference leaves that slot empty.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c0/80063bc.c \
 *     asm/rom_c0/rom_5cf8_a_a_c_a_c.s --func Func_80063bc
 *
 * Pins: 0.  Devices: none.  Production flags.
 *
 * WHAT IT DOES
 * Posts a request into the block starting at ewram_2002080, but only when the
 * slot is empty -- otherwise it returns -1 and touches nothing. The body runs
 * with interrupts disabled and restores the saved IME on the way out.
 *
 * THE IME STORE IS NOT A TRANSCRIPTION SLIP. The ROM's disable is a halfword
 * store of the register's OWN ADDRESS into itself, which writes 0x0208 into
 * REG_IME. Only bit 0 of that register is live and 0x208 has it clear, so this
 * disables interrupts exactly like a plain zero would, one instruction cheaper
 * -- no zero needs materialising because the address is already in hand.
 * `*ime = (unsigned int)ime;` reproduces it exactly; writing `REG_IME = 0;`
 * does not, and writing `REG_IME = REG_ADDR_IME;` does not either -- gcc
 * materialises the truncated constant from a fresh literal instead of reusing
 * the address register.
 *
 * ===================================================================
 * BATCH 332: THE OLD DIAGNOSIS IS REFUTED.  IT WAS NOT REGISTER
 * ALLOCATION ORDER AND IT WAS REACHABLE FROM C.
 * ===================================================================
 *
 * The superseded claim was twenty-seven of thirty-three on streams of
 * unequal length, blamed on REG_ALLOC_ORDER and recorded as "not reachable
 * from C", with the bound "the store order cannot be permuted to fix it,
 * because the store order is what already matches."  Both halves were wrong.
 *
 * THE MECHANISM IS allocno_compare, global.c:598-621:
 *
 *     pri = ((double)(floor_log2(n_refs) * n_refs) / live_length)
 *           * 10000 * size
 *
 * with n_refs and live_length taken from REG_N_REFS / REG_LIVE_LENGTH at
 * global.c:447-451, allocnos sorted descending by it (global.c:542) and
 * allocated in that order (global.c:552-554), ties broken on allocno index
 * (global.c:620) which follows pseudo number which follows parameter order.
 *
 * WHY THAT COSTS AN INSTRUCTION.  find_reg's pass 0 does exclude the
 * registers other conflicting allocnos prefer -- allocno[num].regs_someone_
 * prefers, global.c:1014 -- but the same statement also excludes every
 * register not yet in regs_used_so_far (global.c:1013, "we never allocate a
 * register for the first time in pass 0"), and pass 1 copies `used` back from
 * `used1` (global.c:1023) WITHOUT the preference.  So the protection is dead
 * for a register nobody has touched yet: r0 goes to whichever allocno is
 * allocated FIRST, not to the argument that prefers it.  An argument arriving
 * in r0 keeps it for free; the other argument costs a copy.  With both
 * arguments at two refs, priority is 2/live_length, so THE ARGUMENT STORED
 * LAST LOSES r0.
 *
 * MEASURED, and the model reproduces the printed allocation order exactly:
 *   old body   a 2 refs / 17 insns, b 2 refs / 13   order cur,b,a,flags,slot
 *              -> a in r5, and an extra copy of b into r0
 *   this body  a 2 refs / 13 insns, b 2 refs / 16   order cur,a,slot,b,flags
 *              -> a in r0, no copy, and the three pointers land in the
 *                 reference's own r5/r6/r7
 *
 * TWO DIMENSIONS, AND NEITHER WORKS ALONE.
 *   1. STORE ORDER IN THE SOURCE IS NOT STORE ORDER IN THE OUTPUT.  The
 *      second scheduling pass reorders stores to distinct symbols freely, so
 *      moving `ewram_2002080 = a;` to the front of the critical section
 *      shortens a's live range by four insns and the store still comes out
 *      fourth.  What IS locked is the relative order of the stores whose
 *      address or constant shares r3 after reload -- an output/anti chain --
 *      and that is the constraint the old header mistook for "the order is
 *      fixed".  A full sweep of all one hundred and twenty orderings of the
 *      five critical-section statements bottoms out at five in stream terms.
 *   2. ASSIGNING THE FLAGS POINTER BEFORE THE STATE LOAD moves the state
 *      address pseudo's SET one insn later, dropping its live_length from
 *      twenty-two to twenty and lifting its priority above the second
 *      argument's.  Alone it is worth nothing; crossed with (1) it is what
 *      puts slot in r5 and flags in r7 instead of the other way round.
 *
 * WHAT IS LEFT, AND WHAT IT IS NOT.  From the second scheduling pass's own
 * trace: block 2's ready list at t=0 holds the interrupt-address pool load
 * and the 0x80 constant; the pool load issues and occupies two clocks; at
 * t=2 both the constant and the volatile halfword read are ready, and
 * rank_for_schedule (haifa-sched.c:4029) decides on the very first test it
 * makes, INSN_PRIORITY (haifa-sched.c:4039-4042).  The constant wins because
 * after reload r3 carries every later address and constant in the block, so
 * its dependence chain runs the block's length.  Making it the LAST r3 writer
 * would shorten the chain but would also move its store to the end of the
 * block, which the reference contradicts.
 *
 * Disabling the second scheduling pass is REFUTED, not untested: it reads ten
 * in stream terms and additionally swaps two pool words, so the relocation
 * offsets diverge.
 *
 * MEASURED INERT (no change from this body's figure): the AND-side spellings
 * do not apply here; what was measured inert is naming the state pointer
 * (`slot = &ewram_2002080` used for both the load and the store, which the
 * sibling park's own note explains -- a fixed address names nothing), the
 * `save` local as int, unsigned int or a truncating cast on the disable, and
 * naming the 0x80 in a local.  MEASURED WORSE: a plain non-volatile pointer
 * for the interrupt register, and a halfword-typed `save`, which changes the
 * size.
 * *
 * ---------------------------------------------------------------------------
 * BATCH 334: THE REMAINING 3 IS A STRUCTURAL BOUND, NOT AN OPEN ORDERING
 * QUESTION.  THE 0x80 CANNOT BE PUSHED OUT OF THE LOAD-DELAY SLOT FROM C.
 *
 * The residue is exactly what this header says: `mov r3, #0x80` scheduled into
 * the stall after `ldr r2, =REG_IME` instead of after the IME read and write,
 * three positions rotated and nothing else.  Read out of sched2's own verbose
 * dump (`-da -fsched-verbose=6`, block 2), the numbers are
 *
 *     insn 30  `r2 = 0x4000208`   prio 9  cost 2
 *     insn 42  `r3 = 0x80`        prio 8  cost 1
 *     insn 33  `r1 = [r2]`        prio 7  cost 2   (the volatile read)
 *     insn 39  `[r2] = r2`        prio 7           (the volatile write)
 *     insn 43  `[r7+1] = r3`      prio 7
 *
 * At t = 0 the pool load wins and occupies two clocks; at t = 2 the ready list
 * is insn 33 and insn 42, and rank_for_schedule (haifa-sched.c:4038-4042)
 * returns on INSN_PRIORITY, 8 against 7.
 *
 * WHY THAT GAP IS FIXED AT EXACTLY ONE.  arm_adjust_cost (arm.c:2425-2427)
 * returns 0 for EVERY anti- and output-dependence.  The edge from the volatile
 * read to the volatile write is a memory anti-dependence -- flush_pending_lists
 * adds it as REG_DEP_ANTI -- so it costs nothing, and the write's own successors
 * are the later stores.  Therefore
 *
 *     prio(volatile read) = prio(first store after the IME write)
 *     prio(the constant)  = prio(its own store) + 1
 *
 * and when the first store after the IME write IS the 0x80's store, the
 * constant beats the volatile read by exactly one, always.  Reordering the
 * critical section cannot fix it: priority falls monotonically along the
 * store chain, so putting any other store between the IME write and the 0x80
 * store gives that store the higher priority and emits IT before the 0x80 pair,
 * which the reference contradicts.  That is why this header's own sweep of all
 * one hundred and twenty orderings bottomed out above the current figure.
 *
 * AND A TIE DOES NOT HELP EITHER, which is the part worth writing down because
 * it closes the obvious next idea.  On equal priority rank_for_schedule falls
 * through to the class test (haifa-sched.c:4068-4095): it prefers the insn
 * INDEPENDENT of the last-scheduled insn over one data-dependent on it.  The
 * constant is independent of the pool load; the volatile read is data-dependent
 * on it.  So the constant wins the tie as well, and the target is not
 * prio(read) >= prio(constant) but prio(read) > prio(constant) -- two clocks of
 * headroom that no arrangement of these five statements can produce.
 *
 * MEASURED IN BATCH 334, both EXACTLY INERT at 3: declaring the flag array
 * `volatile unsigned char` (so its two stores are volatile MEMs), and naming
 * the 0x80 in a local before the store.  MEASURED WORSE at 8: making all four
 * ewram globals volatile.  MEASURED WORSE still: moving the halfword store
 * ahead of the 0x80 store, which costs an instruction.
 *
 * SO: THE NEXT MOVE ON THIS FUNCTION IS NOT A SPELLING.  Either the 0x80's
 * dependence structure has to change -- which means the ROM's source is doing
 * something here that is not three independent stores -- or this is a
 * SCHED2_CFLAGS question for the owner, and note that this header already
 * refutes the flag: disabling the second scheduling pass reads ten in stream
 * terms and swaps two pool words.
 *
 * PIECE STATUS, BATCH 334: Func_8006384, the third function in this .s, now
 * MATCHES, and the piece was therefore SPLIT so that it could land --
 * see src/rom_c0/rom_5cf8_a_a_c_a_b.c for the four crossed levers that did it,
 * one of which (the REG_SIOCNT width and relocation) this header's sibling
 * already named.  This park and Func_8006408's stay together in the _c half.
 *
 * ITS SIBLING Func_8006408 (0x08006408) IS THE SAME FUNCTION over a different
 * pair of globals AND SHARES THIS EXACT MECHANISM -- see that park, which the
 * same two dimensions moved from thirty-one on unequal lengths to its current
 * claim.  Both are in THIS .s, together with Func_8006384, so all three must
 * match before the piece can be converted: one .c is one TU.
 */

typedef volatile unsigned short vu16;

extern int ewram_2002080;
extern unsigned char ewram_2002220[];
extern unsigned short ewram_2002008;
extern unsigned char ewram_20023a4;

int Func_80063bc(int a, int b)
{
    unsigned char *flags;
    vu16 *ime;
    int cur;
    int save;

    flags = ewram_2002220;
    cur = ewram_2002080;
    if (cur != 0)
        return -1;
    ime = (vu16 *)0x4000208;
    save = *ime;
    *ime = (unsigned int)ime;   /* 0x0208: bit 0 clear, so interrupts off */
    flags[1] = 0x80;
    ewram_2002080 = a;
    ewram_2002008 = b;
    ewram_20023a4 = cur;        /* cur is zero here; the ROM reuses the register */
    flags[0] = 1;
    *ime = save;
    return 0;
}
