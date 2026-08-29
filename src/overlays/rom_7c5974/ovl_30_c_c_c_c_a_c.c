/* OvlFunc_940_20083dc
 *
 * Cut out of goldensun/asm//overlays/rom_7c5974/ovl_30_c_c_c_c_a_c.s.
 *
 * UNPARKS src/non_matching/overlays/20083dc.c.
 *
 * That park was on gcc derives 0x209 from the 0x1c0 it already has where the
 * ROM pools 0x209, and concluded nothing in the source picks which constant is
 * primary. The fix was the OTHER constant: 0x69 is a genuine pool tell, and
 * spelling it `(int)&_AREA_69` takes the register the derivation chain wanted
 * -- gcc then pools 0x209 with the literal left alone.
 *
 * The second fix is the return type: the ROMs `pop {r1} / bx r1` means `int`
 * with `return 0;`. The park predates that rule.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
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
