/* OvlFunc_923_2008d58  --  0x02008d58, cut from
 * goldensun/asm/overlays/rom_7aa430/ovl_314_c.s.
 *
 * The .s held this function and a trailing data blob; the blob keeps its slot
 * as ovl_314_c_c.o and this file takes the first .text line.
 *
 * Turn toward the player, unless the player is too far along z: above a
 * threshold it stops facing and, if it is not already at 0xc000, hands off to
 * __Func_8092adc.
 *
 * MATCHED ON THE FIRST SCREEN. The one reading worth recording is that
 * 0xc000 IS THE SAME CONSTANT IN BOTH PLACES. The ROM builds it once,
 *
 *     mov r1, #0xc0 / lsl r1, #8 / cmp r3, r1 / mov r0, #3 / mov r2, #0 / bl
 *
 * and leaves it in r1 as the callee's second argument -- so the compare and the
 * argument are one value CSE'd, not a coincidence. Writing a different literal
 * in either place costs two instructions.
 */
struct E {
    unsigned char pad00[6];
    unsigned short f6;
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct E *__MapActor_GetActor(int slot);
extern int __atan2(int dz, int dx);
extern void __Func_8092adc(int a, int b, int c);

int OvlFunc_923_2008d58(struct E *e)
{
    struct E *pl;

    pl = __MapActor_GetActor(0);
    if ((pl->z >> 19) <= 0x16) {
        e->f6 = __atan2(pl->z - e->z, pl->x - e->x);
    } else if (e->f6 != (0xc0 << 8)) {
        __Func_8092adc(3, 0xc0 << 8, 0);
    }
    return 0;
}
