/* Func_8094154 -- 0x08094154, asm/rom_8a000/rom_93304_a_c_c_c_c_c.s
 *
 * STILL NON-MATCHING, **8 of 63 STREAM encodings** (was 16).  MEASURED batch
 * 340 brief E.  Instruction counts 57 / 57 -- IN PHASE, so the 8 IS A TRUE
 * DISTANCE.  Size exact (136 bytes), relocations IDENTICAL (objcmp prints no
 * RELOCATIONS line), pool order exact.  First differing index 19.
 *   objcmp verbatim:
 *     XX ENCODINGS differ in 8 place(s) (ref 63, ours 63)
 *        [STREAM length, pool+pad INCLUDED; instructions are 57/57]
 *        first at index 19: ref 1a9b  ours 1a9a
 *
 * FIGURE HISTORY: 19 -> 18 -> 16 (batch 323) -> 16 (batch 330, refined but
 * unmoved) -> **8** (batch 340).  PINS: 0 (tools/shimcount.py reports no
 * rows).  DEVICES: 0 -- no __asm__, no do{}while(0) anchor, no .equ, no
 * fictitious symbol.  Default flags.
 *
 * NOTE ON THE 63: it is the STREAM count (pool + pad included).  The function
 * is 57 16-bit instructions plus three 32-bit pool words.  The previous header
 * said "COUNT EXACT (ref 63, ours 63)" on the instruction line; the count that
 * is exact is 57.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8094154.c \
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_c.s --func Func_8094154
 *
 * No split yet: one function out of a THREE-function reference
 * (tools/piece.py -- Func_8093e28 48 of 168, Func_8093fa0 140 of 192,
 * Func_8094154 8 of 63), so --whole does not apply and this ships on a cut
 * when it closes.  (This function also appears in
 * asm/rom_8a000/rom_92950_c_a_c_a_c_a.s.)
 *
 * Converts a field actor's world position to screen coordinates: subtract the
 * camera origin at iwram_3001e70 + 0xe4 with its low 16 bits masked off,
 * divide both axes by 0x10000, and write the pair through the caller's
 * pointer.  Actors whose kind nibble at +0x54 is 1 get their y nudged by the
 * sprite's signed byte at +8.  Returns -1 if the actor does not exist.
 *
 * ======================================================================
 * WHAT BATCH 340 CHANGED, AND WHY THREE BATCHES HAD MISSED IT
 * ======================================================================
 *
 * The park's own closing instruction was right and its reason was wrong.  It
 * said: *"NEXT: stop spelling.  The question is what makes insn 44 (a->f10)
 * stop sharing a hard register with insn 35 ... i.e. a liveness change, not a
 * syntax change."*  **A liveness change IS a syntax change here** -- splitting
 * ONE statement in two, and then reusing ONE variable across the halves, moved
 * 16 to 8.  What the park had actually closed was the *expression* axis
 * (re-spelling the mask, the parentheses, the casts); it had not tried
 * **cutting the dy statement at its inner subtraction**.
 *
 * THE TWO EDITS, in the order they paid:
 *
 * 1. **SPLIT dy AT THE INNER SUBTRACTION AND PUT dx BETWEEN THE HALVES.**
 *        t = a->f10 - (cam[1] & 0xffff0000);
 *        dx = a->f8 - (c0 & 0xffff0000);
 *        dy = t - a->fc;
 *    16 -> 13 with the anchor below, **11 without it**.  This closes the WHOLE
 *    dx chain at once -- `ldr r1,[r4,#8]`, `sub r1,r0`, `cmp r1,#0`,
 *    `ldr r3,=0xffff`, `add r1,r3` and the three-operand `asr r3,r1,#0x10` all
 *    become the reference's, register for register.  The park's residue table
 *    attributed those to the sched2 priority ladder; they were a chain-LENGTH
 *    artifact.  With dy written as one three-term expression its chain is two
 *    subs deep against dx's one, so dy outranks dx at every step and sched2
 *    drains dy first; cut in two, the two chains tie and interleave, which is
 *    what the reference does.
 *
 * 2. **REUSE ONE VARIABLE FOR THE MASK PRODUCT AND THE PARTIAL.**
 *        t = cam[1] & 0xffff0000;
 *        t = a->f10 - t;
 *    11 -> **8**.  This is the whole remaining lever and it is a REG_ALLOC_ORDER
 *    effect: arm.h:989 is `{3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, ...}`, so r3
 *    is handed out FIRST and r2 second.  The reference has cam[1]&mask in r2
 *    and a->f10 in r3; every form with the mask product in its own fresh local
 *    gets them the other way round and reads 11.
 *
 * NOTHING ELSE MATTERS, which is itself the finding: at 8 the body is a
 * PLATEAU across 44 further crossings (rounds 5-7, below).
 *
 * ======================================================================
 * WHAT THE REMAINING 8 IS -- ONE QUANTITY, TWO BLOCKS
 * ======================================================================
 *
 *   (i) 2 encodings, in the dy arithmetic:
 *         ref    sub r3, r2          ldr r2, [r4, #0xc]
 *         ours   sub r2, r3, r2      ldr r3, [r4, #0xc]
 *       The reference writes the partial back over **a->f10's** register
 *       (two-operand in-place) and then loads a->fc into the mask product's
 *       freed r2.  We write the partial over the **mask product's** register
 *       (three-operand) and load a->fc into f10's freed r3.
 *
 *  (ii) 6 encodings, in the dy DIVISION tail -- a different block:
 *         ref    mov r3,r6 | cmp r3,#0 | ldr r2,=0xffff | add r3,r2 | asr r3,#0x10
 *         ours   mov r2,r6 | cmp r2,#0 | ldr r3,=0xffff | add r2,r3 | asr r3,r2,#0x10
 *       The dividend copy out of r6 takes r3 in the reference and r2 in ours,
 *       and the 0xffff bias takes the other.  The dx tail is IDENTICAL
 *       (`cmp r1,#0 / ldr r3,=0xffff / add r1,r3 / asr r3,r1,#0x10`), so only
 *       the dy side is inverted.  *** WHETHER (ii) IS INDEPENDENT OF (i) OR A
 *       PSEUDO-NUMBERING KNOCK-ON OF IT IS NOT MEASURED.  Eleven probes aimed
 *       at (ii) alone (the quotient through its own local, divided in place,
 *       `dy /= 0x10000`, both quotients through locals, the dy store first)
 *       all left the figure at exactly 8 or made it worse, which is WEAK
 *       EVIDENCE for a knock-on and not a proof. ***
 *
 * THE TENSION, stated so the next agent does not re-derive it:
 *   - the mask product must be born FIRST for the two LOADS to get the
 *     reference's registers (8), and
 *   - the partial must NOT be the same quantity as the mask product for the
 *     SUBTRACTION to get the reference's register,
 *   but the only form that satisfies the first is the one-variable reuse,
 *   which forces the second to fail.  Every separation measured 11:
 *     `t = cam1m; f = a->f10 - t;`                    11
 *     `t = cam1m; f = a->f10; f -= t;`                11
 *     `t = cam1m; f = a->f10; f = f - t;`             11
 *     `f = a->f10; t = cam1m; f -= t;`                11
 *     ... plus `t` reused for a->fc as well, `f` declared first, the fc read
 *     named, `~0xffff`, `*(cam+1)`, c0 masked at the def: 11 each.
 *   >> SO THE NEXT MOVE IS local-alloc, NOT SPELLING.  Read `block_alloc`'s
 *   >> quantity ordering (local-alloc.c) and `qty_compare` on THIS body and
 *   >> say why the two-quantity form inverts the load pair; the bound is
 *   >> whether a third quantity can be introduced that takes r3 first without
 *   >> costing an instruction.
 *
 * ======================================================================
 * MEASURED NEGATIVES ADDED THIS BATCH (all at count 57/57 unless flagged)
 * ======================================================================
 *
 * AUTOSWEEP on the PARKED body (tools/autosweep.py, run first):
 *   FLAG AXIS: -fno-gcse 16, -ffixed-r7 16, -fno-strict-aliasing 16,
 *     -fno-strength-reduce 16, -fno-rerun-loop-opt 16 (inert);
 *     -fno-schedule-insns2 22, -fno-rerun-cse-after-loop 48, -O1 60 (worse).
 *   ANCHOR AXIS: 13 boundaries; **anchor03 (before `c0 = cam[0];`) 16 -> 15**,
 *     the other twelve inert.
 *   That 15 is a DEAD END and worth naming as one: it closes exactly the
 *   cam[1]-load position, and once the dy split above is in place THE SAME
 *   ANCHOR COSTS 2 (11 -> 13).  A full anchor re-sweep on the 11-body is
 *   closed: boundaries 0-2 and 10-13 inert at 11, 3/4/6 -> 13, 5 -> 15,
 *   7 -> 17, 8/9 -> 18.
 * FLAG AXIS RE-CROSSED ON THE 8-BODY (OBJCMP_EXTRA, 12 flags): -fno-gcse,
 *   -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce,
 *   -fno-rerun-loop-opt, -fno-schedule-insns, -fno-cse-follow-jumps all
 *   exactly 8; -fno-force-mem 19; -fno-schedule-insns2 21; -O1 and
 *   -fno-expensive-optimizations 56 instructions (COUNT, bound nothing);
 *   -fno-rerun-cse-after-loop 58 instructions (COUNT).  FLAG AXIS CLOSED.
 * WORSE, on the 11-body or the 8-body: `f = a->f10;` named before `c0` 19;
 *   `t = a->f10; t -= cam1m;` with c0 after 19; dx written before the dy split
 *   19; dx written after dy 16; the dy store before the dx store 19;
 *   `dy` itself carrying the partial 60 and -4 bytes (RELOCDIFF);
 *   `t = cam[1]; t &= 0xffff0000;` 60 and -4 (RELOCDIFF); naming a->f8 55 and
 *   -4 (RELOCDIFF); the pointer advance before the arithmetic 41 and -4
 *   (RELOCDIFF); dy computed after the advance 44 and -4 (RELOCDIFF).
 * CORRECTED FROM THE PARK: the park's `t = a->f10 - cam1m; dy = t - fc;` row is
 *   listed as "MEASURED EXACTLY INERT ... crossed with c0".  On the parked body
 *   it IS inert -- because the park wrote the dx statement BEFORE it.  The
 *   whole lever is dx sitting BETWEEN the two halves.  The park's row and this
 *   landing differ by one statement's position.
 *
 * ======================================================================
 * CARRIED FORWARD FROM BATCHES 323 AND 330 -- STILL LOAD-BEARING
 * ======================================================================
 *
 * 1. THE ROM ADVANCES A POINTER, IT DOES NOT INDEX.  `mov r2, r5 / add r5, #4`
 *    is two pointer variables -- `q = out; out = out + 1;` -- with the first
 *    store through q and the second through the advanced out.  `out[0]`/`out[1]`
 *    gives 52 and `*out++` gives 48.  PREREQUISITE; do not tidy.
 * 2. NAME ONE OF THE TWO CAMERA READS, NOT BOTH.  `c0 = cam[0];` is worth 2 and
 *    is still a prerequisite (dropping it on the new body reads 18).  Naming
 *    BOTH costs a callee-saved register and reads 61 at 61 instructions; naming
 *    cam[1] only reads 18.  The bank's lever-5 DIRECTIONALITY, direction pinned.
 * 3. The bias-add of 0xffff under `bge` before `asr #16` is the division tell --
 *    the source wrote `/ 0x10000`; a `>> 16` omits it.
 * 4. REFERENCE-FILE CORRECTION (batch 323): an earlier header's first line named
 *    ..._c_c_c_c_a.s while its own recipe named ..._c_c_c_c_c.s.  The recipe is
 *    right; `_a` holds Func_8093af8.
 * 5. STALE, AND NOW SUPERSEDED: batch 330's sched2 priority ladder for the
 *    cam[0]/cam[1] issue order (prio 8 against 9, the anti-dependence reading of
 *    `priority`, the ready-list trace at t=5 and t=7) was a correct description
 *    of the PARKED body and is no longer the residue -- the three camera/mask
 *    loads now match the reference exactly.  Its closing claim, *"Re-spelling the
 *    expressions cannot move a ladder whose rungs are register anti
 *    dependences"*, was true of re-spelling and false of re-STATEMENTING.
 */

struct Sub {
    unsigned char pad00[0x28];
    short *f28;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Sub *f50;
    unsigned char pad54[0];
    unsigned char f54;
};

struct Info {
    unsigned char pad00[8];
    signed char f8;
};

extern unsigned char *iwram_3001e70;
extern struct Actor *GetFieldActor(int id);
extern struct Info *_GetSpriteInfo(int id);

int Func_8094154(int id, int *out)
{
    struct Actor *a;
    int *cam;
    int dx, dy;
    int c0;
    int *q;
    int t;

    a = GetFieldActor(id);
    if (a == 0)
        return -1;
    cam = (int *)(iwram_3001e70 + 0xe4);
    c0 = cam[0];
    t = cam[1] & 0xffff0000;
    t = a->f10 - t;
    dx = a->f8 - (c0 & 0xffff0000);
    dy = t - a->fc;
    q = out;
    out = out + 1;
    *q = dx / 0x10000;
    *out = dy / 0x10000;
    if ((a->f54 & 0xf) == 1)
        *out -= _GetSpriteInfo(*a->f50->f28)->f8;
    return 0;
}
