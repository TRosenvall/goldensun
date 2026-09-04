// fakematch
/* OvlFunc_881_2008a8c  --  0x02008a8c
 *
 * Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a.s.
 *
 * 154 instructions of straight-line cutscene -- a map transition in, three
 * actors walked to the player in turn, a transition out -- exact on the first
 * screen with eighteen pinned call sites and nothing new.
 *
 * THE REPEATED BLOCK IS WRITTEN OUT THREE TIMES, not looped. Each of the three
 * actors gets the identical sequence: set anim, fetch actor 8, travel to its
 * position if non-null, wait, zero the position, wait. Only the slot number
 * changes (0xc, 0xb, 0). A loop over a slot table would be shorter and would
 * have no way to express that the ROM emits the three copies with slightly
 * different register orders -- this is the fourth batch running where a
 * transcribed sequence beat a loop for that reason.
 *
 * `0x49c0000` is passed to two __MapActor_SetPos calls and the ROM reloads it
 * from the pool at each; the r2 pins force that, since a value in a
 * call-clobbered register cannot survive the `bl`.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __SetFlag(int id);
extern void __Func_808c44c(void);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int a);
extern void __Func_8092adc(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_881_2008a8c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_808c44c();
    __SetFlag(0x94f);
    { PIN3; q1 = 0xb7; q0 = 0xb; q1 <<= 21; q2 = 0x49c0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x18; q2 = 8; q0 = 0xb; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(0xb);
    __CutsceneWait(0x3c);
    { PIN3; q1 = 0xb7; q0 = 0xc; q1 <<= 21; q2 = 0x49c0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc; q2 = 0x18; q0 = 0xc; __Func_809228c(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0xa0; q0 = 0xb; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q2 = 0; q1 <<= 8; q0 = 0xc; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0xb, 3);
    { PIN2; q1 = 3; q0 = 0xc; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0x78);
    { PIN3; q2 = 0x97; q1 = 0x16f80000; q2 <<= 19; q0 = 8;
      __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0xc, 2);
    p = __MapActor_GetActor(8);
    if (p != 0)
        __MapActor_TravelTo(0xc, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0xc);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xc; __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0xb, 2);
    p = __MapActor_GetActor(8);
    if (p != 0)
        __MapActor_TravelTo(0xb, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0xb);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xb; __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0, 2);
    p = __MapActor_GetActor(8);
    if (p != 0)
        __MapActor_TravelTo(0, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0);
    { PIN3; q1 = 0; q2 = 0; q0 = 0; __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 8; q2 <<= 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x38; q2 = 8; q0 = 8; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    { PIN3; q1 = 0x28; q2 = 0x28; q0 = 8; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    { PIN3; q1 = 8; q2 = 0x58; q0 = 8; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    __MapTransitionOut();
    __Func_8091e9c(0x6c);
    __CutsceneEnd();
}
