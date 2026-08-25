/* Func_8021b80 -- NOT MATCHING. 12 of 37, and ours is two instructions short.
 *
 * Source asm: goldensun/asm/rom_15000/rom_20198_c_c_c_a_c_c_c_c.s
 *
 * Blocker: the two stack arguments share ONE register in the ROM, each stored
 * immediately after it is built:
 *
 *     mov r1, #0xe / str r1, [sp] / mov r1, #1 / add r2, sp, #0xc
 *     add r3, sp, #8 / str r1, [sp, #4]
 *
 * Two named locals -- the batch-49 form that every other stack-arg pair in this
 * tree wants -- build both values and then store both, which is 12 of 37.
 *
 * A SINGLE LOCAL REUSED GETS TO 6, and that is the useful measurement even
 * though the spelling is not usable: passing `v` and `v - 0xd` halves the count,
 * which confirms the ROM really is walking one register rather than holding two.
 * No honest source form produces that -- the second value is 1, not a function
 * of the first -- so the body below keeps the two-local version.
 *
 * THIS IS THE THIRD DISTINCT STACK-ARG-PAIR SHAPE. Batch 49: two locals, one
 * possibly held in a callee-saved register. Batch 52: both rebuilt inside each
 * arm of an if/else. Here: one register walked through both slots. The pair is
 * not one construct and reading the reference for which registers are held is
 * the only way to tell them apart.
 *
 * The `i > 7` clamp is UNSIGNED (`bls`).
 */
extern int _GetFlag(int id);
extern void LoadPortrait(int a, int b, int *c, int *d, int e, int f);

int Func_8021b80(unsigned int i, int arg)
{
    int a;
    int out;
    int m;
    int n;

    a = arg;
    if (i > 7)
        i = 0;
    if (_GetFlag(0x20)) {
        if (i == 0)
            i = 0x38;
        else if (i == 1)
            i = 0x39;
    }
    m = 0xe;
    n = 1;
    LoadPortrait(i, 0, &a, &out, m, n);
    return out;
}
