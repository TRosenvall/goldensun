/* OvlFunc_931_2008b2c -- 0x02008b2c,
 *   asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s (2 functions).
 *
 * NON-MATCHING, 53 of 91 encodings -- A FIGURE FOR THE FIRST TIME, and it is
 * still PARTLY PHASE.  MEASURED batch 344 brief E.  Pin-free, device-free.
 *   ref 70 16-bit instructions against ours 71: WE EMIT ONE EXTRA.
 *   ref 91 stream entries / 224 bytes against ours 93 / 228.
 *   Position-tolerant residue (tools/aligncmp.py): 21 in 15 hunks.
 *
 * The park this replaces had NO figure: objcmp stopped at the size check.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7b8cb0/2008b2c.c \
 *     asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s --func OvlFunc_931_2008b2c
 *
 * PIECE MEMBERSHIP, verified with tools/piece.py and NOT taken from a brief:
 *   asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s -- 2 functions
 *     PARKED  OvlFunc_931_2008904   4 of 221   src/non_matching/ovl_7b8cb0/2008904.c
 *     PARKED  OvlFunc_931_2008b2c   this file
 *   => all 2 parked, CONVERTS WHOLE: one candidate holding both lands the pair.
 *   The mate at 4 of 221 is the near one.  Neither is hostage to the other --
 *   tools/split_s.py cuts before/target/after, so EITHER cuts out alone.
 *
 * ================== THE NET +1 IS THREE REGIONS, NOT ONE ==================
 *
 * This is the case the batch brief warns about: count each region separately
 * before believing the net.  Here the net +1 is +1 +1 -1.
 *
 *   (a) CONSTANT HOIST, +1.  0x2c60000 is passed to two __MapActor_SetPos calls
 *       in the 0x241 arm.  The ROM reloads it from the pool at each site
 *       (`ldr r2,=0x2c60000` twice, BOTH resolving to the same pool word at
 *       +0xd4); gcc loads it once into r5 and copies (`ldr r5,.L9+16` then
 *       `mov r2,r5` twice).  3 instructions against 2.
 *   (b) THE SAVE OF r5, +1.  `pop {r5}` in the epilogue.  The `push` is not part
 *       of it: `push {lr}` and `push {r5, lr}` are one instruction each.
 *   (c) THE 0x242 ELSE ARM, **-1 -- WE ARE SHORTER THAN THE ROM HERE.**
 *         ref   mov r1, r0 / add r1, #0x59 / ldrb r2,[r1] ... strb r3,[r1]
 *         ours  add r0, #0x59 / ldrb r2,[r0] ... strb r3,[r0]
 *       The ROM materialises the field address in r1 and leaves r0 alone; we
 *       fold it destructively into r0.  The reference does the `mov r1, r0`
 *       form at ALL FOUR `interactFlags |= 4` sites and we already match the
 *       other three, because those read through the shared named pointer `a`.
 *
 * (c) IS SOLVED AND THE FIX IS MEASURED -- see scratch_elev/b344/E/p4_regionC_body.c.
 * Writing the else arm with the same named pointer,
 *
 *       } else {
 *           a = __MapActor_GetActor(0xf);
 *           a->interactFlags |= 4;
 *       }
 *
 * restores the ROM's two-instruction form: gap goes to +2 (we now have the
 * instruction the ROM has) and the POSITION-TOLERANT residue drops 21 -> 15.
 * That body is NOT shipped here only so the recorded figure does not read as a
 * regression -- its NDIFF rises to 75 of 91 BECAUSE the gap is +2, i.e. more
 * phase, not more distance.  WHOEVER TAKES THIS PARK NEXT SHOULD START FROM
 * THAT BODY: it isolates the whole remaining residue to mechanism (a)+(b).
 *
 * MEASURED, and note which lever needs which:
 *   park body, else arm inline                          gap +1  ALIGN 21  NDIFF 53
 *   else arm through the shared `a`                      gap +2  ALIGN 15  NDIFF 75  <= correct shape
 *   else arm through a SEPARATE pointer `b`              gap +1  ALIGN 21  NDIFF 53
 *   three distinct pointers a/b/c                        gap +1  ALIGN 21  NDIFF 53
 *   two named int locals for 0x2c60000                   gap +1  ALIGN 21  NDIFF 53
 *   ... crossed with the else-arm fix                    gap +2  ALIGN 15  NDIFF 75
 *   0x2c6 << 16 at one site, 0x2c60000 at the other      gap +2  ALIGN 15  NDIFF 75
 * The `b` and `a/b/c` rows are the mechanism's own proof: it is SHARING ONE
 * POINTER across the else arm and the tail that produces `mov r1, r0`.  A
 * distinct variable per site goes straight back to the destructive `add r0`.
 *
 * ============ (a) AND (b) ARE ONE DECISION, AND THE PASS IS cse1 ============
 *
 * Named from `-da`, not reasoned:
 *   .00.rtl  insn 82  (set (reg:SI 51) (const_int 46530560))
 *            insn 116 (set (reg:SI 62) (const_int 46530560))
 *   .03.cse  insn 116 IS GONE; insn 122 reads (set (reg:SI 2 r2) (reg:SI 51))
 *
 * reg 51 then spans two calls inside the arm and takes callee-saved r5, which
 * buys the `pop`.  This is the SAME blocker as
 * src/non_matching/ovl_77dd1c/200bc48.c in this batch, and that file's header
 * carries the four-step bound with line numbers: calls.c:849-856 forces the
 * pseudo, cse.c:919 is the only flush, cse.c:2018-2065 spares pseudos at a call,
 * and cse.c:5236-5251 against cse.c:740 and cse.c:509-513 makes the cost
 * comparison unwinnable (12 against 1).  Read it before attacking this.
 *
 * ONE THING HERE THAT 200bc48 DOES NOT HAVE, and it is the open question: this
 * function HAS control flow -- three `if`s -- and the ROM still reloads the
 * constant.  But both 0x2c60000 sites sit in ONE basic block (the 0x241 arm has
 * no branch between them), so the bound applies unchanged inside that arm.  The
 * ROM's two loads are therefore NOT explained by the four steps above, and that
 * is a live discrepancy, not a closed bound.  THE MOST PROMISING UNTESTED ROUTE:
 * local-alloc.c:821-822 promotes the REG_EQUAL note on a constant-valued pseudo
 * to REG_EQUIV and then DOUBLES its REG_LIVE_LENGTH ("decrease the priority of
 * that register"), which `allocno_compare` divides by (global.c:600-618).  If
 * the pseudo then loses the race for a callee-saved register, reload
 * REMATERIALISES it from `reg_equiv_constant` at each use -- which is exactly
 * `ldr r2,=0x2c60000` twice with nothing pushed.  That needs register PRESSURE
 * this arm does not obviously have (r5, r6, r7 are all free here, r4 being
 * call-used under -fcall-used-r4), so the question to answer next is what the
 * ROM's source had live across that arm.  Stated as a route with its citations,
 * NOT as a bound.
 *
 * THE OTHER REAL RESIDUE, same length and so invisible to the gap: SCHEDULING.
 * In the 0x90<<2 arm the ref emits `ldr r1,=0x31a0000 / mov r0,#9 /
 * ldr r2,=0x3390000` and we emit the two `ldr`s adjacent with `mov r0,#9` after;
 * in the 0x242 then-arm the ref has `mov r2,#0xba / mov r0,#0xf / ldr r1,= /
 * lsl r2,#18` against our `mov r2,#186 / ldr r1,. / lsl r2,#18 / mov r0,#15`.
 * Same instructions, r0's setup one or two slots apart, twice.  The pool WORD
 * ORDER is already exactly the ROM's (7 entries, same sequence), so this is
 * sched2's ready-list order and not a pool question.
 *
 * MEASURED NEGATIVES, so they are not re-screened:
 *   FLAG AXIS CLOSED on both bodies.  All eight Makefile groups plus thirteen
 *   probe flags hold the gap at +1 (base) and +2 (region-c body).
 *   **autosweep.py's "-fno-schedule-insns2 49 of 91 <- BETTER" IS A PHASE
 *   ARTIFACT**: it raises the position-tolerant residue from 21 to 23.  -O1 is
 *   much worse (ALIGN 36).  -fno-gcse is exactly inert, which matters because it
 *   confirms cse1 rather than gcse: the deletion is already visible in .03.cse.
 *   ANCHOR AXIS CLOSED: all 14 boundaries, best 52 at a +4-byte size.
 *   The park's original three rows stand and are correctly recorded as inert:
 *   CSE_CFLAGS 42, GCSE 42, two named locals 42 (its own numbering).  Its
 *   "THIRD COUNTEREXAMPLE TO THE TWO-REMEDY RULE" conclusion survives.
 *
 * WHAT THE PARK GOT RIGHT: the constant-reuse diagnosis, the identification of
 * 0x2c60000 as the constant, the `push {r5}` as its cost, pool.py's reuse=1
 * prediction, and that consuming the GetActor result inline helps where no null
 * test is needed.  WHAT IT GOT WRONG: "That single decision is the remaining 42
 * positions and the one extra line."  It is not a single decision -- region (c)
 * is an independent, opposite-signed mechanism worth one instruction, and it is
 * the one that was actually solvable.
 */
struct Actor {
    unsigned char pad0[6];
    unsigned short facing;
    unsigned char pad1[0x51];
    unsigned char interactFlags;
};

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_931_2008b2c(void)
{
    struct Actor *a;

    if (__GetFlag(0x90 << 2) == 0) {
        __MapActor_SetPos(8, 0xca << 18, 0x2d70000);
        __MapActor_GetActor(8)->facing = 0xc0 << 6;
        __MapActor_SetPos(9, 0x31a0000, 0x3390000);
    }
    if (__GetFlag(0x241) == 0) {
        __MapActor_SetPos(0xa, 0x8c << 18, 0x2c60000);
        __MapActor_GetActor(0xa)->facing = 0x80 << 5;
        __MapActor_SetPos(0xb, 0x90 << 18, 0x2c60000);
    }
    if (__GetFlag(0x242) == 0) {
        __MapActor_SetPos(0xf, 0x1270000, 0xba << 18);
        __MapActor_GetActor(0xf)->facing = 0xb0 << 8;
    } else {
        __MapActor_GetActor(0xf)->interactFlags |= 4;
    }
    a = __MapActor_GetActor(0x11);
    if (a != 0)
        a->interactFlags |= 4;
    a = __MapActor_GetActor(0x10);
    if (a != 0)
        a->interactFlags |= 4;
}
