/* ===== COORDINATOR CORRECTION, batch 350: THE FLAG GROUP ALREADY EXISTS =====
 * This header says a `STRENGTH_CFLAGS` rule would be a new per-file flag group
 * and therefore an owner decision.  IT IS NOT.  `STRENGTH_CFLAGS` is defined at
 * Makefile:855 (`$(GCC296_CFLAGS) -fno-strength-reduce`) and ALREADY HAS TWO
 * CONSUMERS, at Makefile:744 and :857.  So adding a third explicit rule is the
 * established practice this tree uses for every other per-file flag -- the same
 * shape as the CSE_CFLAGS rule added in batch 346 -- and NOT a new precedent.
 *
 * WHAT THAT CHANGES.  The 221-of-259 body described below is at a TRUE DISTANCE
 * (259 encodings, 588 bytes, both sides equal) where every prior pass on this
 * function was two instructions long.  It is reachable, and the only thing
 * standing between it and a landing is the remaining 221 -- not a policy
 * question.  When it reaches 0, the install is: the body, plus a four-line
 * explicit rule naming STRENGTH_CFLAGS with its reasoning, exactly like the
 * seven other flag rules already in the Makefile.
 *
 * The crossing that produces it is worth restating because neither half works
 * alone: `-fno-strength-reduce` gives 6 `mul`s, the scale constants through
 * locals give 6, and TOGETHER they give the ROM's 8.  The park's verdict that
 * the flag "does NOT restore the multiply, so a STRENGTH_CFLAGS rule is not the
 * answer on its own" was right about ON ITS OWN and wrong about the conclusion.
 */
/* Field_Catch -- 0x0809ad70.  PARK, 237 of 259 encodings differ (ours 261).
 * NOT a true distance at production flags: 261 encodings / 592 bytes against
 * 259 / 588, TWO INSTRUCTIONS AND FOUR BYTES LONG.
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * ref: asm/rom_8a000/rom_9ad70_a_c.s -- ONE function (grep -ci func_start = 1),
 * tools/datacheck.py reports NO data section, so this converts WHOLE with NO
 * split and no exports.  The ref's leading comment is STALE: it names the
 * function RunFieldAbility with a "~500-instruction body"; it is Field_Catch
 * at 259.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/809ad70.c \
 *     asm/rom_8a000/rom_9ad70_a_c.s --whole
 *
 * batch 350, brief E.  PIN-FREE, DEVICE-FREE, ANCHOR-FREE, PRODUCTION FLAGS
 * (tools/shimcount.py reports nothing).
 *
 * FOUR NUMBERS, re-derived this pass (not inherited):
 *   encodings  ref 259   ours 261     (+2)
 *   bytes      ref 588   ours 592     (+4)
 *   relocations  30 / 30, SAME SYMBOLS NOW (see below), offsets shifted
 *   aligncmp   aligned-equal 173 (66.8%), differing/ins/del 116 in 64 hunks
 *              -- the prior park body was 169 / 120 in 66 hunks
 *
 * =============== WHAT CHANGED THIS PASS: THREE THINGS ===============
 *
 * 1. THE BRIEF'S MECHANISM TRANSFERRED.  Both loop exits now carry the ROM's
 *    encodings.  Batch 350 sent this park the Field_Halt switch-case-run
 *    mechanism (landed body: src/rom_8a000/rom_9a44c_c_c_c_b.c) on the strength
 *    of two hand-written BACKWARD `cmp r7, #0xb / blt` sites in this reference,
 *    at line 164 (-> .L9af38, line 116) and line 266 (-> .L9b01e, line 219).
 *    Both verified backward before writing anything, so the shape fits.
 *
 *    The prior body spelled both exits `} while (i < 0xb);` and emitted
 *        cmp r7, #10 / ble      (both loops)
 *    because combine.c:10140-10145 rewrites `LT C` to `LE (C-1)` unconditionally
 *    for positive C, with the reverse LE->LT direction at :10152-10158 guarded by
 *    `const_op < 0`.  So no literal comparison can ever reach `cmp #11 / blt`.
 *    Written instead as the last statement of a `for (;;)` body with NO
 *    `default:` arm,
 *        switch (i) { case 11 ... 0x7fffffff: goto doneA; }
 *    stmt.c:6346-6369's pure-leaf arm of emit_case_nodes calls
 *    emit_cmp_and_jump_insns DIRECTLY, never through fold; node_has_high_bound
 *    (stmt.c:6004-6042) is true because node->high == TYPE_MAX_VALUE, which
 *    SUPPRESSES the GT test; and the absent `default:` puts default_label at the
 *    end of the switch == the end of the loop body == the back edge.  Result, in
 *    this body, measured:
 *        cmp r7, #11 / blt .L6      (loop A)
 *        cmp r7, #11 / blt .L23     (loop B)
 *    Four encodings recovered; aligncmp 120 -> 116 differing, which is exactly
 *    the four.  `... 0x7fffffff` is load-bearing (an enum cannot substitute:
 *    c_expand_start_case runs default_conversion, which converts ENUMERAL_TYPE
 *    to int).
 *
 *    AND THE NAMED BOUND IS THE SAME PROGRAM HERE.  `n = 0xb;` with
 *    `} while (i < n);` on both loops is BIT-IDENTICAL (object fingerprint, not
 *    .s text) to the case run.  Field_Halt's rule explains why both work here
 *    and only the case run works there: the bound reaches `cmp` as an immediate
 *    only when the bound pseudo gets no hard register, which needs r11 already
 *    taken by another call-crossing allocno (find_reg takes r11 whenever free --
 *    global.c:1010-1050, r11 last in REG_ALLOC_ORDER at arm.h:989-995).  This
 *    function's r11 is occupied; Field_Halt's is free.  EITHER SPELLING IS
 *    CORRECT HERE -- the case run is shipped only because it needs no extra
 *    local.
 *
 * 2. A REAL BUG FIXED: THE SECOND LOOP'S SCALE WAS UNSIGNED.  The prior body
 *    wrote `t = i * 0xffff4000 / 10 + ...`.  0xffff4000 is 4294918144, which
 *    exceeds INT_MAX, so its type is `unsigned int`; the multiply and the
 *    division were therefore unsigned and the body called __udivsi3 where the
 *    ROM calls __divsi3 (visible as a RELOCATION difference at ref offset
 *    0x20a, which objcmp's encoding count cannot see because the `bl`
 *    placeholder is identical).  Shipped as `i * -0xc000`, which restores
 *    __divsi3.  `(int)0xffff4000` is bit-identical; `-0xc000` is shipped for
 *    legibility.  THE PARK'S OWN RELOCATION LIST CONTAINED THIS DEFECT AND IT
 *    HAD GONE UNREAD FOR THREE PASSES -- read objcmp's reloc rows, not only its
 *    count.
 *
 * 3. THE PARK'S STRENGTH-REDUCTION VERDICT IS REFUTED, BY A CROSSING.
 *    The park said: "-fno-strength-reduce shortens the loop but does NOT restore
 *    the multiply, so a STRENGTH_CFLAGS rule is not the answer on its own."  The
 *    first clause is right and the conclusion is wrong -- it is not the answer
 *    ON ITS OWN, and it IS the answer crossed with one source edit.
 *
 *    The ROM emits 8 `mul`s (4 per loop: three interpolations plus the scale).
 *    The park body emits 6: loop.c strength-reduces each scale multiply into an
 *    accumulator.  Measured `mul` counts:
 *        park body                                    6
 *        park body            + -fno-strength-reduce  6
 *        case run only        + -fno-strength-reduce  6
 *        SCALE CONSTANTS THROUGH LOCALS (`sa`/`sb`)   6
 *        scale locals         + -fno-strength-reduce  8   <- the ROM's count
 *    Neither half moves it alone; together they do.  The scale-through-a-local
 *    edit is Field_Douse's lever 4 (src/non_matching/rom_8a000/Field_Douse.c),
 *    which this park had never tried -- Field_Douse needs it because 0xc000 is
 *    3 << 14 and expand_mult synthesises a shift-and-add instead of a `mul`.
 *
 *    AND THAT CROSSING MAKES THE LENGTH RIGHT, which no prior pass achieved:
 *        scale locals + case run + -fno-strength-reduce
 *            222 of 259, ours 259 ENCODINGS / 588 BYTES -- A TRUE DISTANCE
 *        the same, plus master address pointers (see below)
 *            221 of 259, 259 / 588, aligncmp 172 / 115 differing in 61 hunks
 *    This is the first body for this function whose instruction count and byte
 *    size both equal the reference, so objcmp's count is meaningful on it for
 *    the first time.
 *
 *    IT IS NOT SHIPPED, and the reason is policy, not measurement.  No
 *    STRENGTH_CFLAGS group exists in the Makefile (it carries ALIAS_CFLAGS:235,
 *    GCSE_CFLAGS:334, SCHED2_CFLAGS:469, FIXEDR7_CFLAGS:487, O1_CFLAGS:190,
 *    COMMON2_CFLAGS:167), and a new per-file flag group is an owner-facing
 *    decision that a brief does not get to take -- so the manifest carries
 *    "flag_group": null and this paragraph instead.  At production flags the
 *    scale locals COST an instruction without restoring the `mul` (237 -> 246,
 *    aligncmp 116 either way), so they are correctly absent from the shipped
 *    body; they belong with the flag or not at all.
 *    DECISION WANTED: add STRENGTH_CFLAGS, and this park goes from "2 long, not
 *    a distance" to "a true distance at 221".
 *
 * ============== WHAT THE REMAINING RESIDUE IS -- ONE THING ==============
 * The park's named blocker REPRODUCED exactly and is retained.  Read off
 * aligncmp -v on the true-distance body, the first hunks are the whole story:
 *     ref   sub sp, #0x28   /  str r1, [sp, #0]   /  add r2, sp, #16 ; mov r9, r2
 *     ours  sub sp, #0x24   /  (no spill)         /  str r3, [sp, #0] ...
 * The ROM has EIGHT long-lived pseudos -- r5 state, r6 particle, r7 entity then
 * the counter, r9/r11 the two vec3 addresses, r8/r10 the per-loop from/to copies,
 * plus the target-actor pointer -- so the eighth spills; a spill slot is
 * allocated last and FRAME_GROWS_DOWNWARD (arm.h:1297) puts it at offset 0,
 * which pushes all three vec3s up by four to 4 / 0x10 / 0x1c and forces every
 * vec3 store to go through an address pseudo.  This body has only seven live, so
 * nothing spills, the frame is 4 bytes smaller, and the stores are direct
 * `[sp, #N]`.  That single decision is the bulk of the residue.
 *
 * MEASURED THIS PASS AND CLOSED BY BIT-IDENTITY -- the vec3 addressing axis is
 * NOT where the lever is.  Six pointer spellings on the true-distance body:
 *     master pointers declared early + per-loop copies        221
 *     the same, with the straight-line FILLS through them     221  bit-identical
 *     ASYMMETRIC: master for `b` only, array name for `v`     221  bit-identical
 *     ASYMMETRIC: master for `v` only, array name for `b`     222  BIT-IDENTICAL
 *                                                                  TO BASE
 *     `tgt` read later, to lengthen its range (route 5)       222  BIT-IDENTICAL
 *                                                                  TO BASE
 *     three masters, `c` through a pointer too                222
 * SO SIX RECORDED SPELLINGS ARE THREE DISTINCT PROGRAMS, and the three that
 * differ from the base differ by one encoding.  gcse's cprop_insn coalesces
 * every master/copy pair straight back, which is exactly what Field_Douse's
 * header predicts for both-pointers and neither-pointer spellings -- and this
 * pass extends that to the ASYMMETRIC spelling, which Field_Douse records as
 * untried and which is hereby measured INERT at the fills.  If the asymmetry is
 * a lever anywhere it must be placed INSIDE the loop, not at the fills.
 *
 * NEGATIVE-LIST ACCOUNTING (batch 349's method).  Across this pass 19 variants
 * were swept in two groups; 6 were flagged BIT-IDENTICAL to the base or to each
 * other, so roughly a third of the rows measured nothing -- in line with the
 * 62-87% seen elsewhere.  The two decisive moves were both CROSSINGS of rows
 * that are inert alone: (scale locals x -fno-strength-reduce) for the length,
 * and (case run x signedness fix) for the shipped body.
 *
 * RETAINED FROM THE PARK, re-confirmed and not re-measured: declaration order
 * `c, v, b` (lever 7 / FRAME_GROWS_DOWNWARD); `i * (q[0] - p[0])` and NOT the
 * reverse (the reversed spelling is a catastrophic regression because it also
 * changes which operand the strength reducer picks); the pooled `.word 0` with
 * its mid-function `.pool` reproducing from a plain `a->f55 = 0`.
 */
typedef struct {
    int x;
    int y;
    int z;
} vec3;

struct Ent {
    unsigned char pad00[6];
    short f06;
    int f08;
    int f0c;
    int f10;
};

struct Actor {
    unsigned char pad00[6];
    short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    unsigned char pad34[0x55 - 0x34];
    unsigned char f55;
};

struct FState {
    int f00;
    int f04;
    int f08;
    int f0c;
    struct Ent *f10;
    struct Actor *f14;
    unsigned char pad18[8];
    signed char f20;
};

extern struct FState *iwram_3001f30;

extern void vec3_translate(int dist, int angle, int *v);
extern struct Actor *CreateParticleActor(int id, int x, int y, int z);
extern void Func_8097384(void);
extern void Func_809748c(void);
extern void _PlaySound(int id);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_SetColorswap(struct Actor *a, int n);
extern void _Actor_SetPos(struct Actor *a, int x, int z);
extern void _DeleteActor(struct Actor *a);
extern void WaitFrames(int n);

void Field_Catch(void)
{
    struct FState *s;
    struct Ent *e;
    struct Actor *a;
    struct Actor *tgt;
    vec3 c;
    vec3 v;
    vec3 b;
    int *p;
    int *q;
    int i;
    int t;

    s = iwram_3001f30;
    tgt = s->f14;
    e = s->f10;
    v.x = e->f08;
    v.y = e->f0c + (0x80 << 13);
    v.z = e->f10;
    if (s->f20 != 0) {
        b.x = e->f08;
        b.y = e->f0c + (0x80 << 14);
        b.z = e->f10;
        vec3_translate(0x80 << 14, s->f00, &b.x);
    } else {
        b.x = s->f04;
        b.y = s->f08 + (0x80 << 14);
        b.z = s->f0c;
    }
    c.x = s->f04;
    c.y = s->f08 + (0x80 << 14);
    c.z = s->f0c;
    a = CreateParticleActor(0xd7, c.x, c.y, c.z);
    if (a == 0)
        return;
    Func_8097384();
    _PlaySound(0x8a);
    a->f06 = e->f06;
    a->f30 = 0x14ccc;
    a->f55 = 0;
    _Actor_SetAnim(a, 5);
    _Actor_SetColorswap(a, 1);
    i = 0;
    p = &v.x;
    q = &b.x;
    for (;;) {
        a->f08 = p[0] + i * (q[0] - p[0]) / 10;
        a->f0c = p[1] + i * (q[1] - p[1]) / 10;
        a->f10 = p[2] + i * (q[2] - p[2]) / 10;
        t = i * (0xc0 << 8) / 10 + (0x80 << 7);
        a->f18 = t;
        a->f1c = t;
        i++;
        WaitFrames(1);
        switch (i) {
        case 11 ... 0x7fffffff:
            goto doneA;
        }
    }
doneA:
    ;
    WaitFrames(0xa);
    _Actor_SetAnim(a, 6);
    WaitFrames(0xf);
    i = 9;
    do {
        a->f0c += 0xfffe0000;
        WaitFrames(1);
        i--;
    } while (i >= 0);
    _Actor_SetAnim(a, 5);
    _PlaySound(0x84);
    if (tgt != 0)
        _Actor_SetPos(tgt, 0xfff70000, tgt->f0c);
    WaitFrames(0x14);
    i = 0xc;
    do {
        a->f0c += 0xc0 << 9;
        WaitFrames(1);
        i--;
    } while (i >= 0);
    WaitFrames(0xa);
    _PlaySound(0x72);
    i = 0;
    p = &b.x;
    q = &v.x;
    for (;;) {
        a->f08 = p[0] + i * (q[0] - p[0]) / 10;
        a->f0c = p[1] + i * (q[1] - p[1]) / 10;
        a->f10 = p[2] + i * (q[2] - p[2]) / 10;
        t = i * -0xc000 / 10 + (0x80 << 9);
        a->f18 = t;
        a->f1c = t;
        i++;
        WaitFrames(1);
        switch (i) {
        case 11 ... 0x7fffffff:
            goto doneB;
        }
    }
doneB:
    ;
    _DeleteActor(a);
    Func_809748c();
}
