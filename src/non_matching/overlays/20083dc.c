/* OvlFunc_940_20083dc  --  0x020083dc, asm/overlays/rom_7c5974/ovl_30_c_c_c_c_a_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_7c5974/ovl_30_c_c_c_c_a_c.s
 *
 * BLOCKER CLASS: constant derivation, in the wrong DIRECTION.
 * Status: 42 lines against 43.
 *
 * Two constants sit 0x47 apart -- 0x209, the scene-step value stored into
 * iwram, and 0x1c2, the gState member offset read straight after. Both
 * compilers noticed and derived one from the other, and they picked opposite
 * ends:
 *
 *     rom    mov r1, #0xe0 / lsl r1, #1        (0x1c0, for the iwram offset)
 *            ldr r2, =0x209                    (pooled)
 *            ... str r2 ... sub r2, #0x47      (0x1c2, derived FROM 0x209)
 *
 *     ours   mov r2, #0xe0 / lsl r2, #1        (0x1c0)
 *            add r2, #0x49                     (0x209, derived FROM 0x1c0)
 *            ... str r2 ... sub r2, #0x47      (0x1c2)
 *
 * gcc had 0x1c0 in a register already and built 0x209 from it in one
 * instruction, which is cheaper than a pool load; the ROM pooled 0x209 and
 * derived the offset from that instead.
 *
 * NOT A POOL TELL, so no symbol is invented: 0x209 is past the eight-bit
 * immediate range, so pooling it says nothing about the source. The docs' bar
 * for const.sym is not met and this is left alone.
 *
 * Nothing in the source expresses which of two related constants gcc should
 * treat as primary -- both are plain literals in different statements, and the
 * relationship between them is the optimiser's discovery in both compilers.
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
extern void *__MapActor_GetActor(int slot);
extern void __ClearFlag(int id);
extern void __Actor_SetSpriteFlags(void *a, int f);

void OvlFunc_940_20083dc(void)
{
    short e;

    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x209;
    e = gState.f1c2;
    if (e == 0xa) {
        __ClearFlag(0x12f);
        gState.f1c4 = 0x69;
        gState.f1c6 = e;
    }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
}
