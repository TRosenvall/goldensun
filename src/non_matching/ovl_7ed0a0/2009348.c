/* OvlFunc_964_2009348  [ovl_7ed0a0]
 * Source asm: goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_a.s
 *
 * Blocker class 5, SCHEDULING. Eighteen instructions against eighteen, with
 * two pairs transposed:
 *
 *     rom    mov r0, #0 / sub sp, #0xc ... ldr r1, =0xffe00000 / ldr r3, [r0, #8] / mov r2, sp
 *     ours   sub sp, #0xc / mov r0, #0 ... mov r2, sp / ldr r3, [r0, #8] / ldr r1, =0xffe00000
 *
 * NOTE THIS TU IS BUILT AT -O1, not -O2: the Makefile rule
 * `asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a%.o` covers it. Screened at -O2 this
 * function reports a clean match and then fails the build, which is exactly
 * what happened -- the .c was written and reverted. Always check the Makefile
 * for a per-file rule before trusting a screen on an overlay.
 *
 * The offset must be written as an ADDITION of 0xffe00000, not a subtraction
 * of 0x200000; the ROM loads the negative constant and adds.
 */

struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern struct Actor3 *__MapActor_GetActor(int slot);
extern void OvlFunc_964_2008cd0(int *pos);

/* Builds a copy of slot 0's position on the stack with x lowered by 0x20.0000
 * and passes the triple by address.
 */
void OvlFunc_964_2009348(void)
{
    struct Actor3 *actor = __MapActor_GetActor(0);
    int pos[3];

    pos[0] = actor->x + 0xffe00000;
    pos[1] = actor->y;
    pos[2] = actor->z;
    OvlFunc_964_2008cd0(pos);
}
