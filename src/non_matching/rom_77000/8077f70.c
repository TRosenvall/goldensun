/* Func_8077f70  --  0x08077f70, asm/rom_77000/rom_77320_a_c_c.s
 *
 * PARK HELD AT 3 differing encodings of 120.  SIZE EQUAL (ref 120 against ours
 * 120), RELOCATIONS IDENTICAL (objcmp prints no RELOCATIONS line), so the 3 IS
 * a distance.  Figure re-derived in batch 327 G.  PINS: 0.  SHIMS: 0.
 * DEVICES: 0.  FLAGS: none.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/8077f70.c asm/rom_77000/rom_77320_a_c_c.s --func Func_8077f70
 *
 * SPLIT SHAPE: asm/rom_77000/rom_77320_a_c_c.s holds THREE functions
 * (Func_8077f70 first, then Func_807808c, Func_8078144) and tools/datacheck.py
 * prints nothing (no data section).
 *   python3 tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_8077f70
 * Installed path would be src/rom_77000/rom_77320_a_c_c_b.c.
 * All THREE functions in this .s are parked and their headers quote
 * ALTERNATIVE splits of it.  Whichever lands first decides the suffixes;
 * install_batch.py's splits phase repoints the others.
 *
 * ===========================================================================
 * BATCH 327 G.  THE REMAINING 3 IS THE SAME THREE ENCODINGS AS Func_807808c's,
 * IN THE SAME .s, WITH IDENTICAL PRIORITY NUMBERS -- AND IT IS NOW PROVEN
 * UNREACHABLE RATHER THAN OPEN.
 *
 * The residue, byte for byte the twin of 807808c's:
 *
 *      ref                       ours
 *      strh r1, [r5, #0x38]      strh r1, [r5, #0x38]
 *      strh r3, [r5, #0x3a]      lsl  r1, #0x10
 *      lsl  r1, #0x10            asr  r1, #0x10
 *      asr  r1, #0x10            strh r3, [r5, #0x3a]
 *
 * `-da -fsched-verbose=6` -> `.23.sched2`, the block after the GetUnit call.
 * Call __divsi3 priority 33:
 *
 *     insn 61  r1=zxn([r5+0x34])  prio 38
 *     insn 64  r3=zxn([r5+0x36])  prio 36
 *     insn 68  [r5+0x38]=r1       prio 36   deps: 101 87 75
 *     insn 72  [r5+0x3a]=r3       prio 34   deps: 101 473 87
 *     insn 75  r1=r1<<0x10        prio 36   deps: 101 78
 *     insn 78  r1=r1>>0x10        prio 35   deps: 101 87 81
 *     insn 81  r0=r1<<0xe         prio 34   deps: 101 87
 *     insn 87  call __divsi3      prio 33
 *
 * Those are Func_807808c's numbers exactly (36/34/36/35/34/33), on different
 * insn numbers.  **Two functions in one object, one mechanism, one bound.**
 *
 * THE BOUND (full derivation in src/non_matching/rom_77000/807808c.c, kept
 * there so it is written once).  In one line: `arm_adjust_cost` (arm.c:2425-2432)
 * returns 0 for REG_DEP_ANTI/OUTPUT and **1 for any true dependence whose
 * CONSUMER is a CALL_INSN**, so every insn feeding only the call sits at exactly
 * prio(call)+1 = 34 -- a pre-call store is capped there -- while the three-hop
 * sign-extend chain is forced to 36.  `rank_for_schedule` returns on the
 * priority rung (haifa-sched.c:4040-4042), so the class and dependent-count
 * rungs below it are unreachable; the park's earlier note that "a store's
 * 3-to-2 advantage is unreachable while it is 2 points behind" was right about
 * the symptom and the gap is 36 against 34.  The only way to 36 is a cost-0
 * anti dependence on the first shift, i.e. putting the sign-extend intermediate
 * in r3, which destroys the in-place pair.
 *
 * ALSO SETTLED THIS BATCH, and it covers all three functions in the object:
 * SCHED2_CFLAGS is REFUTED.  OBJCMP_EXTRA=-fno-schedule-insns2 gives
 * Func_8077f70 3 -> 17 with relocations going dirty, Func_807808c 3 -> 8,
 * Func_8078144 4 -> 8 with relocations dirty.  Three independent witnesses in
 * one object: the original was NOT built with scheduling off.
 *
 * STILL TRUE FROM BATCH 325, and worth keeping: the `tu-pool` enrolment was
 * refuted AS A CLASS for all 30 such parks, because every input to
 * `add_minipool_forward_ref` is per-function -- `push_minipool_fix`'s address is
 * `insn_addresses` within the function and `arm_reorg` walks only this
 * function's chain -- so a standalone TU has every lever the original had.
 * Cause 1 (the pool's internal order, six of the old nine) is closed; this 3 is
 * all that is left and it is cause 2.
 */
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern void Func_8079ae8(int unit);
extern void CalcStats(int unit);
extern void *GetUnit(int unit);
extern void EquipItem(int unit);
extern unsigned char gState[];

void Func_8077f70(void)
{
    void *r5;
    unsigned char *g;
    int r0;
    int r1;
    int r2;
    int r3;
    int i;
    unsigned short t;

    ClearFlag(0x20);
    ClearFlag(0x21);
    SetFlag(0x901);
    Func_8079ae8(5);
    CalcStats(5);
    ClearFlag(0x11b);
    SetFlag(0x11a);

    for (i = 0; i < 2; i++) {
        r5 = GetUnit(i);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        r1 <<= 16;
        r1 >>= 16;
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
            goto label_items;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_items;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_items:
        for (r1 = 0, r2 = 0xd8; r1 <= 0xe; r2 += 2, r1++) {
            t = *(unsigned short *)(r2 + (int)r5) & 0x1ff;
            if (t == 0xf) {
                *(unsigned short *)(r2 + (int)r5) = 0x10;
                EquipItem(i);
                break;
            }
        }
        Func_8079ae8(i);
        CalcStats(i);
    }

    GiveInnateMove(0, 0x8c);
    GiveInnateMove(0, 0x95);
    GiveInnateMove(1, 0x8c);
    GiveInnateMove(2, 0x8d);
    g = gState;
    *(int *)(g + 0x10) += 0x96 << 1;
}
