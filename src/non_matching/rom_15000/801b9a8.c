/* Func_801b9a8 -- asm/rom_15000/rom_1aeec_a_a_c_a_c_c.s
 *
 * COVERS TWO FUNCTIONS. Func_801b9ec (0x0801b9ec) in the same file is this
 * function plus one trailing `bl Func_801c188`; everything else is identical
 * instruction for instruction. Whatever lands this lands both.
 *
 * BLOCKER: a POOLED SMALL CONSTANT -- almost certainly a symbol we cannot
 * name -- plus a register choice. 18 of 35, one line short.
 *
 * Walks n links down a list from a field at +0x348, and if the node's type is
 * 1 or 6 loads a UI icon with two stack out-parameters and a stack fifth
 * argument. The list walk, both type tests, the 0xc-byte frame, both stack
 * stores and the argument setup all reproduce.
 *
 * THE MISSING LINE IS THE CONSTANT:
 *
 *     rom    ldrh r0, [r2, #0x20] / ldr r3, =0x1f / sub r0, r3
 *     ours   ldrh r0, [r0, #0x20] / sub r0, #0x1f
 *
 * 0x1f fits an 8-bit immediate, so `sub rd, #imm8` is available and gcc uses
 * it. The ROM spends a POOL LOAD instead. docs/elevation.md records that
 * Thumb-1 gas does not fold `ldr rX, =imm8` into `mov`, so a pooled small
 * constant is a genuine SYMBOL TELL -- the original subtracted a named value,
 * not a literal.
 *
 * SEARCHED AND NOT FOUND: no `.set`/`.equ` in asm/ or include/ evaluates to
 * 0x1f. The pooled form recurs across at least five other .s files, so it is a
 * shared constant rather than a one-off -- most likely a base id for an icon
 * table. Naming it would close this function AND its sibling; guessing a name
 * would be inventing source, so it is left open deliberately.
 *
 * ALSO REMAINS: the ROM keeps the walked node in r2 and leaves r0 free for the
 * icon id; ours reuses r0 for the node. Allocation, not spelling.
 *
 * MEASURED:
 *   icon id computed inline as the call's first argument   34 lines, 18 differ
 *   icon id named in a local computed BEFORE the 0xc read,
 *     to match the ROM's load order                        34 lines, 20 differ
 *
 * The second is a negative: the ROM does read +0x20 before +0xc, but forcing
 * that order with a named local costs two differences rather than gaining
 * them, because the named id then competes for the register the node wants.
 */
extern void LoadOldUIIcon(int id, int b, int *x, int *y, int flag);

void Func_801b9a8(char *p, int n)
{
    char *node;
    int a;
    int b;
    int t;

    node = *(char **)(p + 0xd2 * 4);
    while (n != 0) {
        n--;
        node = *(char **)(node + 4);
    }
    t = *(unsigned short *)(node + 0xa);
    if (t == 1 || t == 6) {
        a = *(unsigned short *)(node + 0xc);
        LoadOldUIIcon(*(unsigned short *)(node + 0x20) - 0x1f, 0, &a, &b, 1);
    }
}
