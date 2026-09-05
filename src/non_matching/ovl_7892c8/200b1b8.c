/* OvlFunc_888_200b1b8 -- 0x0200b1b8  [asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_c.s]
 *
 * NOT MATCHING. Best 79 lines against 75, 74 differing -- and THE FOUR EXTRA
 * LINES ARE ENTIRELY ONE EXTRA CALLEE-SAVED REGISTER: two in the prologue
 * (`mov r6, r8 / push {r6, r7}` where the ROM has `push {r7}`) and two in the
 * epilogue. Everything else follows from the resulting shift. The .s holds this
 * function alone, so no layout work is pending.
 *
 * Spawns a debris actor at a randomised offset from a map actor, gives it two
 * randomised timers and a callback, and merges two flag bits into the new
 * actor's sprite byte.
 *
 * BLOCKER CLASS: register allocation -- gcc needs FIVE callee-saved registers
 * where the ROM needs FOUR. Reading the generated assembly (the lever that has
 * twice unblocked this class) shows gcc parking the map actor in `sl` (r10)
 * and paying an extra `mov r1, sl` to use it, while the ROM keeps it in r6.
 *
 * WHAT THE ROM DOES THAT WE DO NOT: it SHARES r5 between two values of
 * different types -- the x coordinate, live across the two `__Random` /
 * `_umodsi3_RAM` pairs before __CreateActor, and then the sprite pointer read
 * out of the new actor afterwards. Four callee-saved registers cover five
 * values because two of them never overlap.
 *
 * MEASURED, six spellings; the length never moved:
 *
 *     v1  baseline, every value a plain named local        79 of 75, 78 differ
 *     v2  drop the redundant `n + 0x55` intermediate       79, 78  (no change)
 *     v3  mask written inline instead of named             79, 75
 *     v4  x and q as two `register` decls both on r5       79, 74  <- below
 *     v5  actor pinned to r6 AND the zero pinned to r8     80, 78  (WORSE)
 *     v6  actor pinned to r6 alone                         80, 78  (WORSE)
 *
 * THE TWO PIN RESULTS ARE THE INFORMATIVE ONES. Pinning the actor to the very
 * register the ROM keeps it in makes the function LONGER. That is the batch 210
 * hazard -- a pin assigned before a call and used after it is dropped -- and it
 * is the same wall recorded on src/non_matching/rom_9000/800bbc0.c: A PIN
 * CANNOT ASK FOR AN ALLOCATION THAT SPANS A CALL.
 *
 * v3 is worth keeping in mind separately: naming the 0xf mask put the CONSTANT
 * in a callee-saved register (`mov r5, #15`) where the ROM uses a scratch, so
 * the named-constant-destination form is not free here. Inlining it recovered
 * three differing lines without changing the length -- the extra register is
 * something else.
 *
 * BATCH 221 -- THE ONE-VARIABLE LEVER DOES NOT TRANSFER ACROSS TYPES, and this
 * is the useful boundary. Batch 220's OvlFunc_956_20093c0 established that
 * writing two logically-distinct values as the SAME C variable is what makes
 * gcc share a register, because gcc-2.96 has no live-range splitting. That is
 * exactly the shape here -- the ROM shares r5 between the coordinate and the
 * sprite pointer -- so it looked like the answer. It is not:
 *
 *     v7  one `int` variable, the byte accesses cast to a pointer   83 of 75, 82
 *     v8  one pointer variable, the coordinate cast at the call     83 of 75, 82
 *
 * BOTH CAST DIRECTIONS PRODUCE BYTE-IDENTICAL OUTPUT, which is itself the
 * finding: to gcc they are the same program, and the casts cost more than the
 * shared register saves. The batch 220 lever works when the two values have the
 * SAME TYPE and one variable is therefore natural; where the types differ, a
 * shared variable has to be bought with casts and the trade is a loss.
 *
 * So the ROM's r5 sharing here is NOT reachable by naming at all -- it is the
 * allocator coalescing two ranges that C cannot express as one object without
 * paying for it. That is a stronger statement than the park's original "next
 * step" and it closes the direction rather than leaving it open.
 *
 * NEXT: the question is which value gcc is keeping alive that the ROM is not.
 * The two-range sharing in v4 is the right shape and did not reduce the count,
 * which means gcc is not honouring it -- reading the generated assembly again
 * AFTER v4, specifically to see what occupies r5 across __CreateActor, is the
 * cheapest next step and was not done. Every attempt here varied HOW VALUES ARE
 * DECLARED; none changed the SET of values that must be live, which is the
 * assumption they all share.
 */
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__CreateActor(int id, int x, int y, int z);
extern void OvlFunc_888_200b144(void);

void OvlFunc_888_200b1b8(int slot)
{
    unsigned char *a;
    unsigned char *n;
    register unsigned char *q __asm__("r5");
    register int x __asm__("r5");
    int y;
    int t;
    int v;
    int z;

    a = __MapActor_GetActor(slot);
    if (a == 0)
        return;
    x = *(int *)(a + 8) + ((__Random() % 0x14) << 16) - 0xa0000;
    y = *(int *)(a + 0xc) + ((0xf & __Random()) << 16) - 0x80000;
    n = __CreateActor(0x8f << 1, x, y, *(int *)(a + 0x10));
    if (n == 0)
        return;
    q = *(unsigned char **)(n + 0x50);
    n[0x55] = 0;
    *(short *)(n + 0x64) = (__Random() % 0xa) + 5;
    z = 0;
    *(short *)(n + 0x66) = (__Random() % 0x3c) + 0x1e;
    *(void **)(n + 0x6c) = OvlFunc_888_200b144;
    q[0x26] = z;
    t = 0xc;
    t &= (*(unsigned char **)(a + 0x50))[9];
    v = -13;
    v &= q[9];
    v |= t;
    q[9] = v;
}
