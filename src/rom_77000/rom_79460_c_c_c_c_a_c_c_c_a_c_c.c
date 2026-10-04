/* SetDjinni  --  0x0807a2e4
 *
 * ===== BATCH 322g -- BYTE-IDENTICAL.  14 of 51 -> 0 of 51. =====
 * 108 bytes, 51 encodings and 3 relocations identical.  ZERO PINS, ZERO SHIMS.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_c.c \
 *     asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_c.s --whole
 *
 * LANDING SHAPE: WHOLE FILE, NO SPLIT.
 * asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_c.s holds exactly one
 * `.thumb_func_start` (SetDjinni) and tools/datacheck.py prints nothing -- no
 * data section.  split_s.py is not needed.  Installed path is
 * src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_c.c; stage1.ld already names
 * rom_79460_c_c_c_c_a_c_c_c_a_c_c.o and does not change.  No new export.
 *
 * ---------------------------------------------------------------------------
 * THE EDIT: THE LAST THREE STATEMENTS LEAVE THE `if` ARM AND FOLLOW THE
 * `if/else`.  Semantically identical, because the else arm returns.
 *
 * ---------------------------------------------------------------------------
 * THE PARK CALLED IT ONE CAUSE.  IT IS TWO, AND THE ONE IT NAMED IS THE SMALL
 * ONE AND NOT INDEPENDENT.
 *
 * The park said: "BLOCKER: constant hoisting ... gcc emits `mov r0,#0x8e /
 * lsl r0,#1` before the `orr`/`str` pair that ends the previous statement; the
 * ROM emits them after", and "What is left is one hoisted constant pair and the
 * one instruction it costs."
 *
 * CAUSE A -- BASIC-BLOCK ORDER.  Eleven of the fourteen.  The ROM's then-arm is
 * SPLIT IN TWO AROUND THE ELSE-ARM:
 *
 *        cmp  r3, #0
 *        beq  .L46                  <- index 25, ours branched to .L58
 *        movs r3, #0x84             \
 *        lsls r3, #1                |  then-arm, part 1
 *        adds r2, r2, r3            |
 *        ldr  r3, [r5, r2]          |
 *        orrs r3, r1                |
 *        str  r3, [r5, r2]          /
 *        b    .L4a                  <- jumps OVER the else-arm
 *  .L46: movs r0, #0                \  else-arm: return 0
 *        b    .L5e                  /
 *  .L4a: movs r0, #0x8e             \
 *        lsls r0, #1                |
 *        adds r2, r6, r0            |
 *        ldrb r3, [r5, r2]          |  then-arm, part 2
 *        adds r3, #1                |
 *        strb r3, [r5, r2]          |
 *        adds r0, r7, #0            |
 *        bl   Func_8079ae8          /
 *
 * The park's body had all eight of part 2 INSIDE the `if` arm, so gcc emitted
 * one contiguous then-block and then the else-block -- same instruction
 * multiset, hence 51 encodings against 51 and a figure of 14 rather than a
 * length difference.  gcc-2.96 has no -freorder-blocks (that arrives in 3.0),
 * so block order is SOURCE order, and a then-arm split in two around the
 * else-arm is a direct statement that the trailing statements were written
 * AFTER the whole `if/else`, not inside the then-arm.
 *
 * CAUSE B -- the `mov r0,#0x8e` hoist, i.e. the park's entire diagnosis.  It is
 * NOT independent: sched2 in gcc-2.96 is BASIC-BLOCK LOCAL, so once the
 * `(*(u + k))++` statement leaves the then-arm, `mov r0,#0x8e` is in a
 * DIFFERENT basic block from the `orrs`/`str` pair and cannot be hoisted past
 * them at all.  Fixing cause A fixes cause B for free.  No alias-set edge, no
 * priority arithmetic, no pin, no scheduling lever of any kind.
 *
 * THE GENERALISABLE PART:
 *
 *   > A SINGLE CONSTANT APPARENTLY HOISTED ABOVE THE PREVIOUS STATEMENT INSIDE
 *   > AN `if` ARM IS EVIDENCE ABOUT WHICH BASIC BLOCK THAT STATEMENT IS IN, NOT
 *   > ABOUT CONSTANT MOTION.  sched2 cannot cross a block boundary, so a
 *   > cross-statement hoist PROVES the two statements share a block.  Ask where
 *   > the block ends before reaching for a scheduling lever.
 *
 * which is batch 321's "sched2 transposes every argument pair except the last
 * one in its basic block -- so an argument-order residue can be a question
 * about WHERE THE BLOCK ENDS" seen from the other side.
 *
 * WHAT OF THE PARK SURVIVED, AND IS KEPT HERE UNCHANGED.  Its progress record
 * is sound and both of its levers are still load-bearing in this body:
 *   - computing `i` BEFORE the mask `m = 1 << bit`  (the park's 37 -> 32 step);
 *   - `else { return 0; }` rather than a bare early `return 0;`  (its 32 -> 24
 *     step, worth four instructions -- an early return lets gcc hoist
 *     `mov r0,#0` above the compare and jump straight to the epilogue).
 * Also kept: its correction to the batch-142 naming lever -- "naming blocks
 * reassociation only when the compiler cannot see the definition" -- which is
 * why `k = entry * 4` still has to be spelled out rather than named away.
 *
 * -- scratch_elev/b322/G, v3a.c
 */

/* SetDjinni  --  0x0807a2e4
 *
 * Moves one djinni from a unit's "available" set to its "set" set.  The
 * 0x14c-byte unit record (see GetUnit) carries a four-byte availability mask
 * per entry at +0xf8 and the matching equipped mask at +0x108; the per-entry
 * count byte lives at +0x11c.  Func_807a1f8 decides whether the unit may take
 * the djinni at all; a zero from it is returned unchanged, and a djinni that is
 * not in the availability mask returns 0.  Func_8079ae8 rebuilds the unit's
 * derived state afterwards.
 */

extern unsigned char *GetUnit(int id);
extern int Func_807a1f8(int id, int entry, int bit);
extern void Func_8079ae8(int id);

int SetDjinni(int id, int entry, int bit)
{
    unsigned char *u;
    int r;
    int m;
    int k;
    int i;

    u = GetUnit(id);
    r = Func_807a1f8(id, entry, bit);
    if (r != 0) {
        k = entry * 4;
        i = k + 0xf8;
        m = 1 << bit;
        if ((*(int *)(u + i) & m) != 0) {
            k = k + (0x84 << 1);
            *(int *)(u + k) |= m;
        } else {
            return 0;
        }
        k = entry + (0x8e << 1);
        (*(unsigned char *)(u + k))++;
        Func_8079ae8(id);
    }
    return r;
}
