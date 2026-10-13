/* OvlFunc_common1_78 -- NON-MATCHING, 56 encodings of 90 differ (objcmp).
 *
 * ref 75 instructions / 208 bytes / 90 stream entries
 * ours 74 instructions / 200 bytes / 87 stream entries   -- gap -1 INSTRUCTION
 * aligned-equal 67 of 90 (74.4%).  BATCH 345 BRIEF B: this park had NO FIGURE
 * at all; it now has one, and it is 56.  The body it replaces measured
 * 69 of 90 with aligned-equal 45 of 90 (50.0%).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/common1_78.c \
 *     asm/overlays/common/common1_a_a_a_a_a_c_a_b.s --func OvlFunc_common1_78
 *
 * LANDING SHAPE: NO SPLIT.  asm/overlays/common/common1_a_a_a_a_a_c_a_b.s holds
 * ONE function and no data; datacheck.py reports nothing.
 * PIN-FREE, DEVICE-FREE: no register pins, no .equ shims, no asm of any kind.
 *
 * ============ THE PARK'S STATED BLOCKER IS REFUTED ============
 *
 * The old header said:
 *     "BLOCKER: gcc reuses the value it just stored; the ROM re-loads it ...
 *      The intervening `strb` at +0x131 does not break the CSE for gcc, and it
 *      would not for any alias analysis either -- the offsets are provably
 *      distinct.  So this is NOT an aliasing problem to be defeated."
 *
 * It is exactly an aliasing problem, and the lever is the OFFSET, not the zero.
 * The old body named the offset in a local (`o2 = 0x131;`).  cse then
 * constant-propagates it, the address becomes `u + 0x131` with a known
 * displacement, the offsets ARE provably distinct, and the store-to-load
 * forwarding survives -- which is the `lsl #16 / asr #16` pair the park saw.
 * Writing the literal offset instead leaves it as a POOLED constant in a
 * register, so the store address is `(plus (reg u) (reg 0x131))` with an opaque
 * displacement, the forwarding dies, and BOTH clamp blocks re-read with
 * `mov rX, #0x38 / ldrsh`, as the ROM does.  Measured: ref[33:47] -- the whole
 * first clamp including the two ldrsh pairs -- goes from 0 matching to EXACTLY
 * matching.  Same change also buys the ROM's `adds r3, r5, r1 / strb r2, [r3]`
 * (immediate-offset store through a computed pointer) in place of our
 * register-offset `strb r3, [r5, r2]`: +1 instruction, which is one of the two
 * the park was short.  Probe: scratch_elev/b345/B/work/probe_hi.c, c1 vs c3/c5.
 *
 * The park's "SOLVED and worth keeping" note had this backwards -- it credited
 * the POOLED ZERO to "writing the literal 0 directly into a byte store".  That
 * is false against any body here: a literal 0 in a byte store gives
 * `mov r3, #0` (measured, probe c1-c5, five spellings).  See below.
 *
 * ============ TWO MORE LEVERS THAT PAID, WITH THEIR MEASUREMENTS ============
 *
 * 1. `w` MUST BE THE REGISTER THAT HOLDS THE CLAMP CONSTANT.  The ROM is
 *        mov r3,#0x80 / lsl r3,#7 / cmp r0,r3 / bgt L / mov r3,#0 /
 *        cmp r0,#0 / blt L / mov r3,r0 / L: strh r3,[r5,#0x14]
 *    -- nine instructions with NO copy, because the `> LIMIT` arm's assignment
 *    `w = LIMIT` is a no-op: w already lives in the comparison constant's
 *    register.  The park's if / else-if / else chain makes cse unify the two
 *    `0x80 << 7` occurrences into one pseudo (r3) and then copies it into w
 *    (r2) -- `adds r2, r3, #0`, the park's extra instruction.  Spelling the
 *    clamp as "seed w with the limit, then narrow" removes the copy:
 *        w = 0x80 << 7;
 *        if (q <= w) { w = 0; if (q >= 0) w = q; }
 *    This is NOT a priority lever and no allocation probe reaches it; it is
 *    route 1 of the five (`global.c:600-618`) -- the two values become ONE
 *    variable, so the contest the park was reasoning about does not exist.
 *    Same shape as batch 344's Func_80a355c.  69 -> 59 crossed with the
 *    offset lever; alone it takes aligned 45 -> 38, i.e. WORSE, which is why
 *    it had to be crossed and is not findable one-at-a-time.
 *
 * 2. A LITERAL 1 IN A HALFWORD STORE IS POOLED; ROUTE IT THROUGH THE int.
 *    `*(short *)(u + 0x14) = 1;`        -> ldrh r3, .L3 + `.word 1`  (POOLED)
 *    `w = 1; *(short *)(u + 0x14) = w;` -> mov r3, #1               (the ROM)
 *    `*thumb_movhi_insn` has no immediate alternative, so a HImode constant is
 *    force_reg'd through the minipool (arm.md:4353, pool_range 64); routing it
 *    through an int makes it `*thumb_movsi_insn` alternative "I"
 *    (arm.md:3838-3856) and gcc builds it with an 8-bit `mov`.  The old body
 *    paid TWO pool words (`.word 0x00000001` twice, two separate dumps) and
 *    4 of its +4 bytes for this.  Measured four spellings: probe_hi.c b1-b5;
 *    `int w = 1`, `w = 1` on a parameter and `short s = 1` all give the `mov`,
 *    both literal forms (signed and unsigned short) pool.
 *
 * ============ WHAT IS LEFT: ONE INSTRUCTION, AND IT IS NAMED ============
 *
 * The residue is now 56 of 90 and FOUR THINGS, in descending size:
 *
 *   (a) THE MID-FUNCTION LITERAL POOL AND THE `b` OVER IT.  ref[47:52] is
 *         b.n 0x80 / .short 0x0000 / .word 0 / .word gState / .word 0x131
 *       dumped at 0x74, immediately after the first clamp block.  Ours puts a
 *       2-word pool at the END and has no `b`.  That `b` IS the missing
 *       instruction and the pad IS the missing 2 bytes: gcc's
 *       `create_fix_barrier` emits both when it dumps a pool mid-function
 *       (batch 323's mechanism), and nothing else in this function can
 *       account for a +1/+2.
 *
 *   (b) THE POOLED ZERO, ref[49] `.word 0x00000000` at 0x74, loaded at 0x4a by
 *       ref[29] `4a0a` = `ldr r2, [pc, #40]`.  `4a0a` is unambiguously a WORD
 *       literal load (Thumb has no PC-relative ldrh/ldrb), so the constant is
 *       SImode.  gcc-2.96 CANNOT POOL A LITERAL 0 IN ANY MODE WHOSE POOL LOAD
 *       IS `ldr`: `*thumb_movsi_insn` alternative 1 is constraint "I"
 *       (arm.md:3840) -> `mov %0, %1`, and `*thumb_movqi_insn` alternative 5 is
 *       also "I" (arm.md:4625) -> `mov %0, %1`.  Five literal spellings
 *       measured (probe_hi.c c1-c5) and all five emit `mov r3, #0`.
 *       const.sym's own header states the rule this is an instance of: "gcc-2.96
 *       never pools a constant it can build with an eight-bit `mov`.  So a
 *       pooled small value in the ROM says the source named a SYMBOL."  The
 *       halfword exception that header warns about DOES NOT APPLY: the
 *       destination here is a BYTE store (`strb`), not a halfword, so there is
 *       no HImode operand to pool against.
 *       => THIS PARK NEEDS A `_CONST_0 = 0x0;` ENTRY IN const.sym.  It meets
 *          both of that file's stated criteria, measured, not asserted.
 *          INSTRUMENTED AND MEASURED: with `extern int _CONST_0;` and
 *          `(unsigned char)(int)&_CONST_0` as the stored value the pool-word
 *          count becomes EXACT (14 of 14 32-bit entries against the old 13),
 *          size 200 -> 204, figure and aligned residue both unchanged at
 *          56 / 67-of-90.  Body kept at scratch_elev/b345/B/work/p1_g.c.
 *          NOT SHIPPED: a new const.sym entry is an owner decision and the
 *          symbol would add an R_ARM_ABS32 the reference does not carry (the
 *          established, documented behaviour -- see
 *          src/non_matching/overlays/2008640.c:8-20 for the precedent).
 *
 *   (c) r1-vs-r2 and r2-vs-r3 permutations on the `mov rX, #offset / ldrsh`
 *       pairs (ref[14:18], ref[56:58], ref[62:66], ref[80:82]).  Downstream of
 *       (a): the ROM's pool sits between the two halves, so its temp is
 *       dead-and-reborn where ours is still live.
 *
 *   (d) THE WIRING ITEM THE PARK ALREADY FLAGGED, AND IT IS REAL: the reference
 *       calls `_divsi3_RAM`, we emit `__divsi3`, so objcmp prints
 *       `RELOCATIONS differ` on every measurement of this park.  This is common-
 *       overlay code; the per-overlay linker scripts carry
 *       `__divsi3 = _divsi3_RAM;` and common1 has never needed it.  THIS MUST BE
 *       WIRED BEFORE THIS FUNCTION CAN EVER LAND, independently of the figure.
 *
 * A LENGTH-EQUAL VARIANT EXISTS AND IS NOT AN IMPROVEMENT -- recorded so the
 * next agent does not re-derive it.  Leaving the FIRST `u+0x14` store as a
 * literal 1 pools a HImode constant whose pool_range is 64 (arm.md:4353), which
 * forces `create_fix_barrier` to dump mid-function and emit the `b`.  That
 * closes ALL THREE length gaps -- 75/75 instructions, 90/90 entries, 208/208
 * bytes -- at figure 56, the SAME figure as this body.  But the dump lands at
 * 0xa4, after the SECOND clamp, where the ROM's is at 0x74 after the first, so
 * aligned-equal FALLS 67 -> 51.  Body at scratch_elev/b345/B/work/p1_e.c.
 * It is the brief's step-3 case exactly: the lengths agreed and the program got
 * further away.  The pool must be forced at 0x74, which means the fix that
 * forces it must be the one loaded at 0x4a -- i.e. (b), not a later store.
 *
 * AUTOSWEPT (batch 345, --max 60).  FLAG AXIS CLOSED: all eight per-file groups
 * inert at 69 on the old body.  ANCHOR AXIS: 31 of 31 boundaries; the two rows
 * reading 68 both carry dsize=4 and are misalignment, not improvement.  4 of 31
 * variants produced no row -- that slice is UNTESTED, not inert.
 */
extern unsigned int gState;
extern void __Func_8079664(int n);
extern void __AddPartyMember(int n);
extern void __SetCameraTarget(int slot, int n);
extern unsigned char *__GetUnit(int slot);
extern void __Func_8091858(void);
void OvlFunc_common1_78(int slot)
{
    unsigned char *u;
    unsigned int g;
    unsigned int off;
    int q;
    int w;

    __Func_8079664(0);
    __Func_8079664(1);
    __Func_8079664(2);
    __Func_8079664(3);
    __Func_8079664(5);
    __AddPartyMember(slot);
    g = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    g += off;
    *(int *)g = slot;
    __SetCameraTarget(slot, 0);
    u = __GetUnit(slot);
    *(short *)(u + 0x38) = *(unsigned short *)(u + 0x34);
    *(short *)(u + 0x3a) = *(unsigned short *)(u + 0x36);
    *(unsigned char *)(u + 0x131) = 0;

    q = (*(short *)(u + 0x38) << 14) / *(short *)(u + 0x34);
    w = 0x80 << 7;
    if (q <= w) {
        w = 0;
        if (q >= 0)
            w = q;
    }
    *(short *)(u + 0x14) = w;
    if ((w << 16) == 0 && *(short *)(u + 0x38) != 0) {
        w = 1;
        *(short *)(u + 0x14) = w;
    }

    q = (*(short *)(u + 0x3a) << 14) / *(short *)(u + 0x36);
    w = 0x80 << 7;
    if (q <= w) {
        w = 0;
        if (q >= 0)
            w = q;
    }
    *(short *)(u + 0x16) = w;
    if ((w << 16) == 0 && *(short *)(u + 0x3a) != 0) {
        w = 1;
        *(short *)(u + 0x16) = w;
    }
    __Func_8091858();
}
