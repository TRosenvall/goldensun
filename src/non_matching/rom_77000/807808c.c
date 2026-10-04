/* Func_807808c -- 0x0807808c, asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * STILL NON-MATCHING.  PARK AT 4 of 86 encodings, device-free, down from the
 * park's 5.  A further 2 of 86 is reachable and diagnosed; see "THE VOLATILE
 * VARIANT" below.  SIZE EXACT, 86 against 86 encodings, both figures.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b322/F/p1_candidate.c \
 *     asm/rom_77000/rom_77320_a_c_c.s --func Func_807808c
 *
 * INSTALLED PATH, if it ever lands: src/rom_77000/rom_77320_a_c_c_b.c.
 * Split shape: TEXT-ONLY.  tools/datacheck.py prints nothing (no data section).
 * tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_807808c --dry-run:
 *   _a.s Func_8077f70 (140 lines), _b.s Func_807808c (97), _c.s Func_8078144 (115).
 * PINS: 0.  No shim, no fakematch row, no flag group.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE PARK CLAIMED, AND WHAT SURVIVED
 *
 * The park named both residues correctly and priced both correctly.  Its
 * OBSERVATIONS all reproduced.  What it got wrong is that it tested its
 * candidate edits ONE AT A TIME, and its own rejected list contains BOTH HALVES
 * of a two-part fix:
 *
 *     "inline index"              rejected at 6      <-- half one
 *     "a one-statement sign extend" rejected at 8    <-- half two
 *     the two together                               4
 *
 * This is exactly docs/elevation.md's "a rejected-because-worse edit that is
 * HALF of a two-part fix", and it is the fourth shape in tools/crossfire.py's
 * list.  Reproduced at depth 4 over five edits; the full table is in
 * scratch_elev/b322/F/FINDINGS.md.
 *
 * WHY THOSE TWO AND NOT EITHER ALONE.  There were two independent residues:
 *
 *   R1 (indices 10-13) `ldr r2,=gState` against `lsl r1,#1`, plus the
 *      base/index register roles.  The inline subscript fixes the ORDER and
 *      breaks the ROLES (6).  The one-statement sign extend, added on top, puts
 *      the ROLES back -- a NON-LOCAL effect: the extra intermediate pseudo in
 *      the sign extend shifts local-alloc's quantity priorities for the whole
 *      block.  Indices 10, 11, 12 and 13 then all go exact.
 *
 *   R2 (indices 18-21) the two halfword stores against the sign-extend pair.
 *
 * THE SCHEDULER ARITHMETIC FOR R2, from `.23.sched2` with -fsched-verbose=6
 * (haifa-sched.c `rank_for_schedule`: priority -> CLASS -> dependent count ->
 * INSN_LUID, best LAST in the ready array):
 *
 *   park body:  strh[0x38] 36   strh[0x3a] 34   lsl 36   asr 35
 *   this body:  strh[0x38] 35   strh[0x3a] 36   lsl 36   asr 35
 *   ROM wants:  strh[0x38], strh[0x3a], lsl, asr
 *
 * A STORE'S PRIORITY IS SET BY WHICHEVER SHIFT OVERWRITES ITS SOURCE REGISTER.
 * The edge is an ANTI dependence (the shift writes the register the store
 * reads), `arm_adjust_cost` gives REG_DEP_ANTI cost 0, so the store inherits
 * that shift's priority exactly.  In the park body the in-place `lsl r1,r1`
 * overwrites r1, so the 0x38 store inherits 36 and the 0x3a store -- whose r3
 * nothing overwrites -- falls to prio(call)+1 = 34.  In this body the
 * intermediate lands in r3, so the roles swap.  Either way exactly one of the
 * two stores is at 36 and the other loses to the lsl.
 *
 * THE VOLATILE VARIANT -- 2 of 86, and the mechanism is a DEPENDENCE, not a
 * priority.  scratch_elev/b322/F/v1/d1.c is this body with BOTH stores written
 * `*(volatile unsigned short *)`.  Each volatile cast ALONE is EXACTLY INERT
 * (4 and 4); together they are worth 2.  That is crossfire shape four, "two
 * edits each exactly inert, jointly worth the residue", and one-at-a-time
 * testing cannot see it.  Why it works: a volatile MEM makes
 * `sched_analyze_insn` call `flush_pending_lists`, which adds a dependence from
 * the second volatile store to the first, so prio(0x38 store) becomes
 * prio(0x3a store) + 1 = 37 and the pair is emitted adjacent, in source order,
 * ahead of the lsl.  Residue then: indices 20/21 only, `lsl r3,r1,#16 /
 * asr r1,r3,#16` against the ROM's in-place `lsl r1,r1,#16 / asr r1,r1,#16`.
 *
 * WHY THE IN-PLACE SHIFT AND THE VOLATILE PAIR WILL NOT COEXIST (measured, 3):
 * the in-place pair needs local-alloc's `combine_regs` to tie the intermediate
 * to its dying source, and local-alloc.c refuses when the source "is not local
 * to this block OR DIES MORE THAN ONCE" -- `r1` is set twice in this block (the
 * 0x34 load, then the sign-extend result), so it never gets a quantity.  Three
 * single-set variables (`t1 = r1 << 16; s1 = t1 >> 16;`) DO chain, and give the
 * ROM's in-place pair -- see v1/g1.c -- but then nothing overwrites r3, the
 * 0x3a store drops back to 34, and it sinks below the shifts again: 3 of 86.
 * So R2 is a THREE-CORNERED constraint, and 2 and 3 are the two corners
 * reachable so far.  NOT a bound; the evidence is the three figures 2, 3, 4 and
 * the priority table above.
 *
 * MEASURED FLAT (all exactly 4, on top of this body): an explicit temp for the
 * sign extend, `r1 = (short)r1`, `r0 = r1 * 0x4000`, swapping the two stores,
 * giving the 0x36 value its own variable, every declaration-order permutation
 * of r0/r1/r3, a fresh variable declared first.  Twenty crossed rows, dead
 * flat -- so the lever for the last 4 is not in spelling, statement order or
 * declaration order.
 * WORSE: the deref form `*(gState + ...)` and `((unsigned char *)gState)[...]`
 * both 77 at 84 instructions (RELOC+COUNT: they fold the symbol).
 * `r1` volatile-loaded 73 at 88.  Shifts before the stores 72 (RELOC+MEM).
 */
extern int GetPartySize(void);
extern void *GetUnit(int unit);
extern unsigned char gState[];

void Func_807808c(int sel)
{
    void *r5;
    int r0;
    int r1;
    int r3;
    int i;
    int n;
    int k;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        r5 = GetUnit(gState[(0xfc << 1) + i]);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        r1 = (r1 << 16) >> 16;
        r0 = r1 << 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x14) = r3;
        if ((r3 << 16) != 0) {
            goto label_0x3a;
        }
        r3 = *(short *)((char *)r5 + 0x38);
        if (r3 == 0) {
            goto label_0x3a;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x14) = r3;
    label_0x3a:
        r0 = *(short *)((char *)r5 + 0x3a);
        r1 = *(short *)((char *)r5 + 0x36);
        r0 <<= 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x16) = r3;
        if ((r3 << 16) != 0) {
            goto label_sel;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_sel;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_sel:
        if (sel == 1) {
            *((char *)r5 + 0x131) = 0;
            *((char *)r5 + 0x140) = 0;
        }
    }
}
