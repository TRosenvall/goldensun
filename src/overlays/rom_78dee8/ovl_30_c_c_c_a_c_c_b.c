// fakematch
/* OvlFunc_895_200961c  --  0x0200961c
 *
 * Cut out of goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c.s.
 *
 * 149 instructions of straight-line cutscene: a map transition, an actor walked
 * through three camera pushes, two dialogue lines. Sixteen call sites are
 * pinned with each site's assignments in its own ROM order.
 *
 * THREE CROSSED SITES, ALL THE SAME CALL, closed with the batch-207 per-mov
 * barrier. __Func_80933f8 is called three times and each fill runs its movs in
 * one order and its shifts and negations in another:
 *
 *     mov r0,#0xd1 / mov r1,#1 / mov r2,#0x83 / lsl r2,#18 / mov r3,#1 /
 *     neg r1,r1 / lsl r0,#19
 *
 * Six of 149 differing with the pins alone, and it is exactly the first mov of
 * each fill landing second. Barriering each mov whose position is wrong, in ROM
 * order -- two on the first two sites, one on the third, where only r1 needs
 * moving -- is exact. Third batch running that this lever behaves as the n-1
 * rule predicts, on a function it was not derived from.
 *
 * The __MapActor_SetSpeed at the head is crossed too, in the two-register form,
 * and takes one barrier.
 *
 * A CONSTANT DERIVED FROM AN OFFSET, and it needs no help. The opening store is
 *
 *     mov r2, #0xe0 / lsl r2, #1 / add r3, r2 / add r2, #0x44 / str r2, [r3]
 *
 * -- the stored VALUE 0x204 is built by adding 0x44 to the ADDRESS OFFSET 0x1c0
 * that is already in the register. Written as two plain literals,
 * `*(int *)(p + (0xe0 << 1)) = 0x204;`, gcc finds that itself. Worth recording
 * because the reflex on seeing a value derived from an unrelated-looking
 * quantity is to reach for a lever, and here the cost model does it unaided.
 *
 * `0x4008` is passed to __Func_8093040 twice and the ROM reloads it from the
 * pool at each, so both sites take an r0 pin.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_895_200961c(void)
{
    unsigned char *p;

    __CutsceneStart();
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x204;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(8, *(int *)(p + 8), *(int *)(p + 0x10));
    {
        PIN3;
        q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
        q2 = 0x80; q2 <<= 8; q0 = 8; q1 <<= 9;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapActor_SetAnim(8, 2);
    { PIN3; q2 = 0xa; q2 = -q2; q1 = 0x18; q0 = 8; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    { PIN2; q1 = 1; q0 = 8; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(6);
    { PIN3; q1 = 0xb0; q0 = 8; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0x28; q0 = 0; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4;
      q0 = 0xd1; __asm__ volatile ("" : : "r" (q0));
      q1 = 1; __asm__ volatile ("" : : "r" (q1));
      q2 = 0x83; q2 <<= 18; q3 = 1; q1 = -q1; q0 <<= 19;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_80933d4(0x19999, 0x3333);
    { PIN4;
      q0 = 0xeb; __asm__ volatile ("" : : "r" (q0));
      q1 = 1; __asm__ volatile ("" : : "r" (q1));
      q2 = 0x83; q2 <<= 18; q3 = 1; q1 = -q1; q0 <<= 19;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_80933d4(0x33333, 0x6666);
    { PIN4;
      q1 = 1; __asm__ volatile ("" : : "r" (q1));
      q2 = 0x89; q3 = 1; q2 <<= 18; q1 = -q1; q0 = 0x6e90000;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_80925cc(8, 2);
    { PIN3; q1 = 0; q2 = 0x1e; q0 = 8; __Func_8092adc(q0, q1, q2); }
    __MessageID(0x103a);
    { PIN3; q0 = 0x4008; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 8; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(8, 1);
    { PIN3; q1 = 0xa0; q0 = 8; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x4008; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(8, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(8, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(8);
    { PIN3; q1 = 0; q2 = 0; q0 = 8; __MapActor_SetPos(q0, q1, q2); }
    __SetFlag(0x825);
    __CutsceneEnd();
}
