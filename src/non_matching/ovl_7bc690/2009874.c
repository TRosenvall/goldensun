/* OvlFunc_933_2009874  [ovl_7bc690]
 * Source asm: goldensun/asm/overlays/rom_7bc690/ovl_4e4_c.s
 *
 * Blocker: arg-interleave, and a case the filter does NOT catch. Nineteen
 * against nineteen, diverging at instruction 4:
 *
 *     rom    mov r1, #0x80 / mov r2, #0x80 / lsl r2, #7 / mov r0, #8 / lsl r1, #8
 *
 * r2's mov/lsl pair is contiguous; r1's is split by THREE instructions. The
 * filter looks two lines back from an lsl for the mov that starts it, so it
 * sees r2's tidy pair and misses r1's spread one.
 *
 * Widening that window is not obviously right -- a two-line window is what
 * makes the check cheap and specific, and a wider one would start flagging
 * ordinary code. Left as a known gap rather than a bad heuristic; the class is
 * already documented and this is another instance of it, not a new problem.
 */
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80921c4(int slot, int a, int b);

void OvlFunc_933_2009874(void)
{
    __MapActor_SetSpeed(8, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(8, 1);
    __Func_80921c4(8, 0xa8, 0x60);
    __MapActor_SetAnim(8, 2);
}
