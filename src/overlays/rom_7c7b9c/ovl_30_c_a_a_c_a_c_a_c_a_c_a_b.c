// fakematch
/* OvlFunc_943_2009b58  --  0x02009b58
 *
 * Was the whole of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_b.s;
 * tools/split_s.py confirmed one function and no data tail, so it converts
 * directly.
 *
 * Map setup: load a field-actor table, place four actors, poke two sprite
 * fields, hand off. Sixty-seven instructions.
 *
 * A VALUE STORED AFTER A CALL MUST BE BUILT AFTER IT, and getting that wrong
 * costs a pushed register. The ROM does
 *
 *     mov r0, #0x14 / bl __MapActor_GetActor / mov r3, #0xc0 / lsl r3, #6 /
 *     ... / strh r3, [r0, #6]
 *
 * -- the value is built in r3, which is call-clobbered, so it CANNOT have been
 * computed before the call. Written with `v = 0xc0; v <<= 6;` as statements
 * ahead of the store, gcc has to keep v across the `bl` and takes a
 * callee-saved register for it: the prologue gains `push {r5, lr}` and 21 of 67
 * differ. Writing the shifted constant INLINE in the store instead
 * (`*(short *)(GetActor(0x14) + 6) = 0xc0 << 6`) moves the build after the call
 * but sends it to the POOL, `ldr r3, =0x3000`.
 *
 * What reaches it is naming both operands with the pointer pinned to the return
 * register:
 *
 *     register unsigned char *a __asm__("r0");
 *     register int v __asm__("r3");
 *     a = __MapActor_GetActor(0x14);
 *     v = 0xc0; v <<= 8;
 *     *(short *)(a + 6) = v;
 *
 * The pointer pin is safe because it is assigned FROM the call rather than
 * before it -- nothing crosses a `bl` -- which is the condition the batch-210
 * hazard turns on. Compare src/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a_b.c, where
 * the inline form DID give mov+lsl: there the store target was an `int` field,
 * here it is a halfword, and the store-width pooling rule decides.
 *
 * THE CROSSED FILL TOOK BATCH 212'S BARRIER-FREE CURE. The first
 * __MapActor_SetPos has movs r1, r2 against shifts r2, r1. Writing the shifts
 * in the ROM's SHIFT order leaves the movs slaved and transposed; writing them
 * in the ROM's MOV order -- `q1 = 0xe8; q2 = 0x9f; q0 = 0; q1 <<= 16;
 * q2 <<= 18;` -- gives both correctly with no volatile asm. Second function to
 * close that way, and it wanted the third argument's `mov` placed between the
 * two other movs and the two shifts, exactly as the listing shows.
 *
 * The field-actor table is reached with the tree's asm-renamed extern, since C
 * cannot spell a name beginning with a dot.
 */
extern int L5160 __asm__(".L5160");
extern void OvlFunc_943_2009c14(int a, int b);

extern void __CutsceneStart(void);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(void *p);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetIdle(int slot);
extern void __SetCameraTarget(int a, int b);
extern void __Func_800fe9c(void);
extern void __Func_8092950(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_943_2009b58(void)
{
    __CutsceneStart();
    __LoadFieldActors(&L5160);
    __WaitFrames(1);
    { PIN3; q1 = 0xe8; q2 = 0x9f; q0 = 0; q1 <<= 16; q2 <<= 18;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN2; q1 = 0xf; q0 = 0; __Func_8092950(q0, q1); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    { PIN2; q1 = 0; q0 = 0; __SetCameraTarget(q0, q1); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapActor_SetIdle(0x16);
    __MapActor_SetIdle(0x15);
    __WaitFrames(1);
    __MapActor_SetPos(0x16, 0, 0);
    __MapActor_SetPos(0x15, 0, 0);
    { PIN3; q1 = 0; q2 = 0; q0 = 0x14; __MapActor_SetPos(q0, q1, q2); }
    {
        register unsigned char *a __asm__("r0");
        register int v __asm__("r3");
        a = __MapActor_GetActor(0x14);
        v = 0xc0; v <<= 6;
        *(short *)(a + 6) = v;
    }
    { PIN3; q1 = 0xe8; q1 <<= 16; q2 = 0x28a0000; q0 = 0x17;
      __MapActor_SetPos(q0, q1, q2); }
    {
        register unsigned char *a __asm__("r0");
        register int v __asm__("r3");
        a = __MapActor_GetActor(0x17);
        v = 0xb0; v <<= 8;
        *(short *)(a + 6) = v;
    }
    __WaitFrames(1);
    OvlFunc_943_2009c14(0x14, 0x17);
}
