/* OvlFunc_926_200a6d8  --  0x0200a6d8
 *
 * Cut out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_a_b.s.
 *
 * The escape cutscene: start a background task, walk the player out, stop the
 * task. Matched on the first screen, straight-line, with no named locals at
 * all -- worth recording because most functions this size need at least one
 * lever.
 *
 * Drafted by a parallel screening agent and re-screened here before wiring.
 */
extern unsigned char *iwram_3001ebc;

extern void OvlFunc_926_200a5b8(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __StartTask(void (*fn)(void), int prio);
extern void __StopTask(void (*fn)(void));
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapTransitionOut(void);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8092950(int a, int b);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int n);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);

void OvlFunc_926_200a6d8(void)
{
    __CutsceneStart();
    __StartTask(OvlFunc_926_200a5b8, 0xc8 << 4);
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x3c;
    __MapTransitionOut();
    __PlaySound(0x9a);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, -6);
    __MapActor_WaitMovement(0);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __StopTask(OvlFunc_926_200a5b8);
    __WaitMapTransition();
    __Func_8091e9c(3);
    __CutsceneEnd();
}
