/* OvlFunc_911_2008304 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c.s
 * Best screen: 2 differing of 85, streams the same length.
 *
 * BLOCKER CLASS: `mov r0, #0` transposed with an `lsl`.
 *
 *     rom    mov r0, #0 / lsl r2, #7 / bl __MapActor_SetSpeed
 *     ours   lsl r2, #7 / mov r0, #0 / bl __MapActor_SetSpeed
 *
 * This is the argument-move rotation whose usual lever is the callee's declared
 * RETURN TYPE (batch 99), and it is the shape batch 100 found the lever does
 * NOT reach -- r0 against a shift rather than against another `mov`. Declaring
 * __MapActor_SetSpeed `int` changes nothing here, confirming that boundary on a
 * second function.
 *
 * TWO THINGS WERE SOLVED GETTING HERE and both are worth keeping:
 *
 *   THE TWO DEFAULTS ARE ASSIGNED BEFORE THE OPENING CALLS. `a = 0; b = 0;`
 *   written after __CutsceneStart and __PlaySound leaves them in call-used
 *   registers and the function pushes only {r5, lr}; the ROM pushes
 *   {r5, r6, r7, lr}. Moving the two assignments above the calls makes them live
 *   across those calls, which forces callee-saved registers and the ROM's
 *   prologue. 46 differing of 81 to 29 of 83.
 *
 *   CASE 9 RE-READS THE GLOBAL. Every other path uses the pointer cached at the
 *   top, but that arm has `ldr r3, =0x3001ebc / ldr r3, [r3]` of its own.
 *   Writing `iwram_3001ebc` again there rather than reusing the local is what
 *   produces it. 29 differing to 2.
 *
 * The second of those is the general lesson: a cached base being re-read in ONE
 * arm is visible in the assembly and is a statement about the source, not a
 * codegen artifact.
 */
extern char *iwram_3001ebc;
extern unsigned char L2e48[] __asm__(".L2e48");
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void OvlFunc_911_20082b4(int n);

void OvlFunc_911_2008304(void)
{
    char *p;
    char *q;
    int a;
    int b;

    p = iwram_3001ebc;
    a = 0;
    b = 0;
    __CutsceneStart();
    __PlaySound(0x9e);
    switch (*(short *)(p + (0xb6 << 1))) {
    case 5:
        a = 0x47;
        b = 9;
        break;
    case 6:
        a = 0x49;
        b = 0x11;
        break;
    case 7:
        a = 0x50;
        b = 0x15;
        break;
    case 8:
        a = 0x54;
        b = 0xc;
        break;
    case 9:
        q = __MapActor_GetActor(0);
        q[0x55] = 0;
        __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
        __Func_809228c(0, 0, 8);
        *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
        __Func_8091e9c(9);
        __CutsceneEnd();
        return;
    }
    __Func_8010560(L2e48, a, b);
    OvlFunc_911_20082b4(*(short *)(p + (0xb6 << 1)));
    __CutsceneEnd();
}
