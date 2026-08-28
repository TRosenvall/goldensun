/* Cluster OvlFunc_959_200cbfc..OvlFunc_959_200cbfc, the whole of
 * goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_c_a_c_a_c_c.s -- a single-function TU with no
 * data, so no split was needed and overlay.ld is untouched.
 *
 * Total .text for this TU = 272 bytes (= 0x110).
 *
 * BUILT WITH CSE_CFLAGS -- see the rule in the Makefile.  The flag id 0x226 is
 * used twice, GetFlag at the top and SetFlag at the bottom; at -O2 gcc commons
 * the two pool loads into a callee-saved register and adds a push the ROM does
 * not have, which shifts the whole function (98 differing).
 *
 * TWO SPELLINGS ARE LOAD-BEARING:
 *
 *   1. The message ids are SYMBOLS taken by address, not int literals.  The ROM
 *      holds each base in r5 and derives its neighbours -- `add r5, #1` off the
 *      first, `add r0, r5, #1/2/3` off the second.  Written as plain constants
 *      gcc rematerialises each id and never keeps one alive.  Only the symbol
 *      form makes it spend the register.  _MSG_242e and _MSG_2430 added to
 *      message.sym; the ids in between are reached by arithmetic and unnamed.
 *
 *   2. __Func_8092c40 IS DELIBERATELY LEFT UNDECLARED -- do not add a prototype.
 *      It is called three times and the ROM orders its two arguments r1-then-r0
 *      at the first site and r0-then-r1 at the other two.  With a prototype gcc
 *      emits r0 first everywhere and the first site is wrong.  Dropping it fixes
 *      that site WITHOUT breaking the other two -- which is worth recording,
 *      because the documented expectation for a callee whose sites disagree is
 *      that the lever trades one for the other.  Here it did not; it was
 *      screened rather than assumed.
 */
extern int _MSG_242e;
extern int _MSG_2430;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_959_200cbfc(void)
{
    int m;

    if (__GetFlag(0x226)) {
        __MessageID(0x2434);
        __ActorMessage(0x14, 0);
        return;
    }
    __CutsceneStart();
    __Func_809280c(0x14, 0, 0);
    if (__GetFlag(0x227) == 0) {
        __MapActor_Jump(0x14, 4, 0);
        __MapActor_SetIdle(0x14);
        __MapActor_WaitScript(0x14);
        __CutsceneWait(0x14);
        m = (int)&_MSG_242e;
        __MessageID(m);
        __ActorMessage(0x14, 0);
        __MapActor_Emote(0x14, 0x81 << 1, 0x1e);
        m += 1;
        __MessageID(m);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0x1e);
        __MapActor_SetAnim(0x14, 4);
        __CutsceneWait(0x1e);
    }
    m = (int)&_MSG_2430;
    __MessageID(m);
    __ActorMessage(0x14, 0);
    __MapActor_Emote(0x14, 0x101, 0x28);
    __MessageID(m + 1);
    __Func_8092c40(0x14, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(m + 2);
        __Func_8092c40(0x14, 0);
        __SetFlag(0x226);
    } else {
        __MessageID(m + 3);
        __Func_8092c40(0x14, 0);
    }
    __SetFlag(0x227);
    __CutsceneEnd();
}
