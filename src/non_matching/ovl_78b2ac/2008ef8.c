/* OvlFunc_890_2008ef8 (0x02008ef8) -- NON-MATCHING.
 * COVERS THREE FUNCTIONS. OvlFunc_890_200901c and OvlFunc_890_2009140 in the
 * same .s are byte-identical to this one except for the state variable they
 * read (.L2de0 and .L2dec against .L2ddc) and the label names. Solving any one
 * elevates all three.
 *
 * Blocker class: STACK-ARGUMENT MATERIALISATION -- the ROM computes both
 * stack-passed arguments before storing either; gcc stores each as it computes
 * it and reuses one register.
 *
 * 140 lines against 140, NINE differing, in three places and two shapes.
 *
 * SHAPE 1, six of the nine, at the two call sites where BOTH stack arguments
 * are literals:
 *
 *     rom    mov r3, #0x1 / mov r2, #0x5 / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0x1 / str r3, [sp] / mov r3, #0x5 / str r3, [sp, #4]
 *
 * THE OTHER FOUR CALL SITES MATCH, and the difference is what the first stack
 * argument is. In cases 1-4 it is the switch variable, already live in r5, so
 * gcc stores it directly and needs a register only for the second -- which is
 * the ROM's shape by accident. When both are literals gcc reuses one register
 * and the ROM does not.
 *
 * This is the third specimen: src/non_matching/ovl_7b9cb4/200a6c0.c has six
 * sites of it, and records the same discriminator from the other side (there,
 * the one site where the two stack values COINCIDE matches). Naming it:
 *
 *   **STACK-ARGUMENT MATERIALISATION. When a call passes two or more arguments
 *   on the stack and the ROM materialises them all before storing any, gcc will
 *   not follow unless something else already holds one of them in a register.**
 *
 * gcc accumulates outgoing arguments (the `sub sp, #8` is in both) and emits a
 * move-then-store per argument. The original build evaluated all arguments into
 * pseudos first. No source spelling reaches it.
 *
 * SHAPE 2, three of the nine, at `L2de8 = 2;`:
 *
 *     rom    ldr r2, =L2de8 / mov r3, #0x2 / str r3, [r2]
 *     ours   ldr r3, =L2de8 / mov r2, #0x2 / str r2, [r3]
 *
 * Which register receives the address and which the value. Ordinary allocation.
 *
 * MEASURED (rom 140 lines, all at exact length unless noted):
 *   `unsigned int s` for the switch variable          140, 11 -- the switch
 *                          dispatch comes out `bhi`/`bls` where the ROM has
 *                          `bgt`/`blt`
 *   `int s`, with __Random() declared unsigned so the
 *     FINAL comparison is still unsigned              140, 9  <- best
 *   the first stack literal named in the entry block  137, 136 (much worse)
 *   the second stack literal named in the entry block 130, 132 (much worse)
 *   __CopyMapTiles declared to return int             140, 25 (worse)
 *   `p = &L2de8; *p = 2;`                             140, 9  (inert)
 *   -fno-schedule-insns / -fno-defer-pop /
 *     -fno-strength-reduce                            140, 9  (inert)
 *   -fno-gcse                                         141, 31 (worse)
 *   -fno-schedule-insns2                              145, 103 (worse)
 *
 * THE SIGNEDNESS SPLIT IS THE REUSABLE FINDING. The switch on `s` needs SIGNED
 * comparisons (`bgt`/`blt`) and the final `if (s > ...)` needs an UNSIGNED one
 * (`bls`). Declaring `s` as `int` and `__Random()` as returning `unsigned int`
 * gives both: the switch compares an int, and the final comparison is promoted
 * to unsigned by its other operand. Declaring `s` unsigned makes the switch
 * wrong; declaring __Random() signed would make the `>> 16` an `asr`.
 *
 * WHAT IS ELSE IS RIGHT: the comparison-tree dispatch (0-4 and 0x5a is too
 * sparse for a jump table and gcc builds the tree by itself); the switch
 * variable REASSIGNED to 1 inside cases 2, 3 and 4, which is what the ROM's
 * `mov r5, #1` in those arms is; passing `s` itself as a stack argument in
 * cases 1-4; and the pooled zero for the final halfword store.
 *
 * NEXT: nothing source-level in nine probes.
 */
extern unsigned short L2ddc __asm__(".L2ddc");
extern int L2de8 __asm__(".L2de8");
extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_890_2008ef8(void)
{
    int s;

    if ((__Random() & 3) == 0)
        return;
    s = L2ddc;
    switch (s) {
    case 0:
        __PlaySound(0xbb);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x21, 1, 5);
        break;
    case 1:
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x21, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x22, s, 5);
        break;
    case 2:
        s = 1;
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x22, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x23, s, 5);
        break;
    case 3:
        s = 1;
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x23, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x24, s, 5);
        break;
    case 4:
        L2de8 = 2;
        s = 1;
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x24, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x25, s, 5);
        break;
    case 0x5a:
        __CopyMapTiles(0x2f, 0x31, 0x2a, 0x21, 1, 0xa);
        break;
    }
    L2ddc = L2ddc + 1;
    s = L2ddc;
    if (s > ((__Random() * 40) >> 16) + 0x64)
        L2ddc = 0;
}
