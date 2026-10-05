/* Func_807808c (0x0807808c) -- asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * PARK HELD AT 3 differing encodings of 86.  SIZE EXACT (86 against 86, 86
 * instructions both sides), RELOCATIONS IDENTICAL (objcmp prints no RELOCATIONS
 * line), per-opcode memory profile the reference's exactly:
 * ldr=2 ldrb=1 ldrh=2 ldrsh=4 strb=2 strh=6.  Figure re-derived in batch 327 G.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807808c.c asm/rom_77000/rom_77320_a_c_c.s --func Func_807808c
 *
 * INSTALLED PATH, if it ever lands: src/rom_77000/rom_77320_a_c_c_b.c.
 * Split shape: TEXT-ONLY, tools/datacheck.py prints nothing (no data section).
 *   tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_807808c --dry-run:
 *     _a.s Func_8077f70 (140 lines), _b.s Func_807808c (97), _c.s Func_8078144 (115).
 * All THREE functions in this .s are parked (8077f70 at 3, this at 3, 8078144 at
 * 4) and their headers quote ALTERNATIVE splits of the same file.  Whichever
 * lands first decides the suffixes; install_batch.py's splits phase repoints the
 * others.  PINS: 0.  No shim, no fakematch row, no flag group.  No device.
 *
 * ===========================================================================
 * BATCH 327 G.  THE 3 IS NOW A PROVEN CORNER, NOT AN OPEN QUESTION, AND THE
 * PROOF IS THE SAME ONE THAT CLOSES Func_8077f70 IN THIS SAME .s.
 *
 * The residue is unchanged -- indices 21,22,23, `strh r3,[r5,#0x3a]` sinking
 * below the ROM's in-place `lsl r1,#16 / asr r1,#16`:
 *
 *      ref                       ours
 *  20  strh r1, [r5, #0x38]      strh r1, [r5, #0x38]
 *  21  strh r3, [r5, #0x3a]      lsls r1, r1, #16
 *  22  lsls r1, r1, #16          asrs r1, r1, #16
 *  23  asrs r1, r1, #16          strh r3, [r5, #0x3a]
 *
 * THE DEPENDENCE GRAPH, read from `-da -fsched-verbose=6` -> `.23.sched2`
 * block 1.  The call to __divsi3 has priority 33 and:
 *
 *     insn 41  r1=zxn([r5+0x34])  prio 38
 *     insn 44  r3=zxn([r5+0x36])  prio 36
 *     insn 48  [r5+0x38]=r1       prio 36   deps: 81 67 55
 *     insn 52  [r5+0x3a]=r3       prio 34   deps: 81 379 67
 *     insn 55  r1=r1<<0x10        prio 36   deps: 81 58
 *     insn 58  r1=r1>>0x10        prio 35   deps: 81 67 61
 *     insn 61  r0=r1<<0xe         prio 34   deps: 81 67
 *     insn 67  call __divsi3      prio 33
 *
 * Insn 48 reaches 36 ONLY by its anti dependence on insn 55, which overwrites
 * its source r1 (`arm_adjust_cost` returns 0 for REG_DEP_ANTI, arm.c:2425-2427,
 * so an anti dependence transfers the overwriting insn's priority EXACTLY).
 * Insn 52's best dependent is the call: 33 + 1 = 34.
 *
 * **THE BOUND, AND THE PART OF arm_adjust_cost THIS PROJECT HAD NOT WRITTEN
 * DOWN.  arm.c:2430-2432 returns 1 for ANY true dependence whose CONSUMER is a
 * CALL_INSN** -- "Call insns don't incur a stall, even if they follow a load".
 * Mind the argument order: haifa's `insn_cost` calls
 * `ADJUST_COST (used, link, insn, cost)`, so arm_adjust_cost's first parameter
 * is the CONSUMER, not the producer.  Insn 52's own `cost` column reads 2 and
 * the applied link cost is 1, which is this rule firing.
 *
 * So EVERY insn whose only consumer is the call has priority exactly
 * prio(call)+1, and a pre-call store is CAPPED at 34.  Meanwhile the
 * sign-extend chain sits three true-dependence hops above the call
 * (55 -> 58 -> 61 -> 67 = 36, 35, 34, 33) and that 36 is FORCED by the hop
 * count: no spelling of three instructions is fewer than three hops.
 * `rank_for_schedule` tests priority FIRST and returns on it
 * (haifa-sched.c:4040-4042), so no tie-break rung below is even reachable.
 *
 * ==> insn 52 can reach 36 only through a cost-0 ANTI/OUTPUT dependence on an
 * insn of priority >= 36 that FOLLOWS it.  The only such insn in the block is
 * insn 55, the sign-extend's first shift -- i.e. the sign-extend intermediate
 * would have to live in r3, which is exactly the 4-body that breaks the
 * in-place pair.  **The 3/4 trade is therefore a proven property of the
 * dependence graph, and 3 is the better corner.  Reopening it needs a different
 * BLOCK, not a different spelling.**
 *
 * Also checked and ruled out: there is NO label between `strh r3,[r5,#0x3a]`
 * and `lsl r1,#16` in the reference (rom_77320_a_c_c.s:168-169), so the ROM's
 * order is not a basic-block-boundary effect.
 *
 * MEASURED IN BATCH 327 (crossfire depth 2, 8 edits, 24 valid rows, EVERY ROW
 * EXACTLY 3, ref 86 / ours 86, no MEM flag).  Three of these are structurally
 * DIFFERENT ILs and not re-spellings:
 *   3   re-read the dividend AND divisor back from the just-stored 0x38
 *   3   re-read both from 0x34
 *   3   dividend from 0x38, divisor from 0x34
 *   3   store3a-first / load36-after-store38 / copy3a-one-stmt / dividend-mult
 *  88   hoist the clamp constant above the division -- RELOC, 90 instructions:
 *       a constant live across the call needs a call-saved register.  Refuted.
 *
 * NOTE ON THE READ-BACK ROWS, because they settle a standing guess: they keep
 * the reference memory profile exactly (ldrsh=4), i.e. cse DOES common the
 * re-read with the stored register and DOES produce the ROM's in-place lsl/asr.
 * So "the original read the value back from 0x38" is CONSISTENT with the ROM
 * and is NOT distinguishable from the direct form.  It buys nothing.
 *
 * FLAG-GROUP ROUTE CLOSED FOR THE WHOLE OBJECT, with three witnesses.  All
 * three functions in this .s have a store sunk by sched2, which invites
 * SCHED2_CFLAGS.  Measured OBJCMP_EXTRA=-fno-schedule-insns2:
 * Func_8077f70 3 -> 17 (relocations go dirty), Func_807808c 3 -> 8,
 * Func_8078144 4 -> 8 (relocations dirty).  REFUTED.  Do not re-propose it.
 *
 * STILL TRUE FROM BATCH 326, and the reason this body reads 3 and not 4: three
 * single-set locals for the sign extend let local-alloc's `combine_regs` tie
 * the intermediate to its dying source, so indices 22/23 are byte-exact.  With
 * `r1 = (r1 << 16) >> 16;` the variable is SET TWICE in the block, never gets a
 * quantity, and the pair cannot be in place.  The two `volatile` casts batch
 * 322 measured it with were never load-bearing.
 *
 * FREE CORRECTNESS DIVIDEND, measured exactly inert: `GetUnit`'s real parameter
 * type is `unsigned int`, from its landed definition at
 * src/rom_77000/rom_77320_a_a_c_c_a_b.c:136.
 */
extern int GetPartySize(void);
extern void *GetUnit(unsigned int id);
extern unsigned char gState[];

void Func_807808c(int sel)
{
    void *r5;
    int r0;
    int r1;
    int t1;
    int s1;
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
        t1 = r1 << 16;
        s1 = t1 >> 16;
        r0 = s1 << 14;
        r0 /= s1;
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
