/* OvlFunc_950_2008760  [ovl_7d5838]
 *
 * Source asm: goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_c.s
 *
 * NOT SPLIT. The .s still holds both of its functions.
 *
 * A two-outcome conversation: say a line, do something, then say one of two
 * follow-ups depending on a test.
 *
 * Blocker: A CONSTANT THE ROM KEEPS LIVE AND DERIVES FROM. The three message
 * ids are consecutive, and the ROM loads only the FIRST, holding it in a
 * callee-saved register across four calls and computing the others with `add`:
 *
 *     ldr r5, =0x1fbb / mov r0, r5 / bl __MessageID
 *     ... add r0, r5, #1 / bl __MessageID
 *     ... add r0, r5, #2 / bl __MessageID
 *
 * gcc folds `m + 1` and `m + 2` at compile time and emits three independent
 * pool loads, so `m` never becomes a live value and r5 is spent on the actor
 * slot instead. 29 instructions against 30, and the register assignment
 * diverges from instruction zero because the ROM needs r6 as well.
 *
 * THIS IS THE REVERSE OF constant-CSE AND IS NOT THE SAME SHAPE AS THE ONE
 * ALREADY SOLVED. src/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_a_a.c has the ROM
 * caching a zero across calls, and a named local reproduces it -- because
 * nothing there folds. Here a named local does NOT survive, because
 * `0x1fbb + 1` is a constant expression and gcc evaluates it. So "the ROM keeps
 * a value in a callee-saved register" has two sub-cases, and only the
 * non-foldable one has a lever.
 *
 * TRIED:
 *   1. `int m = 0x1fbb;` with `m`, `m + 1`, `m + 2` at the three call sites
 *      (the form below) -- gcc folds all three into separate pool entries
 *
 * What would work is anything that makes the base opaque to constant folding
 * while still emitting a plain `ldr =0x1fbb`. Nothing in the tree does that
 * yet, and inline asm would be a scaffold rather than a match.
 */
extern void __ActorMessage(int actor, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_950_2008760(int slot)
{
    int m;

    __CutsceneStart();
    m = 0x1fbb;
    __MessageID(m);
    __Func_8092c40(slot, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0xa);
        __MessageID(m + 1);
    } else {
        __MessageID(m + 2);
    }
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}
