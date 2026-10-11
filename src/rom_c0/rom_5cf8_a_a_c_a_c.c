/* Cluster Func_80063bc..Func_8006408 extracted from asm/rom_c0/rom_5cf8_a_a_c_a_c.s.
 *
 * MATCHING.  One .c is one TU and this piece holds exactly these two functions,
 * both of which were parked; they landed together in batch 335.
 *   ok Func_80063bc  33 encodings
 *   ok Func_8006408  34 encodings
 *   OK whole file -- 156 bytes, 67 encodings and 8 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c0/rom_5cf8_a_a_c_a_c.c \
 *     asm/rom_c0/rom_5cf8_a_a_c_a_c.s --whole
 *
 * Pins: 0.  Devices: none.  Production flags.  Four bare `__asm__ volatile ("")`
 * barriers, which tools/shimcount.py reports separately and this tree does not
 * treat as a fakematch.
 *
 * WHAT THEY DO.  Two halves of one sound-driver API -- the hand `.s` names them
 * StartMusicFadeOut and StartMusicFadeIn.  Each posts a request into a two-byte
 * command slot at ewram_2002220 (byte 1 = the command code 0x80 / 0x81, byte 0 =
 * "pending"), but only when its own state word is already clear; otherwise it
 * returns -1 and touches nothing.  The body runs with interrupts disabled and
 * restores the saved IME on the way out.  The matched Func_800651c
 * (src/rom_c0/rom_5cf8_a_c_b.c) is the teardown over the same five globals, and
 * the matched SetIntrHandler (src/rom_c0/rom_2e00_c_c_b.c) is where the
 * IME-disable idiom below is already written up.
 *
 * THE IME STORE IS NOT A TRANSCRIPTION SLIP.  The disable is a halfword store of
 * the register's OWN ADDRESS into itself, writing 0x0208 into REG_IME.  Only bit
 * 0 is live and 0x208 has it clear, so this disables interrupts exactly like a
 * plain zero would, one instruction cheaper -- no zero needs materialising
 * because the address is already in hand.  `*ime = (unsigned int)ime;`
 * reproduces it; `REG_IME = 0;` does not, and neither does
 * `REG_IME = REG_ADDR_IME;` -- gcc materialises the truncated constant from a
 * fresh literal instead of reusing the address register.
 *
 * ===================================================================
 * THE THREE LEVERS THAT CLOSED THIS PIECE, AND WHY EACH IS NEEDED
 * ===================================================================
 *
 * The parks stood at three of thirty-three and seven of thirty-four, lengths
 * equal and instruction counts 27 and 27 both, so both figures were true
 * distances.  Batch 334 had written the first one up as a STRUCTURAL BOUND: the
 * residue was `mov r3, #0x80` scheduled into the stall after the pool load of
 * the interrupt-enable address, and from sched2's own table
 *
 *     insn 28  r2=0x4000208   prio 10  cost 2
 *     insn 42  r3=0x81        prio  8  cost 1
 *     insn 31  r1=[r2]        prio  8  cost 2
 *     insn 37  [r2]=r2        prio  8
 *
 * with arm_adjust_cost (arm.c:2425-2427) zeroing every anti- and
 * output-dependence, so prio(volatile read) equals prio(the first store after
 * the IME write) while prio(the constant) is that plus one -- a gap of exactly
 * one that no arrangement of the critical section can close, and which the class
 * tie-break (haifa-sched.c:4068-4095) would not close even at equal priority.
 *
 * EVERY NUMBER IN THAT DERIVATION REPRODUCES.  It is still a correct statement
 * about PRIORITY.  It is not a bound, because rank_for_schedule only ever ranks
 * insns that are READY, and the constant was ready at t = 0 for one reason: it
 * had no predecessor at all.
 *
 * LEVER 1 -- A BARRIER GIVES IT ONE.  An empty `__asm__ volatile ("")` is an
 * ASM_INPUT, and sched_analyze_2 (haifa-sched.c:3567-3597) adds a dependence on
 * every reg_last_uses, reg_last_sets and reg_last_clobbers entry, calls
 * flush_pending_lists, and sets reg_pending_sets_all, which
 * sched_analyze_insn (:3780-3789) turns into a dependence for every insn that
 * follows.  It is a two-sided barrier that emits nothing.  Put one between the
 * IME disable and the first store and the constant can no longer reach the stall
 * slot; the volatile read issues there instead, which is what the reference
 * does.  Func_80063bc goes from three to ZERO on that single line.
 *
 * LEVER 2 -- A CHAR *DECL* STORE AND A CHAR *POINTER* STORE ARE DIFFERENT
 * MEMORY OPERANDS, AND POST-RELOAD THE ALIAS SET IS THE ONLY THING THAT CAN
 * SEPARATE TWO STORES AT ALL.
 *
 * lang_get_alias_set (c-common.c:3347-3351) returns alias set ZERO for any
 * 'r'-class reference expression whose type has char precision -- so `p[0] = 1`
 * through an `unsigned char *` is set 0, and set 0 aliases everything.  A bare
 * VAR_DECL reference is class 'd', falls through to `return -1` (:3355), and
 * get_alias_set then uses the alias set of the TYPE, which is non-zero.
 * Measured in .19.flow2 on this very function:
 *
 *     (mem:QI    (reg r6) 0)     p[0] = 1             alias set 0
 *     (mem/f:QI  (reg r6) 2)     ewram_2002220 = 1    alias set 2
 *     (mem/f:SI  (reg r5) 4)     ewram_20023ac = v    alias set 4
 *
 * This decides the SECOND function's first hunk, the state store one slot early
 * against the flag-byte store.  With the flag byte written through the pointer,
 * write_dependence_p (alias.c) sees set 0, cannot use DIFFERENT_ALIAS_SETS_P,
 * and gives an output dependence in SOURCE order, which no priority can
 * reorder.  And the fallback cannot save it: base_alias_check would say
 * "differing symbols never alias", but find_base_term needs reg_base_value, and
 * init_alias_analysis fills that from REG_EQUIV only for PSEUDOS (the
 * `REGNO >= FIRST_PSEUDO_REGISTER` guard in its insn walk).  AFTER RELOAD EVERY
 * BASE IS UNKNOWN, so x_base is 0, canon_rtx (x) == x, and base_alias_check
 * returns 1.  That is the general fact worth carrying: sched2 can reorder two
 * stores ONLY when their alias sets differ and neither is 0.
 *
 * Writing the flag byte as the DECL `ewram_2002220 = 1;` removes the edge -- and
 * that alone is not enough, because the pair then ties on priority (4 and 4),
 * ties on the class rung, ties on depend_count, and falls through to LUID
 * (haifa-sched.c, the last rung of rank_for_schedule), where the state store is
 * still earlier.  The SAME split applied to the other tail store,
 * `ewram_20023a4 = 0;` in place of a named pointer, deletes the state store's
 * third dependent, so depend_count reads 1 against 2 and the flag store wins one
 * rung BEFORE LUID is reached.  Seven became four became two.
 *
 * LEVER 3 -- THE CLOSING BARRIER.  The last two were the IME restore scheduled
 * into the load-delay stall of the pooled zero: once `ewram_20023a4 = 0` is
 * alias set 2 it no longer aliases the volatile HImode restore (set 5), so the
 * restore floats up into the slot.  A second barrier before `*ime = save;`
 * restores the ordering.  A critical section with a barrier at each end is the
 * same construct twice, so both functions carry both; the second function alone
 * also suffices and reads identically.
 *
 * MEASURED, AND NONE OF THE THREE WORKS ALONE: the opening barrier by itself is
 * three and four; the alias split without the closing barrier is two; the
 * closing barrier without the alias split leaves the first hunk untouched.
 *
 * ORDER OF THE CRITICAL SECTION.  All one hundred and twenty orderings were swept
 * TWICE, once per spelling set, and only flag-byte / state / halfword /
 * flag-byte / zero is in the basin either time -- next best nine.  Every ordering
 * that moves the state store off source position two costs the argument its
 * incoming register: `.18.greg` shows the allocno gaining a conflict with hard
 * reg 0, the body gains a `mov r5, r0` and the push list gains r7.  That is the
 * allocno_compare mechanism (global.c:598-621) the parks already described, and
 * it is why the memory dependence could not simply be sidestepped by reordering
 * the two stores in the source: it had to be DELETED.
 *
 * Note the two functions' store orders genuinely differ -- Func_80063bc writes
 * its state word before its ready flag and Func_8006408 after -- and with set 0
 * out of the way that difference is visible in the output, not folded away.
 */

typedef volatile unsigned short vu16;

extern int ewram_2002080;
extern unsigned char ewram_2002220;
extern unsigned short ewram_2002008;
extern unsigned char ewram_20023a4;
extern int ewram_20023ac;
extern unsigned short ewram_2002238;

int Func_80063bc(int a, int b)
{
    unsigned char *flags;
    vu16 *ime;
    int cur;
    int save;

    flags = &ewram_2002220;
    cur = ewram_2002080;
    if (cur != 0)
        return -1;
    ime = (vu16 *)0x4000208;
    save = *ime;
    *ime = (unsigned int)ime;   /* 0x0208: bit 0 clear, so interrupts off */
    __asm__ volatile ("");
    flags[1] = 0x80;
    ewram_2002080 = a;
    ewram_2002008 = b;
    ewram_20023a4 = cur;        /* cur is zero here; the ROM reuses the register */
    flags[0] = 1;
    __asm__ volatile ("");
    *ime = save;
    return 0;
}

int Func_8006408(int v)
{
    unsigned char *p;
    vu16 *ime;
    int cur;
    int save;

    cur = ewram_20023ac;
    p = &ewram_2002220;
    if (cur != 0)
        return -1;
    ime = (vu16 *)0x4000208;
    save = *ime;
    *ime = (unsigned int)ime;   /* 0x0208: bit 0 clear, so interrupts off */
    __asm__ volatile ("");
    p[1] = 0x81;
    ewram_20023ac = v;
    ewram_2002238 = cur;
    ewram_2002220 = 1;
    ewram_20023a4 = 0;
    __asm__ volatile ("");
    *ime = save;
    return 0;
}
