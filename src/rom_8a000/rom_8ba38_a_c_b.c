/* Func_808d394 -- 0x0808d394, tail of asm/rom_8a000/rom_8ba38_a_c.s (72 insns).
 *
 * VERDICT (tools/objcmp.py, run against BOTH the original asm/ path and a
 * single-function ref cut from it -- the two agree, so no Makefile pattern
 * rule is biting):
 *
 *   OK Func_808d394 -- 148 bytes, 73 encodings and 1 relocations identical
 *
 * FLAG GROUP: this TU needs GCSE_CFLAGS (-fno-gcse). Under plain GCC296_CFLAGS
 * the same source is 73 lines against 80 with 77 differing.
 *
 * WHAT THE SCAN SAID vs WHAT IS HERE. The candidate was picked as "zero
 * interleaved into a shifted build at one straight-line site, zero memory
 * operations". Neither half is true of this function. `mov r1, #0xe0 /
 * lsl r1, #0xb` is not an argument-setup interleave: it is gcc BUILDING
 * 7 << 16 inside a loop, and it is built there rather than hoisted precisely
 * because combine invents it after loop optimisation has run. And the body is
 * nothing but memory operations -- ldmia over a four-entry pointer table and a
 * halfword load per record. It is a nested search loop, not a call script.
 *
 * WHAT IT DOES. iwram_3001ebc (gMapState) points at a block whose first four
 * words are pointers to arrays of 0x18-byte records terminated by id == -1.
 * For slot <= 7 the answer is the record whose id equals the slot; for slot > 7
 * the arrays are walked counting only records with id > 7, numbering them from
 * 8 upward, and the answer is the record whose ordinal equals the slot. Falling
 * out of all four arrays leaves p on a terminator, and the shared tail turns a
 * terminator into a null return -- which is why the ROM re-reads the halfword
 * after the outer loop and jumps INTO that test from both hit sites.
 *
 * THE THREE LEVERS, WITH MECHANISMS.
 *
 * 1. `unsigned short id`, read into an `int`, compared through `(short)`.
 *    The record's first halfword is loaded ZERO-extended (`ldrh`) and
 *    sign-extended per use. thumb has no HImode compare, so `(short)v` expands
 *    via arm.md's `extendhisi2` expander, which for TARGET_THUMB emits an
 *    explicit `ashift`+`ashiftrt` PAIR (arm.md:3187; the `ldrsh` form,
 *    *thumb_extendhisi2_insn at arm.md:3239, is only ever reached later by
 *    combine folding a load into that pair). A `short` field instead loads
 *    sign-extended and collapses the function to 67 lines.
 *    That same expander is why the two comparisons diverge: `(short)v != -1`
 *    compares against a LOOP-HOISTED register, so combine cannot fold the
 *    shift pair into a constant compare and leaves a real sign-extend, while
 *    `(short)v > 7` compares against a literal, so combine rewrites it to
 *    `(v << 16) > 0x70000` and the pre-shifted constant appears. Writing that
 *    shifted compare in the source instead is WRONG -- gcc then treats
 *    0x70000 as loop-invariant and hoists it out (83 lines).
 *
 * 2. -fno-gcse. With gcse on, the redundant-looking pair `ldrsh r3, [r0, r5] /
 *    ldrh r2, [r0]` -- the SAME halfword loaded signed for the terminator test
 *    and unsigned for the carried value -- is exactly what gcse deletes,
 *    deriving the raw back out of the sign-extended copy with an extra shift.
 *    The doc's `-fno-gcse` signature (visibly redundant work) is this pair.
 *
 * 3. `int w = p->id;` -- the SECOND read, in the slot > 7 arm ONLY.
 *    This is docs/elevation.md's "Read a field twice to get the
 *    redundant-looking `mov`", and here the mechanism is nameable. cse.c's
 *    cse_around_loop (cse.c:6252, reached from cse.c:7131) fires only when the
 *    insn before NOTE_INSN_LOOP_END is the back-jump AND its label has
 *    LABEL_NUSES == 1. The slot <= 7 loop ends `bne` + an unconditional `b`
 *    to the outer continue, so the last jump names the WRONG label and
 *    cse_around_loop is skipped -- that arm needs no second read and gets
 *    worse (82 lines) if given one. The slot > 7 loop falls through, so
 *    cse_around_loop runs and rewrites the loop head's first expression into a
 *    copy of a REG_LOOP_TEST_P register (jump.c:1242, set by
 *    duplicate_loop_exit_test). With one variable that first expression is
 *    `v << 16` and the copy is of the SHIFTED value, which then blocks combine
 *    and costs three instructions. Reading the field again makes it
 *    `zero_extend(mem)` instead, so the copy is of the RAW halfword --
 *    the ROM's `mov r1, r2` -- and the loop head keeps its own `lsl`.
 *    Both `w = p->id` sites are load-bearing: dropping the pre-loop one
 *    costs 76 lines / 67 differing.
 *
 * MEASURED WORSE (all at -fno-gcse, 80 ROM lines; "differ" is tryc's count):
 *   short id (signed field)                    67 lines, 77 differ
 *   no w, one variable (the obvious source)     77 lines, 56 differ
 *   short v / unsigned short v instead of int   66 / 70 lines, 78 differ
 *   unsigned short w or short w                 76 lines, 56 differ
 *   w = v instead of w = p->id                  77 lines, 56 differ
 *   w = p->id only inside the loop              76 lines, 67 differ
 *   (short)p->id > 7 (no w at all)              76 lines, 67 differ
 *   the same second read in the slot <= 7 arm   82 lines, 66 differ
 *   (v << 16) > 0x70000 written out             83 lines, 81 differ
 *   goto-form loops with the peel written out   72 lines, 77 differ
 *   default flags (gcse on)                     73 lines, 77 differ
 * MEASURED INERT: declaration order of v and w; unsigned int for both;
 * assigning w before v; and -- see below -- the guard spelling.
 *
 * REFINEMENT to docs/elevation.md's "A table walk's guard: a separate `if`,
 * not a `while`" (NEW here, grepped for first as "field twice", "second read",
 * "duplicate_loop_exit_test"). That entry reads the preheader's
 * `ldrsh` + `ldrh` pair on the SAME halfword as proof the source wrote
 * `if (...) { do { } while (...); }` rather than a `while`, because a `while`
 * lets duplicate_loop_exit_test copy the latch test in and CSE then serves
 * guard and body from one load. This function has that exact preheader pair
 * and the claim does not hold: `while` and `if` + `do`/`while` are BYTE-IDENTICAL
 * here (both OK at -fno-gcse, both 73 lines / 77 differing without it). What
 * restores the pair is the FLAG. Read the pair as a -fno-gcse tell first and
 * only then as a guard-spelling tell; on this function the guard spelling is
 * inert and the flag is the whole difference.
 *
 * Proposed name: GetPartyRecordForSlot (see the annotation on the .s).
 */
struct Ent {
    unsigned short id;
    unsigned char pad_02[0x16];
};

extern unsigned char iwram_3001ebc[];

struct Ent *Func_808d394(int slot)
{
    struct Ent **tbl;
    struct Ent *p;
    int v;
    int w;
    int i;
    int n;

    tbl = *(struct Ent ***)iwram_3001ebc;
    n = 8;
    for (i = 0; i < 4; i++) {
        p = *tbl++;
        if (p != 0) {
            if (slot <= 7) {
                v = p->id;
                while ((short)v != -1) {
                    if ((short)v == slot)
                        goto done;
                    p++;
                    v = p->id;
                }
            } else {
                v = p->id;
                w = p->id;
                while ((short)v != -1) {
                    if ((short)w > 7) {
                        if (n == slot)
                            goto done;
                        n++;
                    }
                    p++;
                    v = p->id;
                    w = p->id;
                }
            }
        }
    }
    v = p->id;
done:
    if ((short)v == -1)
        p = 0;
    return p;
}
