/* OvlFunc_940_20083dc  --  0x020083dc
 * ref: asm/overlays/rom_7c5974/ovl_30_c_c_c_c_a_c.s
 *
 * NEEDS ONE LINE IN area.sym:   _AREA_69 = 0x69;
 * With that, the screen is byte-for-byte OK (verified by substituting the
 * already-defined _AREA_6a, which matches every instruction and differs only
 * in the pool word's value).
 *
 * This unparks src/non_matching/overlays/20083dc.c. Two things the park was
 * missing, and the second one is the interesting one:
 *
 *  1. `pop {r1} / bx r1` with a trailing `mov r0, #0` -- the function returns
 *     int, not void. The park's C was one instruction short for that reason.
 *
 *  2. The park's blocker was "gcc derives 0x209 from the 0x1c0 it already has
 *     (`add r2, #0x49`) where the ROM pools 0x209 and derives 0x1c2 from it".
 *     The park concluded 0x209 is not a pool tell (true -- it is past imm8)
 *     and that nothing in the source picks which constant is primary.
 *     What actually decides it is the OTHER constant: 0x69 is stored into a
 *     short and IS a pool tell (`ldr r2, =0x69` where `mov` would do).
 *     Spelled `(int)&_AREA_69` it takes a register the derivation chain wanted,
 *     and gcc pools 0x209 on its own with the literal left alone.
 *     3 differing became 1 (the symbol name) from that one edit.
 */
typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    short f1c4;
    short f1c6;
    unsigned char pad1c8[0x2c0 - 0x1c8];
} GlobalState;

extern unsigned char iwram_3001ebc[];
extern GlobalState gState;
extern int _AREA_69;
extern void *__MapActor_GetActor(int slot);
extern void __ClearFlag(int id);
extern void __Actor_SetSpriteFlags(void *a, int f);

int OvlFunc_940_20083dc(void)
{
    short e;

    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x209;
    e = gState.f1c2;
    if (e == 0xa) {
        __ClearFlag(0x12f);
        gState.f1c4 = (int)&_AREA_69;
        gState.f1c6 = e;
    }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    return 0;
}
