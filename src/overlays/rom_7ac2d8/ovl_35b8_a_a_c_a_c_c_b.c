// fakematch
/* OvlFunc_924_200ca08  --  0x0200ca08
 * [asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c.s, third of five functions]
 *
 * 229 instructions: a djinni-recruitment scene guarded on the party leader's
 * position, with a re-ask loop if the player declines. Byte-exact: 608 bytes,
 * 230 encodings and 66 relocations identical.
 *
 * THE BIG LEVER IS THE RE-ASK LOOP: A SHARED MIDDLE MUST BE SHARED IN SOURCE.
 * Written the obvious way -- a duplicated preamble followed by
 * `while (__Func_8091c7c(...) == 1)` -- the instructions come out right but
 * gcc's cross-jumper only merges back as far as the `bl __Func_8091c7c`, three
 * instructions short of where the ROM puts its loop label. The ROM's label sits
 * at the `bl __MessageID`. Writing it as `while (1)` with the `break` in the
 * middle and a `msg` variable carrying the differing id puts the label where
 * the ROM has it: 233 lines and 122 differing to 229 and 9, in one step.
 *
 * The recorded rule is that jump.c merges only IDENTICAL blocks, so a middle
 * reached from two arms with a differing constant can never be produced by
 * cross-jumping. The twist here is that `msg` needs no pin: both its
 * definitions sit immediately before the loop-top use with no call between, so
 * it lands in r0 at both.
 *
 * `register int m __asm__("r5")` for the message base, the neighbour's finding
 * applied unchanged: r5 holds one value for the whole body, `mov r0, r5` at the
 * first __MessageID and `add r0, r5, #5` about ninety instructions later. A
 * three-operand add off a pooled constant proves a live pseudo, and a plain
 * `int m` is destroyed by gcse's cprop at -O2.
 *
 * `do { } while (0)` after __CutsceneStart, or sched2 hoists the message
 * base's pool load above the call.
 *
 * TWO NAMED LOCALS FOR __Func_8010704'S STACK ARGUMENTS. The ROM emits
 * `mov r3 / mov r2 / str r3,[sp] / str r2,[sp,#4]` -- both values live at once.
 * Bare literals give `mov r3 / str / mov r3 / str`, because both pseudos get r3
 * and the anti-dependence blocks the hoist. Two named locals force two
 * simultaneously-live pseudos. A hard-register pin on r2/r3 also works and was
 * REJECTED: the named locals are weaker scaffolding for the same result.
 *
 * ALL ELEVEN LEVERS ARE LOAD-BEARING -- each was stripped individually under
 * objcmp and every one fails, so there was no inert subset needing a joint
 * re-check.
 *
 * Every relocation in this function's range is an R_ARM_THM_CALL; there is no
 * R_ARM_ABS32 anywhere in it, so all eight pooled values are bare literals.
 *
 * No wildcard captures this object; tree default -O2.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __CalcStats(int slot);
extern int __GiveDjinni(int slot, int elem, int n);
extern int __SetDjinni(int slot, int elem, int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80917d0(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_924_200ca08(void)
{
    unsigned char *p;
    register int m __asm__("r5");

    if (*(int *)(__MapActor_GetActor(8) + 8) / 0x100000 != 0x30)
        return;
    __CutsceneStart();
    do { } while (0);
    m = 0x1591;
    __MessageID(m);
    __CutsceneWait(0x14);
    __Func_80925cc(3, 1);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(3, 0, 0x14);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(3, 0x10);
    __CutsceneWait(0x32);
    __MapActor_SetAnim(3, 1);
    { PIN2; q1 = 0; q0 = 3; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 1) {
        __CutsceneWait(0x14);
        __Func_80925cc(3, 2);
        __CutsceneWait(0x14);
        __Func_8093040(3, 0, 0x14);
        __MapActor_DoAnim(3, 4);
        __CutsceneWait(0x14);
        __Func_8093040(3, 0, 0x14);
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0x14);
        { PIN2; q1 = 0; q0 = 3; __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 1) {
            int msg;

            __CutsceneWait(0x14);
            { PIN2; q1 = 4; q0 = 3; __MapActor_DoAnim(q0, q1); }
            __CutsceneWait(0x14);
            msg = m + 5;
            while (1) {
                __MessageID(msg);
                { PIN2; q1 = 0; q0 = 3; __Func_8092c40(q0, q1); }
                if (__Func_8091c7c(0, 0) != 1)
                    break;
                __CutsceneWait(0x14);
                { PIN2; q1 = 4; q0 = 3; __MapActor_DoAnim(q0, q1); }
                __CutsceneWait(0x14);
                msg = 0x1639;
            }
        }
    }
    __MessageID(0x1597);
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(3, 0xb6 << 2, 0x9e << 2);
    __CutsceneWait(0x14);
    __Func_8093040(3, 0, 0x14);
    __MapActor_SetAnim(3, 0x10);
    __Func_8093040(3, 0, 0x14);
    __MapActor_SetAnim(3, 1);
    __Func_809280c(3, 0, 0x14);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __Func_8093040(3, 0, 0x14);
    { PIN3; q2 = 0x5a; q0 = 3; q1 = 0x105;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_8093040(3, 0, 0x14);
    __Func_80917d0(3, 1);
    __SetFlag(0x44);
    __GiveDjinni(3, 1, 0);
    __SetDjinni(3, 1, 0);
    __CalcStats(3);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    { int e = 0x2e, f = 0x27;
      __Func_8010704(0x6e, 0x27, 5, 1, e, f); }
    __SetFlag(0x873);
    __CutsceneEnd();
}
