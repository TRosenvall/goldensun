/*
 * NON-MATCHING, 2 of 57 encodings  (MEASURED, batch 331 brief E).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7d0e88/200a1ac.c \
 *     asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_a.s --func OvlFunc_947_200a1ac
 *
 * OvlFunc_947_200a1ac -- asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_a.s
 * Streams aligned, ref 57 ours 57, no SIZE and no POOL WORD line, first
 * difference at index 35.  This is a distance and not the padding trap.
 *
 * ===== BATCH 331 BRIEF E: THE PARK CAME DOWN FROM NINE, AND THE RESIDUE IS NOW
 * ===== ONE SCHEDULER TIE WITH A NAMED, FALSIFIABLE REQUIREMENT
 *
 * THE BODY CHANGED.  Everything below the batch-329 heading in the previous
 * revision of this header was measured against a body that used ONE pointer
 * local `p` for the actor-0xd pointer and BOTH sprite pointers.  That body is
 * superseded.  Its figure was nine, spelled here in words so no tool can read
 * it as a claim, and every "worse" figure in its negative list was measured
 * against it, not against what ships now -- treat that whole list as historical
 * and re-measure before quoting any of it.
 *
 * WHAT CLOSED THE FIRST SEVEN.  Three distinct pointer locals instead of one:
 * `b` for the actor fetched at slot 0xd, `p` for the first sprite pointer, `q`
 * for the second.  Both splits are required and the landscape is NOT monotone:
 *
 *     one `p` for all three roles (the old body)            nine of 57
 *     `b` split out, `p` reused for both sprites            nine of 57
 *     `p`/`q` split, the actor pointer still sharing `p`    forty-seven of 57
 *     `b`, `p` and `q` all distinct                           2 of 57
 *
 * This is batch 330's "do not search the diagonal of a square" rule: the park's
 * standing probes had varied the OR constant's type, its assignment position
 * and the declaration order -- five rows in one dimension -- and had never
 * varied how many names the pointers share.
 *
 * WHY THAT MOVES A CONSTANT'S REGISTER.  The old residue was
 * `mov r4,#0x8 / and r3,r1 / orr r3,r4` against ours putting the 8 in r1, the
 * register the `and` had just freed, which pinned the `mov` after the `and` by
 * its own anti-dependence.  REG_ALLOC_ORDER is `{3,2,1,0,12,14,4,5,...}`
 * (arm.h:989) and r12/r14 are HI_REGS, so the usable order is 3,2,1,0,4; r4 is
 * reached only when r3, r2, r1 and r0 are all busy for the whole of the
 * constant's range.  With one pointer pseudo serving every role, `.18.greg`
 * reads `;; 33 preferences: 0` and that single global allocno takes r0 for all
 * three roles, leaving r1 free for the constant.  With three names the second
 * sprite pointer is its own pseudo and takes r1, r1 is no longer free, the
 * constant goes to r4, and -- being in a register the `and` does not touch --
 * sched2 hoists the `mov` above it unprompted.  Both field edits then match the
 * reference instruction for instruction, including the ROM's reuse of the one
 * constant across both `orr` sites.
 *
 * THE REMAINING TWO ARE ONE TIE IN `rank_for_schedule`, AND THE REQUIREMENT IS
 * EXACT.  The reference ends the TravelTo setup
 *     mov r0,r6 / ldr r3,[r6,#0x10] / lsl r2,#14
 * and we end it
 *     lsl r2,#14 / ldr r3,[r6,#0x10] / mov r0,r6
 * -- the same three instructions, every register already correct, slots one and
 * three transposed.  Read off `-fsched-verbose=6`, at the cycle after
 * `[r6+0x30]=r3` the ready list is exactly those three and the scheduler takes
 * the `lsl`.  Every earlier rung of `rank_for_schedule` is a tie: the three
 * have equal priority (100, 100, 100 in the region-dependence table), the
 * `ldr` is the only one in a lower class off the last-scheduled store
 * (haifa-sched.c:4069-4096; `arm_adjust_cost` at arm.c:2416 returns 0 for the
 * anti-dependence), and the dependent-count rung at :4097 reads four against
 * four.  So the decision falls to the last rung, `INSN_LUID` at
 * haifa-sched.c:4115, and the chain order of the argument-register loads is
 * r1, r2, r3, r0 -- the arg-0 move is emitted LAST.  For the reference's order
 * to fall out, the chain must read r1, r0, r3, r2, i.e. LUID of the `r0 = r6`
 * move must be LESS than LUID of the `0x80 << 14` build.  There is no sched1 in
 * this build, so nothing between expand and sched2 can reorder them.
 *
 * SO THE SINGULAR QUESTION IS: what makes `load_register_parameters`
 * (calls.c:1684-1696, a plain ascending `for (i = 0; i < num_actuals; i++)`)
 * emit the arg-0 move before the arg-2 build.  That loop is ascending and
 * should put r0 first; it does not here, and WHY it does not is the one thing
 * left to find in this function.
 *
 * THIRTY-FOUR SPELLINGS ARE NOW INERT AT TWO, so the call's own spelling is a
 * closed dimension and must not be re-swept.  All measured against this
 * reference with the three-pointer body:
 *
 *   naming the z coordinate; naming x and z; naming z then x; naming the y
 *   coordinate alone; `z = 0x80; z <<= 14;` in two statements; naming all three
 *   coordinates, in forward and in reverse assignment order; a copy of the
 *   actor pointer for the call; `a + 0` as the first argument; the shift written
 *   `(0x80 << 7) << 7`; the shift written as the literal 0x200000; the shift
 *   written `0x1fffff + 1`; a struct type with named f8/f10 fields instead of
 *   casts; an unused extra `int z` declaration; withholding the callee's
 *   prototype entirely; an empty parameter list `()`; a varargs prototype;
 *   `int` return type; `void *` first parameter; all-`int` parameters with the
 *   pointer cast at the call.
 *
 * AND TWO ARE WORSE, which is the useful half: deriving the shift from the
 * already-live `v` (`z = v << 5` or `z = v * 32`, both algebraically 0x200000)
 * costs four more, because cse folds `v` to its constant and then rebuilds it in
 * the wrong place; and moving `s1 = 0x16;` up to sit beside the coordinate
 * assignments costs eight more.
 *
 * STILL SETTLED FROM EARLIER PASSES, and all three re-confirmed under the new
 * body:
 *
 *   A MASK APPLIED TO A BYTE GETS NARROWED UNLESS IT IS NAMED.  The reference
 *   builds -13 as `mov r2,#0xd / neg r2,r2`, the full 32-bit value.  Written
 *   inline as `p[9] = (p[9] & -13) | 8;` gcc notices the result is stored
 *   through a byte and emits a single `mov r2,#0xf3`, which is one instruction
 *   short and cascades.  The mechanism is `convert_to_integer`'s trunc1 block
 *   (convert.c:280-296): BIT_AND and BIT_IOR both fall into it, and the gate
 *   `TRULY_NOOP_TRUNCATION (outprec, inprec)` is the default 1 on this target,
 *   so the narrowing is unconditional -- naming the mask `int mask = -13;` is
 *   what stops the constant itself from folding.  An `int` intermediate for the
 *   LOADED BYTE does not substitute: it is the mask that has to be named.
 *
 *   THE NARROW TYPE FOR THE OR CONSTANT IS A REAL LEVER ON THE DEFINITION
 *   POINT, and it is worth recording even though it is not needed now.  In
 *   `.00.rtl` the constant's set sits at insn 47, before everything, but a
 *   QImode COPY of it is emitted at insn 59 between the `and` (55) and the
 *   `ior` (61) -- that copy is `convert(unsigned char, m)` and it, not the
 *   original set, is what allocation sees.  Declaring the constant
 *   `unsigned char` removes the copy and the `mov` does move above the `and` on
 *   its own.  With one pointer pseudo that cost forty-seven because the
 *   constant then took r0 from the pointer; with three pointer names it is a
 *   tie at two, so the narrow type and the pointer split reach the same place
 *   and only one of them is needed.  `signed char`, `char`, a one-member struct
 *   of `unsigned char`, a one-member union of `unsigned char` and a
 *   one-element `unsigned char` array all behave identically to `unsigned
 *   char`; `short`, `int`, a one-member `int` union and a one-member `int`
 *   struct all behave identically to `int`.
 *
 *   THE ACTOR POINTER MUST COME FROM A LOCAL assigned from the call, not from
 *   the call embedded in the store expression.  The embedded form swaps the
 *   r5/r6 roles of the two long-lived values.
 *
 * THE EXISTENCE PROOF STILL STANDS AND NOW HAS A USE.  A
 * generated-against-hand census of the hoisted shape over all of `asm/` (4,468
 * gcc-generated files, 740 hand-written) finds the reference's form
 * `mov rD,#K / and rA,rB / [lsl rD,#N] / orr|add rA,rD` twice in gcc's own
 * output.  One hit, `src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c_a.c`, gets
 * it because three such edits are separated by CALLS so the constant must take
 * a call-saved register; the other, `src/rom_8a000/rom_93304_c_a_c.s`, gets it
 * because the mask is read again after the `and`.  Neither property is
 * available here, and the pointer split reached the same shape without either,
 * which is the generalisation: what the hoist needs is only that the
 * constant's register be one the `and` does not read.
 
 * ===== BATCH 336: THE "SINGULAR QUESTION" IS ANSWERED, AND IT WAS MISPOSED.
 * ===== THE GATE IS calls.c:848-856 AND IT IS STRUCTURAL FOR THIS ARGUMENT.
 *
 * RE-MEASURED AT 2 of 57, ref 57 ours 57, first difference at index 35.  The
 * body below is unchanged and every figure above reproduces.
 *
 * THE PARK ASKED: "what makes `load_register_parameters` (calls.c:1684-1696, a
 * plain ascending loop) emit the arg-0 move before the arg-2 build."  IT DOES
 * NOT.  In `.00.rtl` the four argument moves are insns **101 (r0), 103 (r1),
 * 105 (r2), 107 (r3)** -- ascending, arg 0 FIRST, exactly as the loop reads.
 * There is nothing to explain there.
 *
 * WHAT ACTUALLY HAPPENS IS RELOAD.  At `.17.lreg` the chain is
 *
 *   95  pseudo61 = [r6+8]            arg-1 value
 *   97  pseudo62 = 0x200000          arg-2 value
 *   99  pseudo63 = [r6+0x10]         arg-3 value
 *   101 r0 = reg32                   arg-0 move
 *   103 r1 = pseudo61 / 105 r2 = pseudo62 / 107 r3 = pseudo63
 *
 * and at `.18.greg` insns 103, 105 and 107 are GONE: reload gave their hard
 * registers to the earlier value insns (95 -> r1, 99 -> r3) and split the
 * constant in place at 97 into **152 `r2 = 0x80`** and **153 `r2 = r2 << 14`**.
 * Insn 101 survives only because its source `reg32` is already r6, so there is
 * nothing to fold it into.  **The arg-0 move is last because the other three
 * were deleted, not because it was emitted late.**
 *
 * WHERE THE THREE VALUE INSNS COME FROM, AND WHY THAT IS THE WHOLE GATE.
 * `calls.c:848-856` forces an argument value into a pseudo when it is not
 * already a REG and `rtx_cost (value, SET) > 2` and
 * `SMALL_REGISTER_CLASSES && *reg_parm_seen` -- both true on ARM for any call
 * with register arguments.  `arm_rtx_costs` (arm.c:2077-2084) gives a CONST_INT
 * under SET: **0 if it is below 256**, `COSTS_N_INSNS (2)` = 6 if
 * `thumb_shiftable_const`, `COSTS_N_INSNS (3)` = 10 otherwise.  0x200000 is
 * `0x80 << 14`, so it costs **6**, so it is copied to a pseudo, so its
 * definition insn lands in the argument-evaluation loop -- which runs BEFORE
 * `load_register_parameters`.  `LUID(lsl) < LUID(mov r0)` is therefore forced
 * for ANY third argument that is a constant of 256 or more.
 *
 * AND THE SCHEDULER'S LAST RUNG MAKES THAT DECISIVE.  Corrected from the park,
 * which had the rungs right and the DIRECTION of the final one unstated: the
 * chosen insn is the LAST element of the sorted ready array, and
 * `rank_for_schedule`'s final rung (haifa-sched.c:4111) is
 * `INSN_LUID (tmp) - INSN_LUID (tmp2)` with the arguments swapped at entry, so
 * **the LOWEST LUID wins**.  Read off the scheduler log: at t=171 the ready list
 * is `99 101 153`; insn 99 (`ldr r3,[r6,#0x10]`) is demoted to class 2 by the
 * class rung because it is anti-dependent on the just-scheduled
 * `str r3,[r6,#0x30]`, and between 101 and 153 -- both class 3, both with one
 * dependent -- the lower LUID, 153, wins.  So we emit `lsl, ldr, mov` where the
 * ROM has `mov, ldr, lsl`.
 *
 * THE REQUIREMENT IS THEREFORE WEAKER THAN THE PARK STATED, AND STILL BLOCKED.
 * The park asked for chain order r1, r0, r3, r2.  Only ONE inequality matters:
 * **LUID(101) < LUID(153)**.  The relative order of 99 and 101 is irrelevant,
 * because the class rung demotes 99 at t=171 whichever way round they are, and
 * at t=172 the lower-LUID of {99, 153} wins anyway.  But the one inequality
 * that matters is the one calls.c:856 forbids.
 *
 * THE ONE ESCAPE LEFT, NAMED SO IT CAN BE TESTED RATHER THAN RE-DERIVED.
 * If pseudo62 were SPILLED with a `REG_EQUIV (const_int 2097152)` note, reload
 * would rematerialise it at the USE (insn 105, which is AFTER 101) instead of
 * splitting it at the definition -- which is exactly what it already does for
 * the 0x6666 and 0xcccc constants, whose `.18.greg` insns 87 and 91 carry
 * REG_EQUIV notes and whose `ldr r3,=...` lands immediately before each store,
 * as in the ROM.  pseudo62 is NOT spilled: it gets r2 outright.  So the open
 * question is a REGISTER-PRESSURE question -- what makes the 0x200000 pseudo
 * fail to get a hard register -- and NOT a spelling question.  Do not re-sweep
 * the call's spelling; the park's thirty-four inert rows stand and this batch
 * adds no reason to doubt them.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 2 of 57, instruction counts EQUAL, byte size and pool
 *   count equal too, so this figure IS a real distance.
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 19 placements swept, but 10 WERE BIT-IDENTICAL to
 *   the base, so only 9 DISTINCT PROGRAMS were actually tested.
 *   Those 9 are inert; the other 10 measured nothing.  Nothing capped, nothing failed to compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  The residue is NOT a placement or flag question;
 *   it is a register choice, a tie, or a pass-level effect, and it needs a
 *   hypothesis.
 *
 *   Read as "inert against THIS body" -- a recorded negative expires silently
 *   when the body changes.

 * ===== BATCH 350 BRIEF A: 2 of 57 HELD.  THE "THIRTY-FOUR SPELLINGS" ARE
 * ===== ONE PROGRAM -- AND ESCAPE (b) IS UPGRADED FROM NAMED TO VALIDATED.
 *
 * RE-DERIVED FIRST: **2 differing encodings of 57**, ref 57 / ours 57,
 * instructions 47 / 47, first at index 35 (ref 1c30 `mov r0,r6`, ours 0392
 * `lsl r2,#14`), no SIZE / INSTRUCTION COUNT / RELOCATIONS line.  The figure IS
 * a distance.  BODY UNCHANGED.  Pin-free, device-free, flag-free.
 *
 * ---- THE CENSUS: 20 OF THE 34 CLAIMED-INERT ROWS REBUILT, ALL 20 ARE THE
 * ---- BASE OBJECT.  THE CALL'S SPELLING IS CLOSED BY A BIT-IDENTICAL SET.
 *
 * Swept with tools/sweep_variants.py's object fingerprint: naming z; naming x
 * and z; naming z then x; naming y alone; `z = 0x80; z <<= 14;`; all three
 * coordinates forward and reverse; a copy of the actor pointer; `a + 0`;
 * `(0x80 << 7) << 7`; the literal `0x200000`; `0x1fffff + 1`; `0x80 * 0x4000`;
 * an unused extra `int z`; no prototype; `()`; a varargs prototype; `int`
 * return; `void *` first parameter; all-`int` parameters with a cast at the
 * call -- **every single one BIT-IDENTICAL TO THE BASE.**  Twenty recorded
 * rows, ONE program.  No spelling of this call reaches the back end, which is
 * the strongest form of the park's own "do not re-sweep the call".
 *
 * ---- THE GATE RE-READ ON DISK, WITH THE TWO CONSTANTS THE PARK ASSERTED
 *
 * `calls.c:848-856` (in `precompute_register_parameters`) copies an argument
 * value to a pseudo when it is not a REG, is not BLKmode,
 * `rtx_cost (value, SET) > 2`, and `(SMALL_REGISTER_CLASSES && *reg_parm_seen)`.
 * Both of the park's constants CHECK OUT:
 *   * `SMALL_REGISTER_CLASSES` is `TARGET_THUMB` -- **arm.h:1061** -- so 1 here.
 *   * `*reg_parm_seen` is set by the preliminary loop at **calls.c:812-818**
 *     for ANY argument that has a register, so 1 for any call with register
 *     arguments.
 *   * `arm_rtx_costs`' CONST_INT-under-SET branch (**arm.c:2077-2085**) returns
 *     0 below 256, `COSTS_N_INSNS (2)` = 6 for a `thumb_shiftable_const`, 10
 *     otherwise.  0x200000 is `0x80 << 14`, so 6 > 2.
 * The copy therefore happens, its def insn lands in the argument-evaluation
 * loop (which runs before `load_register_parameters`), and LUID(the 0x200000
 * build) < LUID(`r0 = r6`) is forced.  **The park's gate is correct.**
 *
 * ---- ESCAPE (b) IS NOT JUST "THE ONE LEFT", IT WOULD ACTUALLY PRODUCE THE ROM
 *
 * The park named "spill pseudo62 with a `REG_EQUIV`, so reload rematerialises
 * at the use" as the one escape, without checking that the rematerialisation
 * emits the ROM's instructions rather than a pool load.  It does, and here is
 * the chain:
 *   * `THUMB_LEGITIMATE_CONSTANT_P` (**arm.h:1807-1809**) is true for ANY
 *     `CONST_INT`, so reload's `reg_equiv_constant` path is open for 0x200000 --
 *     it is NOT forced to memory.
 *   * the rematerialised `(set (reg:SI) (const_int 2097152))` matches
 *     alternative **`K`** of `*thumb_movsi_insn` (**arm.md:3838-3841**), where
 *     `CONST_OK_FOR_THUMB_LETTER (VAL, 'K')` is `thumb_shiftable_const`
 *     (**arm.h:1095-1098**) and the output template is `#` -- so it SPLITS into
 *     `mov r2,#0x80 / lsl r2,#14`, exactly the ROM's pair, at the USE, i.e.
 *     after `load_register_parameters` has already emitted `r0 = r6`.
 * So escape (b) is the route, and the `ldr r2,=0x200000` pool-word worry that
 * would have killed it does not arise.  **Recorded so the next round does not
 * have to re-derive it before testing pressure.**
 *
 * ---- BUT WHY THE PSEUDO GETS r2 IS NO LONGER "AN OPEN PRESSURE QUESTION"
 *
 * It is local-alloc's copy suggestion, and the function's shape forces it:
 *   * the reference function **HAS NO BRANCH AT ALL** (54 lines, four `bl`s and
 *     a `bx`, zero conditional jumps), so the whole body is ONE basic block and
 *     EVERY pseudo is single-block -- `global_alloc` owns none of them.
 *   * the use of the constant pseudo is the copy `r2 = pseudo`, and a copy
 *     between a pseudo and a hard register records that hard register as a
 *     COPY SUGGESTION for the quantity (**local-alloc.c:1683-1692**).
 *   * the suggested-register pass runs FIRST and, for a quantity with
 *     suggestions, `find_free_reg` is called with `just_try_suggested = 1`
 *     (**local-alloc.c:1341-1347**), which restricts the search to exactly those
 *     registers (**:2005-2016**).
 *   * at the constant's definition point r0-r3 are all dead: the mask's last use
 *     is the `strb r2,[r1,#9]` two instructions earlier.
 * So r2 is handed over on the suggested pass whatever the priority ratio, and
 * denying it needs either a SECOND BASIC BLOCK or a value live across the
 * definition point.  **Both are program changes in a function whose other 55
 * encodings are exact.**  That is the bound: not "find the pressure", but
 * "there is no pressure to find in a single-block, four-live-value function".
 *
 * NEXT: nothing in the call.  If this park is scheduled again, the only
 * question worth a round is whether a construct exists that puts a second,
 * zero-cost use on the arg-2 pseudo (which would defeat the copy suggestion
 * without adding an instruction).  Do not re-sweep the twenty rows above: they
 * are one object.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_947_200a1ac(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    unsigned char *q;
    int v;
    int m;
    int mask;
    int s1;
    int s2;

    a = __MapActor_GetActor(0xe);
    b = __MapActor_GetActor(0xd);
    v = 0x80 << 9;
    *(int *)(b + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xd) + 0x1c) = v;
    p = *(unsigned char **)(__MapActor_GetActor(0xd) + 0x50);
    mask = -13;
    m = 8;
    p[9] = (p[9] & mask) | m;
    q = *(unsigned char **)(a + 0x50);
    q[9] = (q[9] & mask) | m;
    *(int *)(a + 0x34) = 0x6666;
    *(int *)(a + 0x30) = 0xcccc;
    __Actor_TravelTo(a, *(int *)(a + 8), 0x80 << 14, *(int *)(a + 0x10));
    __MapActor_WaitMovement(0xe);
    s1 = 0x16;
    s2 = 0x10;
    __Func_8010704(0x14, 0xe, 1, 1, s1, s2);
}
