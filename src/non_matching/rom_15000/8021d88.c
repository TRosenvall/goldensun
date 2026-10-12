/* Func_8021d88 (0x08021d88) -- NON-MATCHING, 7 of 53 encodings.
 *
 * THIS BODY'S OWN FIGURE: 7 of 53 encodings.  MEASURED batch 344 brief D,
 * re-measured by the coordinator at install.
 *     ENCODINGS differ in 7 of 53    INSTRUCTIONS ref 47 / ours 47  (EQUAL)
 *     SIZE equal at 116 bytes        RELOCATIONS CLEAN
 *   The park this replaces measured 40 of 53 at ref 47 / ours 48, i.e. OURS ONE
 *   INSTRUCTION LONG, so that 40 was a PHASE reading.  THE LENGTH GAP IS CLOSED
 *   and this 7 is a real distance for the first time.  PINS 0, shims 0,
 *   devices 0 -- genuinely, see the coordinator note at the foot of this header.
 *
 * ADDING `volatile` TO THE p+8 READ REACHES 4 of 53 at the same 47/47, and this
 * body deliberately does NOT carry it.  It buys a REGISTER CHOICE, not a
 * semantic reading, and `shimcount` does not count a volatile access -- so a
 * park body holding one reads as device-free to every instrument here.  Pass 3
 * may re-litigate it.  Both
 * figures are given because they differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8021d88.c \
 *     asm/rom_15000/rom_20198_c_c_c_c_c.s --func Func_8021d88
 *
 * Split shape.  tools/datacheck.py on the reference:
 *     data sections : .rodata
 *     functions     : Func_8021cb8, Func_8021d88
 *     Func_8021d88  reads no data label -> split needs NO new export
 *     -> converting a function here needs a TEXT/DATA SPLIT
 * tools/split_s.py --dry-run:
 *     would write ..._c_c_c_c_c_a.s (1 function, 95 lines)
 *                 ..._c_c_c_c_c_b.s (1 function, 61 lines)   <- Func_8021d88
 *                 ..._c_c_c_c_c_c.s (1 function, 36 lines)
 *     linker scripts updated: stage1.ld
 * Pins: 0.
 *
 * ======================================================================
 * THE PARK'S CAUSE A WAS NOT AN ALLOCATION BLOCKER.  IT WAS THE WRONG
 * FIRST ARGUMENT, AND THAT ONE TOKEN IS WORTH 40 -> 11.
 * ======================================================================
 *
 * The park called its residue "cause A: HIGH REGISTER VERSUS SPILL", recorded
 * it as "the fifth specimen of the class, with rom_9000/800bbc0.c,
 * ovl_7e3e08/2008f94.c, rom_15000/8020150.c and rom_b5000/80b6a60.c", and
 * closed with "NEXT: nothing source-level for cause A.  It needs the
 * spill-versus-high-register class solved, which is tracked across five parks."
 *
 * *** THAT IS REFUTED.  READ THE REFERENCE'S REGISTERS. ***
 *
 *     push {r5, r6, lr} / mov r6, r8 / push {r6}
 *     mov  r3, r1 / lsl r5, r3, #3 / sub r5, r3
 *     mov  r4, r0          <- `a` copied to r4; r0 IS NEVER WRITTEN AGAIN
 *     mov  r8, r2          <- `c`
 *     ... lsl r5,#2 / mov r2,#0x82 / add r6,r4,r5 / lsl r3,#4 / lsl r2,#1
 *     sub  sp, #4
 *     add  r6, r2          <- r6 = p
 *     mov  r1, r8 / mov r2, r3 / str r4, [sp]
 *     bl   Func_8021cb8
 *
 * r0 still holds the incoming `a` at the `bl`.  The reference's first argument
 * is `a`, NOT `p`.  The park passed `p`, so gcc had to materialise p into r0
 * (`adds r0, r6, #0`) AND keep `a` alive in a second high register -- which is
 * the entire "two high registers against the ROM's one" observation, and the
 * entire extra instruction.  Change `Func_8021cb8(p, c, m)` to
 * `Func_8021cb8(a, c, m)` and gcc produces the ROM's allocation EXACTLY:
 * `mov r4, r0`, ONE high register, and the four-instruction frame spill
 * (`sub sp,#4` / `str r4,[sp]` / `ldr r4,[sp]` / `add sp,#4`) that the park
 * described as something the ROM paid and we did not need.
 *
 *     40 of 53, ref 47 / ours 48   ->   11 of 53, ref 47 / ours 47, reloc CLEAN
 *
 * So the first-argument change alone closes the length gap and 29 of the 40.
 * `p` is still needed -- it is the base of the three p-relative accesses -- it
 * is just not what is passed.
 *
 * NOTE FOR THE OTHER FOUR PARKS IN THAT "CLASS": this one was never a member.
 * A function that holds one more long-lived value than the reference because
 * its SOURCE names one more value looks identical, at the prologue, to one that
 * holds one more because the allocator found an extra register.  Check the
 * reference's argument registers at every call before accepting the diagnosis.
 *
 * ======================================================================
 * THEN THE ANCHOR AXIS: 11 -> 7, AND IT IS TWO SCHED2 PAIR INVERSIONS
 * ======================================================================
 *
 * At 11 the residue was three facts.  Two of them were adjacent-pair
 * inversions around the frame insns --
 *     ref   lsl r2,#1 / sub sp,#4 / add r6,r2      ours  ... / add r6,r2 / sub sp,#4
 *     ref   lsl r2,#1 / ldr r4,[sp]                ours  ldr r4,[sp] / lsl r2,#1
 * -- and a `do { } while (0);` anchor removes both.  Swept at all 14 statement
 * boundaries: 20, 16, 14, 14, 12, 9, 9, **7, 7, 7, 7**, 17, 17, 11.  The four
 * tying placements are before `*(int *)(p + 4) = 0x80002000;` through before
 * `r = Func_8021c64(...)`; this body uses the first of them.  The anchor is a
 * NOTE_INSN_LOOP_BEG/_END pair (haifa-sched.c:3714, 3735-3736), emits nothing,
 * and shimcount does not count it.
 *
 * -fno-schedule-insns2 reads 26 -- it is the wrong instrument for a sched2 tie
 * here, because it loses far more than the two inversions.
 *
 * ======================================================================
 * THE REMAINING 4 ARE ONE FACT, AND CAUSE B'S MECHANISM IS REFUTED
 * ======================================================================
 *
 * What is left, read off the aligned listing, is ONE pool fix and nothing else:
 *     ref pool   0x3ff, 0x80002000, 0xfffffc00
 *     ours       0x3ff, 0xfc00,     0x80002000
 * plus the one `ldr` whose pc-relative offset moves with it.  Every instruction
 * aligns and matches.
 *
 * `-da` says it in one line each (<base>.c.26.mach):
 *     ;; SImode fixup for i64; addr 46, range (0,1020): 0x80002000
 *     ;; HImode fixup for i92; addr 70, range (0,64):   0x3ff
 *     ;; HImode fixup for i99; addr 80, range (0,64):   0xfc00     <- WRONG MODE
 * The reference needs that third fix to be **SImode 0xfffffc00**: with range
 * 1020 its max_address is 80+1020 = 1100, above 0x80002000's 46+1020 = 1066, so
 * add_minipool_forward_ref (arm.c:4817-4860) inserts it LAST -- the ROM's order.
 * Ours is HImode, max_address 80+64 = 144, below both, so it sorts to the HEAD.
 * That is the same mechanism src/non_matching/rom_8a000/8091eb0.c is parked on,
 * running on a different constant: `*thumb_movhi_insn` (arm.md:4318-4320)
 * constrains operand 1 as "l,mn,l,*h,*r,I", `n` matches ANY const_int at
 * alternative 1, and recog takes the first match, so EVERY HImode const_int in
 * Thumb pools.  Our 0x3ff fix is HImode too and that one is CORRECT -- the
 * reference's is also HImode, which is why it also sorts to the head.
 *
 * *** CAUSE B'S MECHANISM IS WRONG, AND SO IS ITS CLAIM ABOUT THIS BODY. ***
 * The park says the narrowing is combine's `simplify_and_const_int` using
 * `nonzero_bits`, cites combine.c:7987-8000, and states the generalisable lever
 * as "to keep a wide mask constant out of simplify_and_const_int's narrowing
 * give the masked value a pseudo with MORE THAN ONE SET.  Reusing an existing
 * local is the pin-free way to do it."  Three things are wrong:
 *
 *   1. THE NARROWING IS NOT IN COMBINE.  Grepping every `-da` dump for the two
 *      constants: `64512` (0xfc00) is present in **.00.rtl** and -1024
 *      (0xfffffc00) is present in NONE of them.  The constant is already
 *      narrowed before the first RTL dump, i.e. at fold/expand -- the
 *      assignment to an `unsigned short` lvalue distributes its truncation over
 *      the `|` and then over the `&`.  No RTL pass is involved and no RTL
 *      barrier can reach it.
 *   2. REG_N_SETS IS NOT THE GATE even in combine.  combine.c:7991-7997 reads
 *      `(reg_last_set_label[REGNO (x)] == label_tick || (pseudo && REG_N_SETS
 *      == 1 && !live_at_start))`.  The FIRST disjunct alone satisfies it, and
 *      this function is one basic block, so label_tick never advances and the
 *      REG_N_SETS arm is unreachable here.
 *   3. MEASURED: a body with `v` read twice (two sets, same value) reads 11,
 *      identical to the single-set body.  So does a body reusing `v` for the
 *      0x80002000 value, for `j`, and for the zero store (16, 21, 16).  The
 *      lever does not work on this park in any of its forms.
 *
 * WHAT THE PARK'S OWN BODY ACTUALLY POOLED, and nobody checked: `0x0000fc00`,
 * not `0xfffffc00`.  Its pool word COUNT matched the reference, which is what
 * made "SIZE EXACT / ENCODING COUNT EXACT" true, and the park read that as
 * evidence the wide constant had survived.  It had not.  So cause B was never
 * fixed and the "two levers in direct tension" framing is void: the inlined
 * first read costs NOTHING, because there was nothing to lose.
 *
 * ======================================================================
 * THE VOLATILE IS WORTH EXACTLY THREE ENCODINGS, AND IT IS A REGISTER
 * CHOICE -- NOT A READING
 * ======================================================================
 *
 * Without it the residue is 7 and the extra three are a register swap at the
 * two pool loads:
 *     ref   ldr r3,=0x3ff / ldrh r2,[r6,#8] / and r0,r3
 *     ours  ldr r2,=0x3ff / ldrh r3,[r6,#8] / and r0,r2
 * `volatile` on the p+8 read flips it and those three go.  It is NOT a
 * `volatile` in the sense of docs/elevation.md:5239 ("a global the ROM RELOADS
 * is volatile"): the reference's re-read of p+8 needs no volatile, because
 * Func_8021c64 is a call and cse invalidates memory at a CALL_INSN
 * (cse.c:5710-5715) by itself.  It is being used for its allocation side
 * effect.  Kept because it is an admissible spelling -- 181 landed files carry
 * a volatile cast and tools/shimcount.py's device classes are all `__asm__`
 * forms, so it is neither a pin nor a device -- but FLAGGED for pass 3.
 * `const volatile` reads the same 4; 10 other spellings of the same three
 * encodings do not reach it (see below).
 *
 * MEASURED INERT at 7 (device-free, volatile-free, anchored), i.e. the three
 * encodings do not move: the OR operands swapped; the `r & 0x3ff` half named in
 * a local, before and after the read; declaration order `r` before `v`; `j`,
 * `m` and `k` as the halfword carrier; `volatile` on the STORE instead of the
 * read; a named `unsigned short *` for p+8.
 * MEASURED WORSE, and all of them LOSE AN INSTRUCTION (ref 47 / ours 46, size
 * short by 4) -- a better-looking figure obtained by doing less work than the
 * ROM, so they fail the per-opcode screen: the AND hoisted into its own
 * statement in any of eight forms (`v = v & 0xfffffc00;`, `v &= 0xfffffc00;`,
 * `w = v & 0xfffffc00;`, `v = v & ~0x3ff;`, crossed with and without volatile
 * and at four anchor positions) -- all 16; `v` reused as the OR's output (23);
 * an int output temp (23); the compound three-statement form (22); a named mask
 * local (22).
 * MEASURED WORSE, other: `*(short *)` (signed) read, 23; a named halfword
 * pointer without volatile, 23 at 48 instructions.
 *
 * FLAG AXIS, re-crossed against the 11-body (the brief's instruction after a
 * real source change) -- NOTHING IMPROVES: -fno-gcse, -ffixed-r7,
 * -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-cse-after-loop,
 * -fno-rerun-loop-opt, -fno-peephole, -fno-regmove and -fno-cse-follow-jumps
 * all TIE at 11; -fno-schedule-insns2 26, -fno-expensive-optimizations 23,
 * -O1 50 (and loses two pool words).  The nine tying flags are the crossing
 * partners for any future source change.
 *
 * NEXT, and it is one question, not a class: GET THE THIRD POOL FIX INTO
 * SImode.  The narrowing is at fold/expand, so the search is a C-level one --
 * a spelling in which the masked half is NOT an operand of a value assigned to
 * an `unsigned short` lvalue, WITHOUT hoisting it into its own statement (which
 * costs an instruction).  Everything else in this function is byte-exact.
 *
 * PIECE NOTE.  The reference holds TWO functions, Func_8021cb8 (also parked,
 * src/non_matching/rom_15000/8021cb8.c, 14 of 92) and Func_8021d88.  The park
 * said "A piece converts whole or not at all, so neither can land alone" --
 * THAT IS FALSE and the shared brief says to ignore it: split_s.py cuts
 * before/target/after, so either member cuts out alone.  The dry-run above is
 * the proof.
 *
 * COORDINATOR CHANGE, batch 344: the `volatile` is REMOVED from the p+8 read and
 * this park records the DEVICE-FREE figure of 7, not 4.  Both were re-measured
 * against asm/rom_15000/rom_20198_c_c_c_c_c.s at the head of the install:
 *
 *     with    volatile :  4 of 53, instructions 47/47
 *     without volatile :  7 of 53, instructions 47/47
 *
 * The brief's own finding is that the volatile buys only a register choice, and
 * `tools/shimcount.py` does NOT count a volatile access as a device -- so a park
 * body carrying one reads as device-free to every instrument in this tree and to
 * the next agent who picks it up.  That is a trap, and it is the kind of trap
 * this project has already paid for: an inherited control measured with a device
 * in is UNTRUSTED, and the three encodings are worth nothing while the park is
 * not shipping either way.  The length gap is closed at 47/47 in BOTH bodies,
 * which is the result that matters and is independent of the volatile.
 *
 * So the 7 is the honest baseline and the 4 is recorded here as a measured
 * observation about register selection, not as the park's state.
 */
extern void Func_8021cb8(void *p, int c, int m);
extern int Func_8021c64(int v, int c);

void Func_8021d88(unsigned char *a, int i, int c)
{
    unsigned char *p;
    int k;
    int j;
    int m;
    int v;
    int r;

    k = (i * 8 - i) << 2;
    p = a + k;
    m = i << 4;
    p += 0x82 << 1;
    Func_8021cb8(a, c, m);
    j = k + (0x8e << 1);
    *(int *)(a + j) = c;
    do { } while (0);
    *(int *)(p + 4) = 0x80002000;
    *(int *)(p + 8) = 0;
    k += 0x88 << 1;
    r = Func_8021c64(*(unsigned short *)(a + k), c);
    v = *(unsigned short *)(p + 8);
    *(unsigned short *)(p + 8) = (r & 0x3ff) | (v & 0xfffffc00);
}
