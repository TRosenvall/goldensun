/* Cluster Func_8021b80..Func_8021b80 extracted from goldensun/asm/rom_15000/rom_20198_c_c_c_a_c_c_c_c.s.
 *
 * Total .text for this TU = 72 bytes (= 0x48).
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_20198_c_c_c_a_c_c_c_b.o and asm/rom_15000/rom_20198_c_c_c_b.o in
 * goldensun/stage1.ld.
 *
 * MATCHING -- 0 of 34.  72 bytes against 72, 34 encodings against 34,
 * 2 relocations identical.  (Park src/non_matching/rom_15000/rom_21b80.c read
 * 22 of 34 with ref 34 / ours 32 and 72 bytes against 68.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_20198_c_c_c_a_c_c_c_c.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_c_c_c_c.s --func Func_8021b80
 *
 * SPLIT: none.  asm/rom_15000/rom_20198_c_c_c_a_c_c_c_c.s holds ONE function
 * (one .thumb_func_start) and tools/datacheck.py reports no data section and no
 * required exports, so the whole .s is replaced by this .c.
 * PINS: 0 (tools/shimcount.py reports none).  No fakematch row.
 *
 * ================== WHAT THE PARK'S 22 DECOMPOSED INTO =======================
 * Two independent causes, and the first one HID the second.
 *
 *  1. THE IF/ELSE-IF CHAIN IS A `switch`, and that is the missing two
 *     instructions.  The ROM's middle block is
 *         cmp r5,#0 / beq .A / cmp r5,#1 / beq .B / b .END
 *         .A: mov r5,#0x38 / b .END
 *         .B: mov r5,#0x39
 *     -- nine instructions: the whole comparison chain FIRST, then the case
 *     bodies in source order, each but the last ending in a jump to the end.
 *     That is `expand_end_case`'s compare-chain layout for a switch too small
 *     to table.  An `if (i == 0) i = 0x38; else if (i == 1) i = 0x39;` is seven:
 *     gcc interleaves test and body (`bne` past each body), so there is no
 *     trailing default jump and no `b` out of the first arm.  Swapping in the
 *     switch took the figure from 22 of 34 (counts 34 vs 32, 72 bytes vs 68)
 *     to 7 of 34 (counts 34 vs 34, 72 bytes vs 72).
 *
 *  2. THE TWO STACK ARGUMENTS ARE LITERALS, NOT NAMED LOCALS -- the remaining
 *     seven, all of them one consecutive run in the argument-setup block.  The
 *     ROM walks ONE register through both outgoing slots:
 *         mov r1,#0xe / str r1,[sp] / mov r1,#1 / add r2,sp,#0xc
 *         add r3,sp,#8 / str r1,[sp,#4]
 *     With `m = 0xe; n = 1;` named above the call, both pseudos are live at the
 *     same time, so local-alloc gives them r1 and r0 and both `mov`s precede
 *     both `str`s.  Passed as literals, `store_one_arg` is called once per
 *     argument and expands the value in place, so the first constant is dead
 *     before the second is built and both get r1.  sched2 then lifts the two
 *     `add rN,sp,#X` into the gap after `mov r1,#1`, which is the ROM's order.
 *
 * THE PARK'S DIAGNOSIS IS REFUTED, and this is the useful part.  It concluded
 * "No honest source form produces that -- the second value is 1, not a function
 * of the first -- so the body below keeps the two-local version", after
 * measuring that a single local reused got to 6.  The honest source form is
 * plain literals; it was invisible because the switch defect made the stream two
 * instructions short, and on a different-length stream the positional figure for
 * the argument block measures misalignment rather than register choice.
 * Decomposing the diff into runs and fixing the LENGTH run first is what made
 * the second run readable.
 *
 * Also refuted: "THIS IS THE THIRD DISTINCT STACK-ARG-PAIR SHAPE ... one
 * register walked through both slots".  It is not a fourth construct; it is what
 * two literal stack arguments always compile to.
 *
 * The `i > 7` clamp is UNSIGNED (`bls`), which is why the parameter is
 * `unsigned int`.  (Park's observation, reproduced.)
 */
extern int _GetFlag(int id);
extern void LoadPortrait(int a, int b, int *c, int *d, int e, int f);

int Func_8021b80(unsigned int i, int arg)
{
    int a;
    int out;

    a = arg;
    if (i > 7)
        i = 0;
    if (_GetFlag(0x20)) {
        switch (i) {
        case 0:
            i = 0x38;
            break;
        case 1:
            i = 0x39;
            break;
        }
    }
    LoadPortrait(i, 0, &a, &out, 0xe, 1);
    return out;
}
