/* OvlFunc_932_200a9dc -- NOT MATCHING. 2 of 22, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a.s
 *
 * Blocker: arg-interleave that the basic-block lever cannot reach.
 *
 *     rom    mov r1,#0xb8 / mov r2,#0xa4 / mov r0,#9 / lsl r1,#16 / lsl r2,#17
 *     ours   ... mov r0,#9 emitted after both shifts
 *
 * The call IS inside an `if`, so the lever's conditions look satisfiable, and
 * per batch 57 the plain form was screened FIRST -- it is 2 of 22, and the
 * lever leaves it at 2.
 *
 * WHY IT CANNOT REACH IT: the constant 9 is the slot, and the slot is used
 * TWICE -- once for the __MapActor_GetActor before the `if`, once for the
 * __MapActor_SetPos inside it. Naming it puts a use in the assignment's own
 * block, which is the third clause added in batch 44: every repeated use must
 * be in a different block from the assignment. Keeping the literal for the
 * first call and the local only for the second changes nothing either, because
 * gcc re-merges them.
 *
 * NEXT: nothing. This is the documented limit of the lever rather than a new
 * shape.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_932_200a9dc(void)
{
    unsigned char *p;
    unsigned int off;
    short v;

    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    p = (unsigned char *)&gState;
    off = 0xe1;
    off <<= 1;
    p += off;
    off = 0;
    v = *(short *)(p + off);
    if (v == 2)
        __MapActor_SetPos(9, 0xb8 << 16, 0xa4 << 17);
}
