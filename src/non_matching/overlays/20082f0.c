/* OvlFunc_949_20082f0 -- 0x020082f0, NOT MATCHING: 4 of 91 encodings (ref 91, ours 91).
 *
 * THE LENGTHS NOW AGREE: 68 / 68 instructions, 224 / 224 bytes, 21 / 21 pool
 * entries, relocations IDENTICAL.  The park this replaces read 41 of 91 at
 * 66 / 68 -- two instructions SHORT -- so the length gap is CLOSED and the
 * figure is a real distance for the first time.  PIN-FREE and DEVICE-FREE
 * (tools/shimcount.py prints no rows).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/20082f0.c \
 *     asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_a.s --func OvlFunc_949_20082f0
 *
 * Piece: asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_a.s holds TWO functions,
 * OvlFunc_949_20082f0 and OvlFunc_949_20083d0, BOTH parked and both now at
 * 4 of 91 on the same body with constants substituted.  tools/datacheck.py
 * reports no data residue.  Written into one TU, objcmp --whole reads 4 and 4
 * with nothing new, so neither twin's residue is a TU effect.
 *
 * ==== THE MISSING TWO INSTRUCTIONS, AND THE PASS THAT REMOVES THEM ====
 *
 * The ROM puts the message id straight into r0 and branches each arm to ONE
 * shared `bl __MessageID`:
 *      cmp r0,#0 / beq L1 / ldr r0,=<id> / b L2        ... L2: bl __MessageID
 * The park's body hoisted the id load ABOVE the compare, inverted the branch
 * and emptied the arm, which costs an extra `mov r0, r3` once and saves a `b`
 * per arm -- net -2:
 *      bl __GetFlag / ldr r3,=<id> / cmp r0,#0 / bne L8 ... L8: mov r0,r3 / bl __MessageID
 * r3 rather than r0 is a CONSEQUENCE, not the cause: once the load precedes
 * `cmp r0,#0` the value cannot live in r0.
 *
 * THE PASS IS IF-CONVERSION, located in the dumps rather than guessed.  Insn 64
 * (the 0x2365 load) sits AFTER jump_insn 61 in every dump through
 * `.13.combine` and BEFORE it in `.14.ce`, with jump 61's condition flipped
 * from `eq` to `ne`.  That is gcc-2.96's if_convert (toplev.c:3238) taking
 * find_if_case_1 -> dead_or_predicable (ifcvt.c:1651, :1831) with reversep=1.
 *
 * WHY NO FLAG REACHES IT: there is no -fno-if-conversion in gcc-2.96 (cc1
 * rejects it), and if_convert is called unconditionally under `optimize > 0`.
 * MEASURED, every one still 66 / 68 on the park body: -fno-gcse,
 * -fno-schedule-insns, -fno-schedule-insns2, -fno-cse-follow-jumps,
 * -fno-cse-skip-blocks, -fno-thread-jumps.
 *
 * ==== THE FIX: ONE VARIABLE FOR THE FLAG RESULT AND THE ID ====
 *
 * dead_or_predicable refuses when `MERGE_SET & (TEST_SET | TEST_LIVE)` is
 * non-empty (ifcvt.c:1988-1996) -- i.e. when a register SET IN THE ARM is
 * already LIVE at the condition's earliest insn.  Writing the GetFlag result
 * into the SAME local that the arm then overwrites puts the id's pseudo in both
 * sets, the conversion is refused, the arm keeps its load, and the value lands
 * in r0 because that is where __GetFlag returns it:
 *
 *      id = __GetFlag(0x950);
 *      if (id) id = 0x2365; else { id = __GetFlag(0x962); ... }
 *
 * No pin, no device.  A `register int id __asm__("r0")` reaches the identical
 * figure through a DIFFERENT clause of the same function -- SMALL_REGISTER_CLASSES
 * is TARGET_THUMB (arm.h:1061), so a hard register in MERGE_SET fails at
 * ifcvt.c:1962-1971 -- and is not needed.  (`__asm__("r3")` is byte-identical to
 * the park, so this is not simply "pin the register the ROM uses".)
 * The arm-size gate is NOT the route: count_bb_insns ignores jumps
 * (ifcvt.c:111-128) and BRANCH_COST is 1, so the one-insn arm always passes
 * ifcvt.c:1683.
 *
 * ==== WHAT THE 4 ARE, AND WHAT DOES NOT REACH THEM ====
 *
 *      rom   ldrh r2,[r6] / ldr r3,=0x2 / orr r3,r2 / mov r2,#0xbf / lsl r2,#1 / strh r3,[r6]
 *      ours  ldrh r3,[r6] / ldr r2,=0x2 / orr r3,r2 / mov r2,#0xbf / strh r3,[r6] / lsl r2,#1
 *
 * `orr r3, r2` is identical on both sides; what differs is which input was
 * loaded into the destination -- the ROM's dest holds the POOLED 2, ours holds
 * the loaded halfword -- and the `lsl`/`strh` order follows from it.  The park's
 * warning that this is the same signature as 200a5c0.c in this directory
 * SURVIVES re-measurement on the new body.  Thumb's `orr` is the two-operand
 * destructive pattern, so source operand order cannot reach it, exactly as
 * docs/elevation.md's "the commutative-operand boundary is the Thumb pattern's
 * arity" says.
 *
 * MEASURED INERT, all at 4 of 91: `*f = 2 | *f`, `*f = *f | 2`,
 * `*f = (unsigned short)(2 | (int)*f)`, `*f = 1 & *f`, a `short`
 * accumulator pre-loaded with 2 (`t = 2; t |= *f; *f = t;`), an
 * `unsigned short` temp for the load, swapping the `saved`/`f` definition
 * order, and moving `f`'s definition to just before its first use.
 * MEASURED WORSE: an `unsigned short` or `int` accumulator and a named
 * `unsigned short two = 2` (all 216 bytes -- the 2 stops being pooled, and the
 * ROM pools it); a named `int k = 0xbf << 1` at the top (70 / 68) or just
 * before its use (66 / 68); `q` defined late (62 / 68); `-ffixed-r7` (71 / 68);
 * `-O1` (216 bytes, pool 20 against 21); `-fno-schedule-insns2` (13 of 91);
 * and all four `do { } while (0);` placements around the OR (6, 7, 8, 9).
 *
 * *** THE PARK'S -O1 HYPOTHESIS IS REFUTED. *** Its whole argument was that
 * this TU is -O1 because only -O1 made the line counts agree.  On this body the
 * lengths agree AT -O2, and -O1 is strictly worse (216 bytes, one pool word
 * short).  autosweep on the old body already read -O1 at 68 of 91 against a
 * base of 41.  No Makefile rule is wanted here and none is proposed.
 *
 * THE OTHER PRESERVED CLAIM THAT DOES NOT SURVIVE: the note's "first difference
 * is argument precompute ... calls.c:805, not fixable from C" was measured at
 * -O1.  At -O2 on this body the prologue is EXACT, `ldr r3,=0x3001ebc /
 * mov r0,#0x10 / ldr r5,[r3] / bl __MapActor_GetActor` included.
 */
extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_949_20082f0(void)
{
    char *q;
    char *p;
    unsigned short *f;
    int saved;
    int id;

    q = iwram_3001ebc;
    p = __MapActor_GetActor(0x10);
    saved = *(short *)(p + 6);
    f = (unsigned short *)(p + 0x64);
    __CutsceneStart();
    *f |= 2;
    if (*(short *)(q + (0xbf << 1)) == 0) {
        id = __GetFlag(0x95 << 4);
        if (id)
            id = 0x2365;
        else {
            id = __GetFlag(0x962);
            if (id)
                id = 0x21e2;
            else
                id = 0x1f95;
        }
    } else {
        id = __GetFlag(0x95 << 4);
        if (id)
            id = 0x2371;
        else {
            id = __GetFlag(0x962);
            if (id)
                id = 0x21f5;
            else
                id = 0x1faa;
        }
    }
    __MessageID(id);
    __MapActor_SetAnim(0x10, 0);
    __Func_8092848(0x10, 0, 2);
    __Func_8093040(0x10, 0, 0xa);
    *(short *)(p + 6) = saved;
    __WaitFrames(1);
    *f &= 1;
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_949_20082f0 -- 0x020082f0, asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_a.s
 * Twin of OvlFunc_949_20083d0 -- 0x020083d0, SAME .s file.
 *
 * A clean twin family (neither member previously parked).  They differ only in
 * constants -- 0x10 against 0x11 and six message ids each one higher -- so one
 * solution elevates both.  Candidate: scratch/L82f0.c.
 *
 * THIS TU IS ALMOST CERTAINLY -O1, AND THAT IS THE FINDING.
 *
 *      default (-O2)   81 lines against the ROM's 86, 62 differing
 *      --O1            86 against 86, FIFTEEN differing
 *
 * The length agreeing only at O1 is the signal: the ROM's five extra
 * instructions are work that -O2 optimises away here, not something a source
 * spelling can add back.
 *
 * CORRECTION.  An earlier version of this note claimed the Makefile already
 * carries an O1 rule for a sibling in this directory.  It does not -- the one
 * explicit rule for rom_7d4af4 (ovl_30_c_c_a_c_c_c_c_c_c_b) uses CSE_CFLAGS.
 * That claim was asserted from memory and is withdrawn.
 *
 * What the Makefile actually shows, measured: O1 is applied at OVERLAY scale,
 * not per file -- 720 O1 rules live in just 13 directories, in blocks of 30 to
 * 180.  Ten of those 13 directories also carry a handful of single-file
 * exceptions in another group, so per-file deviation is normal.  But rom_7d4af4
 * is not an O1 directory at all; its only rule is CSE.  A lone O1 file here
 * would run against the grain, so the line-count evidence below has to carry
 * the hypothesis ON ITS OWN rather than being corroborated by neighbours.
 *
 * WHY -O2 IS FIVE SHORT, since it explains the shape rather than just the count:
 * the six message-id arms each end by assigning one id, and the ROM loads each
 * id straight into r0 and branches to a single `bl __MessageID`.  At -O2 gcc
 * hoists the pool load ABOVE the `cmp r0, #0` that tests the __GetFlag result,
 * so r0 is still live and the merged value has to live in r3 instead, changing
 * the branch structure.  At -O1 the hoist does not happen.
 *
 * CONFIRMED ON THE WAY (worth keeping even though the function is parked):
 * at -O2 the compound `*f |= 2;` reproduces the ROM's POOLED constant
 * (`ldr r3, =0x2`) exactly as docs/elevation.md's HImode-literal rule predicts.
 * At -O1 the same source gives `mov r2, #0x2`, so that rule is -O2-specific --
 * which the doc does not currently say.
 *
 * REMAINING AT -O1, 15 differing: adjacency swaps in the prologue (the iwram
 * load against the GetActor argument, and `mov r8, r2` against the r6 setup)
 * and the register roles on the halfword OR.  TRIED, all 16 -- i.e. slightly
 * worse: swapping the source order of the `saved` and `f` assignments,
 * `*f = 2 | *f`, and an int intermediate for the OR.
 *
 * The OR role swap is the same signature as 200a5c0.c in this directory, which
 * was parked this round after seven spellings.  Do not re-spend a round on it.
 *
 * LATER: the -O1 hypothesis above is CONFIRMED as far as it goes -- 86 against
 * 86 with 15 differing, reproduced -- and at least one of the remaining 15 is
 * now identified and is NOT reachable from C.
 *
 * The first difference is argument precompute:
 *
 *     rom    ldr r3, =0x3001ebc / mov r0, #0x10 / ldr r5, [r3] / bl GetActor
 *     ours   ldr r3, =0x3001ebc / ldr r5, [r3] / mov r0, #0x10 / bl GetActor
 *
 * The cheap `mov r0, #0x10` is the call's argument; the memory load is
 * expensive work beside it. gcc emits the expensive work first and lets the
 * cheap constant land last, which is exactly HANDOFF.md's "Argument
 * precompute: DIAGNOSED" section -- calls.c:805, not fixable from C.
 *
 * Swapping the two source statements so the call comes first makes it WORSE,
 * 18 differing: gcc then does the whole call before touching iwram_3001ebc,
 * where the ROM reads it before the call. The 15-differing form above is the
 * better one and is kept.
 *
 * SO THE -O1 QUESTION IS NOT SETTLED BY CLOSING THIS FUNCTION, and it cannot
 * be. Some of the residue is a documented compiler difference that would
 * remain at any optimisation level. Adding an O1 rule for this TU on the
 * strength of a line count that still leaves 15 differing -- against the grain
 * of a directory whose only explicit rule is CSE -- is not justified, and none
 * was added. The line-count evidence stands where the note left it.
 *)

*/
#endif
