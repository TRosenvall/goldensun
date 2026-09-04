/* OvlFunc_943_200ab7c  --  0x0200ab7c  [asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c.s]
 *
 * NOT MATCHING. Best 2 of 97, LENGTH EXACT. The candidate below is that form.
 *
 * A sibling of src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_b.c
 * (OvlFunc_943_2009b58, elevated the same round) -- same overlay, same
 * `.L5160` field-actor load, same place-actor-then-poke-sprite-field pattern --
 * and every lever from it transferred: the pinned pointer-and-value pair for a
 * halfword store built after a call, the `m = 0xb0; m <<= 8;` callee-saved
 * local reused at three sites, and the one-variable offset-then-value chain for
 * the iwram word.
 *
 * WHAT REMAINS IS TWO INSTRUCTIONS OF INTERLEAVE THAT C CANNOT SPELL:
 *
 *     rom   ldr r3, =0x3001ebc  /  strh r5, [r0, #0x6]  /  mov r2, #0xe0  /  ldr r3, [r3]
 *     ours  strh r5, [r0, #0x6] /  ldr r3, =0x3001ebc   /  mov r2, #0xe0  /  ldr r3, [r3]
 *
 * gcc has SPLIT the global access across the store: the ADDRESS materialisation
 * `ldr r3, =0x3001ebc` is hoisted above the `strh`, while the DEREFERENCE
 * `ldr r3, [r3]` stays below it. In C those two instructions are one
 * expression -- `b = iwram_3001ebc` -- and there is no statement boundary
 * between them to place anything at.
 *
 * MEASURED, four forms:
 *
 *     no barrier                                   7   whole iwram block sinks above the strh
 *     do { } while (0) between them                2   strh held, address load now below it
 *     the store nested inside the pointer's block  7   identical to no barrier
 *     `b = iwram_3001ebc;` before the barrier     50   and 95 lines -- the deref moves too
 *
 * The barrier is exactly one instruction too strong: it holds the `strh` ahead
 * of the iwram chain, which is what is wanted, and in doing so also holds the
 * address load, which is not. THE WALL IS A STATEMENT BOUNDARY AND THE ROM'S
 * SEAM IS INSIDE A STATEMENT.
 *
 * NEXT: the class is "gcc hoisted half of a global access past a store". A
 * handle would need the address and the dereference to be separate source
 * expressions -- which for a `T *g;` global they are not. Whether a different
 * spelling of the global (an array with an index, say, as the gState idiom
 * does) puts the seam somewhere expressible has not been tried, and is the one
 * idea left.
 */
extern int L5160 __asm__(".L5160");
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_943_200b9ec(int a);

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(void *p);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __Func_8091e9c(int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_943_200ab7c(void)
{
    register int m __asm__("r5");

    __CutsceneStart();
    { PIN2; q1 = 0xf; q0 = 0; __Func_8092950(q0, q1); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __LoadFieldActors(&L5160);
    __WaitFrames(1);
    { PIN3; q1 = 0xc4; q2 = 0xfb; q1 <<= 16; q2 <<= 17; q0 = 0x14;
      __MapActor_SetPos(q0, q1, q2); }
    {
        register unsigned char *a __asm__("r0");
        register int v __asm__("r3");
        a = __MapActor_GetActor(0x14);
        v = 0xa0; v <<= 8;
        *(short *)(a + 6) = v;
    }
    { PIN3; q1 = 0xb8; q2 = 0x83; q1 <<= 16; q2 <<= 18; q0 = 0x16;
      __MapActor_SetPos(q0, q1, q2); }
    m = 0xb0;
    m <<= 8;
    *(short *)(__MapActor_GetActor(0x16) + 6) = m;
    { PIN2; q1 = 1; q0 = 0x15; __Func_8092b08(q0, q1); }
    { PIN3; q1 = 0xb8; q2 = 0x9e; q1 <<= 16; q2 <<= 18; q0 = 0x15;
      __MapActor_SetPos(q0, q1, q2); }
    *(short *)(__MapActor_GetActor(0x15) + 6) = m;
    do { } while (0);
    {
        register unsigned char *b __asm__("r3");
        register int v __asm__("r2");
        b = iwram_3001ebc;
        v = 0xe0; v <<= 1;
        b += v;
        v += 0x42;
        *(int *)b = v;
    }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __MapActor_Jump(0x16, 4, 0xa);
    { PIN3; q2 = 0x14; q1 = 6; q0 = 0x16; __MapActor_Jump(q0, q1, q2); }
    __MessageID(0x1ee5);
    OvlFunc_943_200b9ec(0x16);
    __MapActor_DoAnim(0x14, 3);
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0x15; q1 <<= 10; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0xb4; q2 = 0x222; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0x28; q1 = m; q0 = 0x15; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 0x15; __Func_80925cc(q0, q1); }
    OvlFunc_943_200b9ec(0x15);
    __Func_8091e9c(0xf);
}
