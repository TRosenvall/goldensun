/* OvlFunc_896_200a27c -- park. The subject line must stay FIRST: tools that identify a
 * park by its subject (piece.py, autosweep.py, shortrefs.py, scratchfill.py)
 * read line 1, and a note prepended above it makes them resolve the wrong
 * function or no function at all. Notes go UNDER this line. Added in batch
 * 341 after the pre-fix autosweep was found sweeping wrong subjects.
 */
/* ===== BATCH 341: FIGURE RE-DERIVED, PIN LADDER MEASURED, CLASS NAMED ========
 *
 * RE-DERIVED with tools/objcmp.py:
 *     XX ENCODINGS differ in 17 place(s) (ref 160, ours 160)
 *        [STREAM length, pool+pad INCLUDED; instructions are 125/125]
 *        first at index 61: ref 4651  ours 4653
 * BOTH INSTRUCTION COUNTS ARE 125, size is silent and RELOCATIONS ARE IDENTICAL
 * (29 of 29, same symbols at the same 29 offsets), so 17 is a TRUE DISTANCE.
 * NOTE the units: this header elsewhere says "160 instructions" -- 160 is the
 * STREAM encoding count including pool and padding; the instruction count is 125.
 *
 * THE PIN-FREE FIGURE IS 44 OF 160, and unlike most pin-free figures it IS a
 * distance: dsize 0, instructions 125/125.  Its relocation banner is BENIGN --
 * the same 29 symbols in the same order, only the offsets shifted (0x3e against
 * 0x3c and so on), which is instruction movement and not a wrong program.
 * So the five pins buy 44 -> 17.
 *
 * THE PIN LADDER, each PIN3 block dropped ONE AT A TIME (base 17 of 160):
 *     drop P0 (SetPos 0x5)   32 of 160   first 22   RELOCDIFF(offsets)
 *     drop P1 (SetPos 0x9)   20 of 160   first 34   rel ok
 *     drop P2 (SetPos 0xb)   31 of 160   first 33   RELOCDIFF(offsets)
 *     drop P3 (SetPos 0xa)   20 of 160   first 44   rel ok
 *     drop P4 (SetPos 0xe)   20 of 160   first 50   rel ok
 *     all five dropped       44 of 160   first 22   RELOCDIFF(offsets)
 * EVERY ONE OF THE FIVE IS LOAD-BEARING -- there is no inert pin to shed here,
 * which is the opposite of the sibling park src/non_matching/ovl_7f2f14/2009af0.c
 * where batch 341 shed three of fifteen for free.  This confirms the header's own
 * drop-table in objcmp units rather than `aligned` ones.  (The "P5" that the
 * header drops as inert scaffolding is the SIXTH SetPos site, 0xd, which already
 * ships unpinned -- it is not one of the five above.)
 *
 * THE CLASS, AND IT IS SHARED.  This park's pins are an ARGUMENT-WINDOW class,
 * not an allocation class, and it is the class batches 339/340 named:
 * precompute_register_parameters (calls.c:805-854) copies every argument whose
 * rtx_cost(value, SET) > 2 -- i.e. >= 256 in Thumb (arm.c:2077-2084) -- into a
 * pseudo BEFORE load_register_parameters writes any hard argument register, so
 * the two expensive arguments (`0x1db0000`, `0xa6 << 17`, ...) are hoisted and
 * THE CHEAP `mov r0, #<slot>` IS LEFT WITH THE HIGHEST LUID.  sched2's last
 * tie-break is INSN_LUID (haifa-sched.c:4110-4113), so it emits last.  The pin is
 * what denies the hoist.
 *
 * NAMING THE SLOT IS INERT, MEASURED, AND THE REASON IS cse.c:509-514 -- COST is
 * 1 for a pseudo against rtx_cost*2 for the constant, so cse1 folds a sub-256
 * constant straight back into the argument.  Two spellings, both 44 of 160, i.e.
 * EXACTLY the bare-literal pin-free figure:
 *     int t0..t4, each assigned immediately before its own call      44
 *     int t0..t4, all five assigned at the top of the function       44
 * AND THE REMATERIALISATION ESCAPE IS NOT AVAILABLE TO THIS PARK AT ALL: the
 * local-alloc.c:884-889 arm needs REG_BASIC_BLOCK (regno) < 0, i.e. the set and
 * its single use in DIFFERENT basic blocks, and THIS FUNCTION IS BRANCH-FREE --
 * one straight-line block from `__MapActor_GetActor(0x0)` to `__CutsceneEnd()`,
 * with no `if`, no loop and no `?:` anywhere.  A call does not end a basic block
 * here, so no boundary exists to put the set on the other side of.  See the
 * sibling src/non_matching/ovl_7e7574/200cda0.c, where the boundary DOES exist
 * and the escape still does not fire.
 *
 * AUTOSWEPT, BOTH CHEAP AXES CLOSED (tools/autosweep.py, batch 341).  Flag axis,
 * unchanged body: -fno-gcse / -ffixed-r7 / -fno-strict-aliasing /
 * -fno-strength-reduce / -fno-rerun-cse-after-loop / -fno-rerun-loop-opt all 17;
 * -O1 and -fno-schedule-insns2 both 41.  Anchor axis: all 40 `do { } while (0);`
 * placements measured, all 17 or worse.  Do not re-screen either axis.
 *
 * VERDICT: HOLDS at 17 of 160 with 15 pins.  The pin-free figure of 44 is
 * evidence about the blocker, not a shippable body, and by the standing pin
 * policy this is pass-3 depin material.
 */
/* PARKED -- OvlFunc_896_200a27c -- 0x0200a27c
 *
 * NON-MATCHING, 17 of 160 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_78ef88/200a27c.c \
 *     asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c_a.s --func OvlFunc_896_200a27c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c.s, the SECOND of the
 *    three functions left in that .s when _c_c_b.s was cut out for
 *    OvlFunc_896_200978c]
 *
 * 160 instructions of cutscene setup -- six behaviour changes, six position
 * sets, and seven near-identical actor blocks that each store a parent pointer,
 * set a flag bit and attach one script.  BEST RESULT, against BOTH a scratch
 * copy of the reference and the REAL asm/ path, each re-run to confirm:
 *
 *   XX ENCODINGS differ in 17 place(s) (ref 160, ours 160)
 *      first at index 61: ref 4651  ours 4653
 *
 * SIZE IS SILENT -- 388 bytes both -- and RELOCATIONS ARE SILENT: all 29 are
 * identical, in the same 29 places.  Same result under `-fno-gcse`,
 * `-fno-rerun-cse-after-loop`, `-fno-cse-follow-jumps`,
 * `-fno-expensive-optimizations`, `-fno-strength-reduce`, `-fno-strict-aliasing`
 * and `-ffixed-r7`; `-fno-schedule-insns2` is 47.  No flag group reaches it.
 *
 * ############################################################################
 * ## THE ENTIRE RESIDUE IS ONE QUESTION ASKED SEVEN TIMES: WHICH LOW         ##
 * ## REGISTER A HIGH-REGISTER-TO-LOW COPY LANDS IN.  Not one instruction is  ##
 * ## missing, extra, or in the wrong place -- only r1 where the ROM has r3.  ##
 * ############################################################################
 *
 *   ref   mov r1, sl / str r1, [r0, #104]      blocks 0x5, 0x9, 0xa
 *   ours  mov r3, sl / str r3, [r0, #104]
 *   ref   mov r3, sl / str r3, [r0, #104]      blocks 0xb, 0xe, 0xd -- MATCHES
 *
 *   ref   movs r1, #0 / ... / mov r9, r1       the zero into r9
 *   ours  movs r3, #0 / ... / mov r9, r3
 *   ref   mov r1, r9 / ... / str r1, [r5, #12] the zero back out of r9
 *   ours  mov r3, r9 / ... / str r3, [r5, #12]
 *
 *   ref   ldrb r3,[r2] / orrs r6, r3 / strb r6, [r2]   the LAST of the seven
 *   ours  ldrb r3,[r2] / orrs r3, r6 / strb r3, [r2]   ORs
 *
 * IT IS NOT RELOAD, WHICH IS WHERE THE ROUND-ROBIN LIVES.  `allocate_reload_reg`
 * (reload1.c:4965) advances a static `last_spill_reg` round-robin between
 * insns, which would explain an alternating r1/r3 pattern exactly -- and it is
 * the wrong answer: `-da` shows this function performs ZERO reloads.  The
 * copies are ordinary insns and the register is chosen by local-alloc, whose
 * `REG_ALLOC_ORDER` (arm.h:989) starts `{3, 2, 1, 0, ...}`, so r3 is the first
 * choice and our uniform r3 is exactly what "nothing conflicts" produces.
 *
 * THE LEVER IS A LIVE-RANGE OVERLAP, AND IT IS MEASURED.  Splitting the
 * read-modify-write so the loaded byte is live ACROSS the parent-pointer store
 *
 *     w = b[0x5a];
 *     *(unsigned char **) (b + 0x68) = p;
 *     b[0x5a] = w | one;
 *
 * makes the copy pseudo conflict with both the address (r2) and the byte (r3)
 * and it takes r1 -- in EVERY block.  Applied to blocks 0x5, 0x9 and 0xa only,
 * it reproduces the ROM's r1,r1,r3,r1,r3,r3 pattern EXACTLY and takes the zero
 * defects with it: 14 aligned differing, down from 18, with only the six moved
 * stores and the last `orr` left.  So the mechanism is identified and
 * demonstrated.
 *
 * IT IS NOT A FIX.  The split reorders the emitted code -- the ROM stores the
 * parent pointer BEFORE the `ldrb` in all seven blocks, and the split emits it
 * after -- so the six instructions it fixes are paid for with six it breaks.
 * And a per-block difference in a run of seven identical blocks is not source
 * anybody wrote.  The uniform spelling (18) ships in preference to the
 * per-block one (14).
 *
 * WHAT WAS TRIED FOR THE OVERLAP WITHOUT THE REORDER, and none of it works:
 * hoisting only the ADDRESS (`f = b + 0x5a;` before the store, `*f |= one;`
 * after) is inert; so is hoisting the address AND splitting the RMW after the
 * store.  The conflict has to be with the LOADED BYTE, and the byte cannot be
 * loaded before the store without moving the store.
 *
 * INERT, ALL OF IT (each ties the 18 exactly, so none of it is the answer):
 * `1` as a literal instead of a named `one`; `p` as `int`, `void *` or
 * `unsigned char *`; the 0x68 store as a pointer store or an `int` store; one
 * actor variable or five; the script pointer as `unsigned char *` or `void *`;
 * `b[0x5a] |= one` written longhand or split through a temp; and ALL TEN
 * declaration orders of the six locals.  Declaration order is completely inert
 * here, which is worth knowing against docs/elevation.md §"POINTER BIRTH ORDER
 * decides which register each pointer gets" -- that lever is about which
 * pseudo gets which register when they COMPETE, and here they do not.
 *
 * THE LAST `orr` IS THE SAME PHENOMENON, NOT THE SIBLING'S LEVER.
 * `_c_c_b.c` needed `t = 1; t |= x; x = t;` to put the CONSTANT in the
 * destination.  Here six of the seven ORs want the loaded byte as destination
 * -- which is what plain `x |= one` gives -- and only the SEVENTH wants the
 * constant's register, because `one` is dead there and regmove is free to
 * swap the commutative operands.  Writing that one site as `one |= x; x = one;`
 * does not buy it: 166 instructions, SIX LONG, 25 aligned, because a modified
 * `one` is live again.  Writing all seven that way is 30 aligned.
 *
 * WHAT IS CONFIRMED, by the 143 matching encodings.
 *
 * THE PROLOGUE, BY CONTENT.  `push {r5, r6, lr}` plus `mov r6,sl / mov r5,r9 /
 * push {r5,r6}` plus `mov r6,r8 / push {r6}`: FIVE values live across calls --
 * r10 the parent actor, r8 the script address, r9 a zero, r6 the constant 1,
 * r5 the one actor kept past its block.  No r4 (-fcall-used-r4), and NO r7 at
 * all.
 *
 * SIX CANDIDATE ORDERING PINS, FIVE OF WHICH SHIP.  The six `__MapActor_SetPos`
 * sites carry two repeated pooled constants (`0xa6 << 17` twice, `0xae << 17`
 * twice); unpinned, gcc commons each into a call-saved register and reaches it
 * with `adds r2, r5, #0` where the ROM rebuilds `movs r2,#0xa6 / lsls r2,#17`
 * every time.  This is docs/elevation.md §"The commoned-constant tell has TWO
 * remedies" -- and it is the THIRD remedy, the call-clobbered pin, that takes:
 * neither `-fno-rerun-cse-after-loop` nor separate named locals move it.
 *
 * The pins are NOT independently droppable and the two repeated constants are
 * not the whole story.  Dropping each of the six from the full set:
 *
 *   drop P5   18  INERT -- its pooled constant is used ONCE, nothing commons it
 *   drop P4   20  REQUIRED
 *   drop P3   22  REQUIRED
 *   drop P1   22  REQUIRED
 *   drop P0   26  REQUIRED
 *   drop P2   27  REQUIRED
 *
 * P4 has NO repeated constant either and is still required, for the `mov r0`
 * INTERLEAVE (`mov r1,#0xe6 / mov r2,#0xb4 / mov r0,#0xe / lsl r1 / lsl r2`),
 * which is the ordering half of the pin's job rather than the CSE half.
 * P5 is dropped as inert scaffolding; five pins ship.
 *
 * PARTIAL PINS ARE WORSE, and monotonically: pinning r0+r1+r2 is 18, r0+r2 is
 * 36, r1+r2 is 36, r2 alone is 42, r0+r1 is 33, r0 alone is 43, r1 alone is 40,
 * none is 40.  UNIFORM WHOLE-VALUE ASCENDING FILL IS CORRECT -- one ascending
 * statement per site reproduces all six of the ROM's emitted orders with no
 * per-site tuning, the descending fill is 42, and respelling `a << k` as its
 * whole value is byte-identical.
 *
 * MEASURED WORSE (one change at a time against the five-pin candidate):
 *
 *   | change                                       | insns | aligned |
 *   |----------------------------------------------|-------|---------|
 *   | no pins at all                               |  160  |  40     |
 *   | descending fill at all six SetPos sites      |  160  |  42     |
 *   | the script pointer assigned at the top       |  160  |  51     |
 *   | `z = 0` at the top of the function           |  160  |  24     |
 *   | `z = 0` moved next to its use                |  154  |  29     |
 *   | the literal `0` instead of the r9 zero       |  154  |  29     |
 *   | `z = 0;` before `s = gScript...;`            |  166  |  60     |
 *   | the OR before the 0x68 store, all blocks     |  160  |  37     |
 *   | `unsigned char one` instead of `int one`     |  164  |  62     |
 *   | `one \|= x; x = one;` at the last OR only     |  166  |  25     |
 *   | `one \|= x; x = one;` at all seven            |  160  |  30     |
 *   | `z = 0` in the 2nd/3rd/4th block             |  160  |  18 *   |
 *   |----------------------------------------------|-------|---------|
 *   | the RMW split around the store, blocks 0/1/3 |  160  |  14     |
 *
 *   * same aligned count but the relocations start differing, so the
 *     relocation line separates them where the aligned count cannot -- the
 *     triage rule doing its job on a pair the metric calls equal.
 *
 * FOR THE LANDING: this function does not match, so it stays in asm/.  It sits
 * BETWEEN OvlFunc_896_2009d04 (also parked) and OvlFunc_896_200a400 (exact),
 * which is why landing 200a400 splits the .s after 200a27c rather than around
 * it -- see the header of the 200a400 candidate.
 *
 * Harness: scratch_elev/b243/f2009d04/{sweep,sdiff}.py plus a27c/gen.py.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern unsigned char gScript_896__0200cbd0[];


void OvlFunc_896_200a27c(void)
{
    unsigned char *p;
    unsigned char *a;
    unsigned char *b;
    unsigned char *s;
    int one;
    int z;

    p = __MapActor_GetActor(0x0);
    __CutsceneStart();
    __MapActor_SetBehavior(0x5, 0x1);
    __MapActor_SetBehavior(0x9, 0x1);
    __MapActor_SetBehavior(0xb, 0x1);
    __MapActor_SetBehavior(0xa, 0x1);
    __MapActor_SetBehavior(0xe, 0x1);
    __MapActor_SetBehavior(0xd, 0x1);
    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0x5; q1 = 0x1db0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0x9; q1 = 0x1eb0000; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0xb; q1 = 0x1cb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0xa; q1 = 0x1fb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0xe; q1 = 0xe6 << 17; q2 = 0xb4 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0xd, 0x1d70000, 0x99 << 17);

    b = __MapActor_GetActor(0x5);
    *(unsigned char **) (b + 0x68) = p;
    one = 0x1;
    b[0x5a] |= one;
    s = gScript_896__0200cbd0;
    z = 0x0;
    __Actor_SetScript(b, s);

    b = __MapActor_GetActor(0x9);
    *(unsigned char **) (b + 0x68) = p;
    b[0x5a] |= one;
    __Actor_SetScript(b, s);

    b = __MapActor_GetActor(0xb);
    *(unsigned char **) (b + 0x68) = p;
    b[0x5a] |= one;
    __Actor_SetScript(b, s);

    b = __MapActor_GetActor(0xa);
    *(unsigned char **) (b + 0x68) = p;
    b[0x5a] |= one;
    __Actor_SetScript(b, s);

    a = __MapActor_GetActor(0xe);
    *(unsigned char **) (a + 0x68) = p;
    a[0x5a] |= one;
    *(int *) (a + 0x18) = 0x80 << 9;
    *(int *) (a + 0x1c) = 0x80 << 9;
    a[0x55] = __MapActor_GetActor(0xb)[0x55];
    *(int *) (a + 0xc) = z;
    __Actor_SetScript(a, s);

    b = __MapActor_GetActor(0xd);
    *(unsigned char **) (b + 0x68) = p;
    b[0x5a] |= one;
    __Actor_SetScript(b, s);
    __CutsceneEnd();
}
