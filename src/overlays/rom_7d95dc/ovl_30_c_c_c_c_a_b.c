/* OvlFunc_953_2009c6c  --  0x02009c6c
 *
 * Cut out of goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a.s.
 *
 * Repaints a doorway three ways depending on two save bits: once the story flag
 * is set the door is open, otherwise the actor is moved and a second bit decides
 * whether a smaller patch is drawn.
 *
 * THE THREE CALLS SHOW BOTH HALVES OF THE STACK-ARG RULE IN ONE FUNCTION, which
 * is why this one is worth reading:
 *
 *   __CopyMapTiles takes 2 and 2 in its two stack slots. The ROM writes
 *   `mov r3, #2 / str r3, [sp] / str r3, [sp, #4]` -- ONE register, walked
 *   through both stores -- so the two arguments are plain literals.
 *
 *   Both __Func_8010704 calls take two DIFFERENT values (0x10 and 8; 0xe and
 *   0xb), and the ROM builds each into its own register before storing either:
 *   `mov r3, #0x10 / mov r2, #8 / str r3, [sp] / str r2, [sp, #4]`. Those are
 *   the ones that have to be named locals.
 *
 * So the discriminator is not "does the value repeat across calls" -- batch 94
 * settled that a value living in a callee-saved register proves nothing -- it is
 * whether the two SLOTS hold different values at one call site. Same value:
 * literals. Different values: two locals.
 *
 * Matched on the first screen with the two calls written that way.
 */
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092950(int a, int b);

void OvlFunc_953_2009c6c(void)
{
    int e, f;

    if (__GetFlag(0x95 << 4)) {
        __CopyMapTiles(0x40, 0, 0x30, 5, 2, 2);
        e = 0x10;
        f = 8;
        __Func_8010704(0xe, 8, 2, 1, e, f);
    } else {
        __Func_8092950(0x10, 2);
        if (__GetFlag(0x962)) {
            e = 0xe;
            f = 0xb;
            __Func_8010704(0x1e, 0x16, 1, 2, e, f);
        }
    }
}
