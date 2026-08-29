/* OvlFunc_911_2008304  --  0x02008304
 *
 * Cut out of goldensun/asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c.s.
 *
 * Opens a cutscene, picks a message position by scene id, and prints one line
 * -- except for scene 9, which drives an actor instead and leaves early.
 *
 * PARKED IN BATCH 104 AT 2 OF 85 on `mov r0, #0` transposed with an `lsl`.
 * The BASIC-BLOCK LEVER closes it: the two `__MapActor_SetSpeed` arguments are
 * assigned to locals before the switch and used inside case 9, so gcc
 * rematerialises them at the call with `mov r0, #0` in the gap. The park had
 * this recorded as the shape the RETURN TYPE lever does not reach, which was
 * true and beside the point -- the lever is not a call-site property.
 *
 * Two readings from the original attempt still hold and both were needed:
 *
 *   THE TWO DEFAULTS ARE ASSIGNED BEFORE THE OPENING CALLS. Written after
 *   __CutsceneStart and __PlaySound they stay in call-used registers and the
 *   function pushes only {r5, lr}; the ROM pushes {r5, r6, r7, lr}. Above the
 *   calls they are live across them, which forces callee-saved registers.
 *   46 differing of 81 to 29 of 83.
 *
 *   CASE 9 RE-READS THE GLOBAL. Every other path uses the pointer cached at
 *   the top; that arm has `ldr r3, =0x3001ebc / ldr r3, [r3]` of its own.
 *   Writing `iwram_3001ebc` again there rather than reusing the local is what
 *   produces it. 29 differing to 2. A cached base re-read in ONE arm is
 *   visible in the assembly and is a statement about the source.
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
    int sx;
    int sy;

    p = iwram_3001ebc;
    a = 0;
    b = 0;
    sx = 0x80 << 8;
    sy = 0x80 << 7;
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
        __MapActor_SetSpeed(0, sx, sy);
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
