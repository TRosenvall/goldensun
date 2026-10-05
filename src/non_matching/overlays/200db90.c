/* OvlFunc_969_200db90 -- 0x0200db90  -- UNCHANGED THIS BATCH.
 *
 * STILL NON-MATCHING, **2 differing encodings of 43** (ref 43 / ours 43, first
 * differing index 31).  RE-DERIVED THIS BATCH and CONFIRMED; the body below is
 * byte-for-byte the parked one and NOTHING HERE PROPOSES A CHANGE TO IT.
 * PIN-FREE, SHIM-FREE, FLAG-FREE, DEVICE-FREE.
 *
 * Verify with: python3 tools/objcmp.py src/non_matching/overlays/200db90.c asm/overlays/rom_7f6e64/ovl_314_c_c_c.s --func OvlFunc_969_200db90
 *
 * SPLIT: tools/datacheck.py says this .s carries .bss AND .data and holds TWO
 * functions (OvlFunc_969_200da28, OvlFunc_969_200db90), so it needs a
 * TEXT/DATA split.  Neither function reads a data label, so the split needs
 * NO new export.  (Unchanged from the park; re-verified.)
 *
 * ===== WHAT THIS BATCH ADDS: INDEPENDENT CONFIRMATION OF THE FLOOR =====
 *
 * The park closes at 2 on a floor argument: cse1 always folds the constant into
 * the `plus` (thumb addsi3 takes operand 2 through `nonmemory_operand`, and
 * predicates are checked before reload while constraints are not), reload then
 * re-materialises it immediately before the add, and sched2 cannot hoist a
 * reload-created insn above an equal-priority insn that precedes it.
 *
 * THAT NOW HAS A SECOND, INDEPENDENT WITNESS.  Its module-mate and near-twin
 * OvlFunc_969_200b600 (src/non_matching/ovl_7f6e64/200b600.c) was taken from
 * 6 of 44 to 2 of 44 this batch, and its remaining 2 is INSTRUCTION-FOR-
 * INSTRUCTION this park's residue:
 *
 *     rom    ldr rX, =const / ldrh r3, [r6] / add r3, rX
 *     ours   ldrh r3, [r6] / ldr rX, =const / add r3, rX
 *
 * On that function, TWELVE tail spellings were measured from a 2-differing base
 * and ELEVEN ARE EXACTLY INERT -- including this park's own named-int spelling,
 * the plain `*p = *p - K`, `*p -= K`, a `do { } while (0)` placed BETWEEN the
 * constant's assignment and the read of *p, the assignment wrapped in its own
 * `do { } while (0)`, and a bare `__asm__ volatile ("")` in the same gap.  The
 * only mover is `*p = -K + *p`, which is FOUR BYTES SHORTER and so a length
 * change rather than a distance.
 *
 * > THE cse1 BOUNDARY DOES NOT HELP.  That was the obvious next idea --
 * > `cse_end_of_basic_block` ends a cse1 block at NOTE_INSN_LOOP_END, and a
 * > `do { } while (0)` buys that boundary for zero instructions -- so a barrier
 * > between the assignment and the use ought to stop the fold.  Measured on the
 * > twin, in two placements plus the asm form: EXACTLY INERT.  Recorded here so
 * > the next reader does not spend the round on it.
 *
 * ALSO A CORRECTION TO THIS PARK'S DIAGNOSIS, measured on the twin.  Step (2)
 * credits the order to haifa's rank_for_schedule tiebreaks -- the longer
 * INSN_DEPEND list, then INSN_LUID.  On the twin, sched2 never faces a tiebreak:
 *
 *     .23.sched2  Ready list (t=143): 79 / (t=144): 79 / (t=145): 110 / (t=146): 110
 *
 * where 79 is the ldrh and 110 the reload-created `(set (reg:SI 1 r1)
 * (const_int -2048))`.  They are NEVER SIMULTANEOUSLY READY, so the order is
 * forced one level earlier, at readiness, and no priority or dependent-count
 * manipulation can reach it.  Why 110 becomes ready only at t=145 is NOT
 * established -- it is a plain const_int with no memory operand -- so do not
 * build on that part.  The tiebreak reasoning is not wrong so much as never
 * consulted.
 *
 * ALSO RE-CONFIRMED: this park's "THE CURRENT SPELLING IS LOAD-BEARING, not
 * merely equivalent" claim rests on measurements taken at the 11-DIFFERING
 * stage, before the barrier and the a+8 naming took it to 2.  On the twin at 2,
 * the spelling is NOT load-bearing -- eleven spellings tie.  The claim should be
 * read as "load-bearing at 11", which is a different statement.
 *
 * ===== EVERYTHING BELOW IS THE PARK'S OWN RECORD, UNCHANGED =====
 *
 * Its twin is OvlFunc_925_200b460 (0x0200b460) in
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_c.s; the two differ in ONE constant
 * (0xa4 against 0x90), so one solution elevates both -- and with
 * OvlFunc_969_200b600 now on the same residue, ONE ANSWER LANDS THREE.
 *
 * The full inherited analysis -- the r8/r10 source-order lever, `mul` copying
 * its second operand, the `do { } while (0)` barrier worth 11 -> 4, naming the
 * a+8 value worth 4 -> 2, the not-an-aliasing-problem finding, and the batch-321
 * flag sweep (nine flags exactly inert at 2, -fno-schedule-insns2 worse at 14)
 * -- is in src/non_matching/overlays/200db90.c and is NOT restated here.
 */
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200db90(unsigned char *a)
{
    unsigned short *p;
    unsigned char *q;
    int ang;
    int r;
    int c, s;
    int v;
    int w;

    p = (unsigned short *)(a + 0x64);
    q = *(unsigned char **)(a + 0x68);
    ang = *p;
    c = __cos(ang);
    r = *(int *)(a + 0x30) + 0x1c;
    *(int *)(a + 8) = *(int *)(q + 8) + c * r;
    s = __sin(ang);
    w = *(int *)(a + 8);
    *(int *)(a + 0x10) = (s << 4) + (0xa4 << 16);
    *(int *)(a + 0x38) = w;
    *(int *)(a + 0x40) = *(int *)(a + 0x10);
    do { } while (0);
    v = 0xfffffe00;
    v += *p;
    *p = v;
}
