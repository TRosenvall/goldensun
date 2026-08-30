/*
 * OvlFunc_898_2008acc -- asm/overlays/rom_793768/ovl_314_c_c_a_c_c_c_a_a_c.s
 *
 * BLOCKER: scheduling. 44 lines against 44, SIX differing, and the whole
 * disagreement is where one halfword load sits:
 *
 *      rom   mov r8, r2 / ldr r3, =0x2 / ldrh r2, [r5]
 *      ours  ldrh r3, [r5] / mov r8, r2 / ldr r2, =0x2
 *
 * The ROM saves the preserved field into its callee-saved register and loads
 * the pooled constant before touching the flags halfword; we load the halfword
 * two slots earlier.
 *
 * TRIED AND REJECTED:
 *
 *   * Assigning the flags pointer before reading the preserved field. WORSE --
 *     13 differing.
 *   * `*f = *f | 2;` instead of `*f |= 2;`. Byte-identical.
 *
 * SETTLED, and it is the reason the pooled 2 needs no symbol:
 *
 *   `ldr r3, =0x2` looks like the symbol tell -- 2 is an eight-bit mov -- but
 *   it is the HALFWORD EXCEPTION instead. The constant is ORed with an
 *   `unsigned short` lvalue, which makes it a HImode operand, and gcc pools it
 *   from the plain literal. const.sym's header records the same behaviour for
 *   0xc in OvlFunc_881_200b8fc. Check for a halfword before reaching for a
 *   symbol; that check is what kept a spurious _CONST_2 out of const.sym.
 *
 *   Note the AND at the end takes `mov r3, #1` with no pool, because 1 is
 *   ANDed rather than ORed -- gcc narrows the mask to the halfword and builds
 *   it cheaply. Same function, both directions of the exception.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);
extern void __WaitFrames(int n);

void OvlFunc_898_2008acc(void)
{
    unsigned char *e;
    unsigned short *f;
    int saved;

    e = __MapActor_GetActor(0xf);
    saved = *(short *)(e + 6);
    f = (unsigned short *)(e + 0x64);
    *f |= 2;
    __CutsceneStart();
    __MessageID(0x133b);
    __MapActor_SetAnim(0xf, 0);
    OvlFunc_898_200973c(0xf, 0, 2);
    OvlFunc_898_2009724(0xf, 0xa);
    *(short *)(e + 6) = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *f &= 1;
}
