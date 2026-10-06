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
