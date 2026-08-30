/* Cluster Func_80b6e30..Func_80b6e30 extracted from goldensun/asm/rom_b5000/rom_b5a0c_c_c_c.s.
 *
 * Total .text for this TU = 76 bytes.
 *
 * FROM THE BRANCH-OVER-POOL CLASS. The pool shape caused no trouble at all --
 * gcc emits the `b` over the pool at the ROM's position unaided.
 *
 * WARNING: tools/tryc.py REPORTS 13 DIFFERING ON THIS FUNCTION AND IS WRONG.
 * gcc emits two labels at the same address where the ROM has one, so the
 * streams misalign by a token and everything after reads as differing. The
 * assembled .text is byte-identical, verified with objdump and cmp. Any
 * branch-over-pool function can hit this; compare bytes before believing a
 * diff.
 *
 * THE OFFSET MUST BE DECLARED INSIDE THE LOOP BODY. `move_movables` inserts a
 * hoisted invariant immediately before `loop_start`, so a hoist is ALWAYS last
 * in the preheader -- confirmed across five statement orderings. The ROM has
 * its `mov r5, #4` AFTER the hoist, so that 4 cannot be source-level preheader
 * code; it has to come from `strength_reduce`, which runs later and inserts
 * closer to the loop. Declaring the offset in the body makes it a giv and puts
 * its init on the right side of the hoist. Declared outside as a biv it never
 * can.
 *
 * Also `int` return, not `void` -- that alone gives the ROM's `pop {r1} / bx r1`.
 */
extern char *iwram_3001e74;
extern void _PreloadSpriteGFX(int a, int b, int c, int d);

int Func_80b6e30(int slot)
{
    int p;
    int i;

    p = (int)iwram_3001e74;
    i = 0;
    do {
        int o = 4 + i * 2;
        if (*(short *)(o + p) == slot) {
            _PreloadSpriteGFX(i, 0, 0, 0);
            *(short *)(o + p) = 0;
        }
        i++;
    } while (i <= 5);
}
