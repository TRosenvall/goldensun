/* Cluster OvlFunc_932_200a9dc..OvlFunc_932_200a9dc extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c.s.
 *
 * Total .text for this TU = 52 bytes (= 0x0034).
 * Preserves the original ROM layout when slotted after
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a.o in goldensun/overlays/rom_7b9cb4/overlay.ld.
 * The target was the LAST of six functions, so there is no _c part.
 *
 * WAS PARKED, and the park concluded "NEXT: nothing.  This is the documented
 * limit of the lever rather than a new shape."  That was wrong, and the way it
 * was wrong is worth keeping.
 *
 * The park tried naming the SLOT -- the interleaved `mov r0, #9` -- and found
 * that the slot is used twice, once before the `if` and once inside it, so
 * naming it puts a use in the assignment's own block and the lever's third
 * clause forbids it.  All true, and all beside the point: the fix is to name the
 * two SPLIT BUILDS, x and y, in the block dominating the guarded call, and leave
 * the slot alone as a literal.  That is exact.
 *
 *      rom   mov r1,#0xb8 / mov r2,#0xa4 / mov r0,#9 / lsl r1,#16 / lsl r2,#17
 *
 * The lever moves the argument you DO NOT name.  Naming x and y is what frees
 * gcc to slot `mov r0, #9` between the movs and the shifts; trying to name the
 * thing you want moved is what fails.  See docs/elevation.md.
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
    int x, y;

    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    p = (unsigned char *)&gState;
    off = 0xe1;
    off <<= 1;
    p += off;
    off = 0;
    v = *(short *)(p + off);
    x = 0xb8 << 16;
    y = 0xa4 << 17;
    if (v == 2)
        __MapActor_SetPos(9, x, y);
}
