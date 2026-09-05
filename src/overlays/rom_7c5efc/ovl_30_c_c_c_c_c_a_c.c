// fakematch
/* OvlFunc_941_2009760  --  0x02009760
 *
 * 266 instructions of cutscene, the second half of the scene its sibling
 * OvlFunc_941_2009448 opens. Byte-exact: 684 bytes, 265 encodings and 76
 * relocations identical.
 *
 * FIFTEEN PINS. The prologue is `push {r5, lr}`, so the ROM keeps exactly one
 * callee-saved value -- the message base -- and rebuilds every other repeated
 * constant at every use. 0xa0<<7 appears six times, 0x80<<7 four, 0x84<<2
 * twice, 0xa0<<2 three; plain C commons each into a pseudo straddling calls
 * and comes out with push {r5,r6,r7,lr} plus an r8 spill, 264 lines and 254
 * differing.
 *
 * `do { } while (0)` BEFORE THE MESSAGE BASE, as in the sibling. sched2 hoists
 * `ldr r5, =0x2558` six slots to the top of the function -- it is the only
 * insn with no anti-dependence holding it down, r5 being the sole call-saved
 * value. 6 differing -> 0. Confirmed again that a bare label, `if (0) ;` and a
 * bare `;` are all INERT while `while (0) ;` is byte-identical: it is the loop
 * note, not label-shifting.
 *
 * THE ONE-STATEMENT FILL IS WORSE HERE, AND THAT IS THE FINDING. On
 * OvlFunc_955_20096d4, writing each argument as a single statement
 * (`q1 = 0xa0 << 7;`) removed nine scheduling barriers, and it went into the
 * doc as a thing to try before reaching for one. On this function the same
 * spelling measures 25 differing. The reason is visible in the ROM: it emits
 * the `lsl` BETWEEN the movs at most sites, and only per-instruction
 * statements can express that -- moving the shift to the end of every fill
 * costs 14. So the two spellings are not ranked; they encode different fills,
 * and which one is right is read off the ROM.
 *
 * PIN MINIMISATION, and this is the clearest measurement of the set rule yet.
 * Nineteen sites screened exact. Stripping one at a time found six inert, but
 * dropping all six fails; every 5-subset fails, 11 of 15 4-subsets fail, 8 of
 * 20 3-subsets fail, and EXACTLY TWO 2-subsets fail -- both of them pairs of
 * sites sharing one value. So the incompatible pairs are the same-value
 * chains, one of each must survive, maximum joint removal is four, and the
 * fifteen that remain were re-stripped individually with every one
 * load-bearing. Swapping either pair's members gives an equally valid set.
 *
 * The only pool word is 0x2558 and the reference object carries no relocation
 * for it, so it is a literal and needs no message.sym entry. No wildcard
 * captures this object; tree default -O2.
 */
extern void __Func_8092adc(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_809280c(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809218c(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __Func_8093500(int a, int b);
extern void __MapTransitionOut(void);
extern void __Func_8091e9c(int a);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_941_2009760(void)
{
    unsigned char *p;
    int m;

    { PIN3; q1 = 0xa0; q2 = 0; q1 <<= 7; q0 = 1; __Func_8092adc(q0, q1, q2); }
    do { } while (0);
    m = 0x2558;
    __MessageID(m);
    __ActorMessage(1, 0);
    __MapActor_DoAnim(2, 3);
    __MessageID(m + 1);
    __ActorMessage(2, 0);
    __Func_809280c(0xd, 2, 0);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x14);
    __MessageID(m + 2);
    __ActorMessage(0xd, 0);
    { PIN3; q1 = 0xc0; q2 = 0; q0 = 0xc; q1 <<= 6; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xc, 3);
    __CutsceneWait(0x1e);
    __MessageID(m + 3);
    __ActorMessage(0xc, 0);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_SetAnim(3, 3);
    __CutsceneWait(0x50);
    { PIN3; q1 = 0x10; q2 = 0; q1 = -q1; q0 = 0xd; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(0xd);
    __MapActor_SetAnim(0xd, 1);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xd, 3);
    { PIN3; q1 = 0xa0; q2 = 0; q1 <<= 7; q0 = 0xd; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_SetAnim(3, 3);
    { PIN3; q2 = 0x84; q1 = 0x98; q2 <<= 2; q0 = 0xc; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x84; q1 = 0xa0; q2 <<= 2; q0 = 0xd; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0xc);
    { PIN3; q2 = 0xa0; q1 = 0xa8; q2 <<= 2; q0 = 0xc; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0xd);
    { PIN3; q2 = 0xa0; q0 = 0xd; q1 = 0xa8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 3; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0xa0 << 7, 0);
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 2; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0x80 << 7, 0);
    __CutsceneWait(0xc8);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneWait(0x1e);
    __Func_809228c(0, -0x10, 0);
    __MapActor_WaitMovement(0);
    __Func_8093500(0, 1);
    { PIN3; q2 = 0xa0; q1 = 0xa8; q2 <<= 2; q0 = 0; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapTransitionOut();
    __Func_8091e9c(3);
}
