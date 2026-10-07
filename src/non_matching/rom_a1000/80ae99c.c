/* BATCH 333 DELTA -- Func_80ae99c IS NOW 18 differing encodings of 39 (ref 39,
 * ours 39), DOWN FROM thirty-eight of thirty-nine, AND THE LENGTHS NOW AGREE.
 * Size 84 against 84, relocations 2 against 2, so the figure is a DISTANCE and
 * not a phase offset for the first time in this park's life.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80ae99c.c \
 *     asm/rom_a1000/rom_ae88c_c_c_c.s --func Func_80ae99c
 *
 * WHAT CHANGED, AND WHY THE PARK COULD NOT SEE IT. The batch-297a delta below
 * states the residue correctly as TWO deficits -- the if-conversion that
 * deletes the join, and the address form -- and it measured the one spelling
 * that fixes both: a POINTER PER ARM, which it recorded as overshooting by one
 * encoding and therefore set aside. It was not overshooting. There was a THIRD
 * residue, a SURPLUS, sitting in the tail and hiding it:
 *
 *     rom    mov r3, #0 / strb r3, [r0, #4] / strh r3, [r0, #0xc]
 *     ours   mov r3, #0 / strb r3, [r0, #4] / ldrh r3, .L / strh r3, [r0, #0xc]
 *
 * A LITERAL ZERO STORED THROUGH A HALFWORD IS AN HImode CONSTANT, so it goes to
 * the pool -- `*thumb_movhi_insn` with `pool_range` 64, arm.md:4318 and :4353 --
 * costing one instruction AND one pool word. The ROM re-uses the SImode zero it
 * already has in the register. Naming the zero as an `int` local and storing
 * that local through both the byte and the halfword keeps it in SImode and the
 * pool entry disappears. This is the SAME MODE MECHANISM that landed the two
 * palette functions in this batch, running in the OPPOSITE direction: there a
 * `(u16)` cast was added to PUT a mask in the pool, here a cast is removed to
 * KEEP a zero out of it. Worth reading as one lever with two signs.
 *
 * So the park's own two-deficit diagnosis was right and its arithmetic was off
 * by a surplus it had not looked for. A park measuring a candidate as "one too
 * long" should check the TAIL for an independent surplus before discarding the
 * candidate -- three residues cannot be read off one length.
 *
 * WHAT IS LEFT IS ONE THING: THE PARAMETER-COPY ROTATION, and it is now the
 * whole residue rather than a cascade. All four arguments are live on entry and
 * the iwram pool address needs a low register, so one of them must be evicted.
 * The ROM evicts the LAST argument, freeing its register for the pool address
 * and leaving the third argument undisturbed in place until it is pushed as the
 * fifth call argument. We evict the THIRD argument, use its register for the
 * pool address, and must then stage the fifth argument through the evicted copy
 * -- which also flips the order of the stack-argument store against the
 * register-argument setup, since the store now has a dependence the ROM's does
 * not. Every difference in the residue is that one choice.
 *
 * MEASURED IN BATCH 333, eighteen variants, EVERY ONE EXACTLY at the figure
 * above -- so these dimensions are closed, not merely unpromising:
 *   four declaration orders of the four locals                     all equal
 *   three arm spellings: equality, inequality, conditional expr    all equal
 *   the condition argument copied into a named local first         equal
 *   the fifth call argument copied into a named local first        equal
 *   the loaded id copied into a named local before the call        equal
 *   the base retyped as a halfword pointer with halfword indices   equal
 *   the second call argument spelled as a wider shift              equal
 *
 * NEXT, and it is a specific question about ONE decision rather than a search:
 * which incoming argument local-alloc evicts to free a low register for the
 * pool address. That is `find_free_reg` walking `reg_alloc_order`
 * (local-alloc.c:2021-2050) over quantities ordered by `qty_compare`, and the
 * thing to read is which quantity is reached first with all four argument
 * registers still occupied. No source spelling found here moves it, which is
 * consistent with it being an ordering inside the allocator rather than a
 * liveness fact the source controls -- but that should be CONFIRMED from the
 * allocator dump before the park is called blocked, because the eighteen
 * variants above all leave the liveness shape identical and so cannot
 * distinguish the two explanations.
 *
 * THE BODY BELOW IS THE IMPROVED ONE: pointer per arm, and the tail's zero
 * named as an SImode local.
 */

/* MEASURED FIGURE, backfilled in batch 324 (this park carried none).
 *
 *   SUPERSEDED: it read thirty-eight differing of thirty-nine, ref thirty-nine
 * against ours thirty-eight.
 *
 * The one call relocation is the same symbol at -4, so this is a distance: one
 * instruction missing at the very top (first diff at index 1) shifts
 * essentially the whole function.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80ae99c.c asm/rom_a1000/rom_ae88c_c_c_c.s --func Func_80ae99c
 *
 * The figure is EVIDENCE.  Everything below it is a HYPOTHESIS, and across
 * pass two a park's diagnosis has been wrong roughly 40 times in 42.
 */

/* ================== BATCH 297a DELTA -- Func_80ae99c ==================
 * RE-MEASURED: thirty-eight of thirty-nine, and NOT a distance -- ref thirty-nine
 * encodings, ours thirty-eight.
 * The park said "39 of 40, two lines short"; the figures have drifted by one and
 * the correct statement is ONE INSTRUCTION SHORT.  84 bytes against 84 (the pool
 * padding absorbs the difference), 2 relocations against 2.  The park also
 * carried no `Verify with:` recipe, so parkcheck could not check it:
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80ae99c.c \
 *     asm/rom_a1000/rom_ae88c_c_c_c.s --func Func_80ae99c
 *
 * SPLIT SHAPE: two functions (Func_80ae99c, Func_80ae9f0 -- the latter also
 * parked) plus a `.rodata` section whose labels .Laed4c and .Laedcc are already
 * `.global`; datacheck.py says NEITHER function reads a data label, so a landing
 * is a TEXT/DATA split with NO new export.
 *
 * THE PASS IS NOW NAMED FROM THE DUMPS: .14.ce (if_convert).  Before it,
 * x.c.13.combine has the ROM's control flow exactly --
 *     (jump_insn 22) if (r35 != 0) goto 28
 *     bb1: (insn 25) r37 = 914 ; (jump_insn 26) goto 32 ; (barrier 27)
 *     bb2: (code_label 28) (insn 31) r37 = 916
 *     bb3: (code_label 32) join
 * -- and x.c.14.ce has hoisted insn 25 ABOVE jump_insn 22, reversed the test to
 * `eq`, retargeted it at the join label 32, dropped the unconditional jump and
 * the barrier, and left label 28 with "[0 uses]".  That is a
 * dead_or_predicable-style move of the then-block's single set, not conditional
 * execution (thumb has none), and it is what costs the join instruction.
 *
 * THE PARK'S ONE UNTRIED IDEA IS NOW TRIED AND IT DOES NOT WORK.  The park
 * noted the arms have different costs -- 0x392 = 914 is not thumb-shiftable so it
 * pools, 0xe5 << 2 = 916 = 229 << 2 is shiftable so it is a mov/lsl pair -- and
 * guessed that forcing both to the same form might tip the decision.  Measured,
 * every attempt is exactly 38 of 39 (38 enc, 84 bytes):
 *     else arm written as two statements `off = 0xe5; off <<= 2;`      38
 *     both arms written as two statements                             38
 *     `off = (d == 0) ? 0x392 : 0xe5 << 2;`                           38
 *     `switch (d) { case 0: ... default: ... }`                       38
 *     destructive pointer add `g += off;` then `*(u16 *)g`            38
 *     the two-statement else arm PLUS `g += off;`                     38
 * The reason is visible in the dump: cse1 folds `off = 0xe5; off <<= 2;` back to
 * `(set r37 (const_int 916))` long before .14.ce runs, so ifcvt still sees one
 * single-set insn per arm.  Only the pointer-per-arm form
 * (`q = g + 0x392` / `q = g + (0xe5 << 2)`) changes the block shape, and it is
 * 30 of 39 at 40 encodings -- one too LONG, i.e. it overshoots.
 *
 * SO THERE ARE TWO THINGS TO FIX, NOT ONE, and the park only named the first:
 *  (a) .14.ce hoisting the pooled constant and deleting the join -- 1 instruction;
 *  (b) THE ADDRESS FORM.  The ROM does `add r3, r3, r1 / ldrh r0, [r3]`; we do
 *      `ldrh r0, [r2, r1]`, a register-offset load, which is another instruction
 *      cheaper.  `g += off;` does not reach it (cse recombines the add into the
 *      addressing mode).
 * Together they also rotate the parameter saves: the ROM copies `d` (r3) out
 * first, freeing r3 for the iwram pool address and leaving `c` in r2 until
 * `str r2,[sp]`, while we copy `c` out first and put `g` in r2.  That cascade is
 * why 38 of 39 differ rather than three.
 * -- scratch_elev/b297a/t7
 */

/* Func_80ae99c -- 0x080ae99c  (asm/rom_a1000/rom_ae88c_c_c_c.s)
 *
 * BLOCKER: gcc IF-CONVERTS a two-arm constant selection that the ROM keeps as
 * a real branch, and the parameter saves then rotate. 39 of 40, two lines short.
 *
 *     rom   cmp r5,#0 / bne L0 / ldr r1,=0x392 / b L1
 *           L0: mov r1,#0xe5 / lsl r1,#2 / L1: add r3, r1
 *     ours  ldr r1,=0x392 / cmp r3,#0 / beq L0
 *           mov r1,#0xe5 / lsl r1,#2 / L0:
 *
 * We hoist the pooled 0x392 above the compare and conditionally overwrite it,
 * which is two instructions shorter and loses the join. Everything downstream
 * shifts, which is why the count is 39 rather than the handful of real
 * disagreements.
 *
 * MEASURED, both inert at 39:
 *   writing the arms so the ROM's fall-through (0x392) is the `if` body
 *   writing them as explicit `goto other; ... goto join;` around the two
 *     assignments, which is literally the ROM's control flow
 *
 * So the if-conversion is not reachable by control-flow spelling here. That is
 * worth recording against the block-layout reading, which is usually reliable:
 * it tells you which arm gcc will make the fall-through WHEN IT EMITS A BRANCH,
 * and says nothing when gcc decides not to emit one at all. Two constant
 * assignments with no other work in either arm is the shape where it declines.
 *
 * The remaining question is what makes the ROM's compiler keep the branch. One
 * asymmetry is visible and untested: the two constants have different COSTS --
 * 0x392 is a pool load and 0xe5 << 2 is a mov/lsl pair -- so the arms are not
 * interchangeable to a cost model, and forcing both to the same form (or making
 * one more expensive) might tip it. Not tried.
 */
extern int iwram_3001f2c;
extern void *_Func_801eadc(int id, int a, int b, int c, int d);

int Func_80ae99c(int a, int b, int c, int d)
{
    char *g;
    unsigned short *q;
    unsigned char *r;
    int t;

    g = (char *)iwram_3001f2c;
    if (d == 0)
        q = (unsigned short *)(g + 0x392);
    else
        q = (unsigned short *)(g + (0xe5 << 2));
    r = (unsigned char *)_Func_801eadc(*q, 0x80 << 23, a, b, c);
    if (r == 0)
        return -1;
    t = 0;
    r[4] = t;
    *(unsigned short *)(r + 0xc) = t;
    r[5] = 1;
    return 1;
}
