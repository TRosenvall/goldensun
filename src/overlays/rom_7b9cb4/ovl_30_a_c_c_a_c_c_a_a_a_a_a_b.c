// fakematch
/* OvlFunc_932_2008c9c  --  0x02008c9c
 *
 * Cut out of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a.s.
 *
 * A short cutscene: place a tile group, play a cue, give the player and actor
 * 0xa a shared walk speed, clear bit 0 of actor 0xa's flag byte at +0x5a, walk
 * both to adjacent marks, and hand off to OvlFunc_932_200840c.
 *
 * Picked on the current templated.py criteria -- 10 shared symbols and ZERO
 * r8-r11 traffic. It is close kin to OvlFunc_891_2009b44, elevated earlier in
 * this batch: same SetSpeed pair with 0x3333 and 0x1999, the same `&= ~1` on
 * +0x5a, the same paired TravelTo. That kinship is exactly what the tool is
 * for, and the whole extern block came from it.
 *
 * FAKEMATCH. THREE separate repeated constants, each hoisted into a
 * callee-saved register where the ROM rebuilds it, and each needing its own
 * treatment:
 *
 *   0x3333 / 0x1999   two __MapActor_SetSpeed calls
 *   0xd4 << 1         two __MapActor_TravelTo calls, as the z argument
 *   0x18 / 0x1a       the two stack arguments of __Func_8010704
 *
 * The plain candidate is 52 differing of 54 with the prologue widened to
 * `push {r5, r6, lr}` against the ROM's bare `push {r14}` -- the recorded
 * wider-prologue marker, and the reason this was diagnosed in one look rather
 * than swept.
 *
 * THREE PIECES, EACH CONFIRMED LOAD-BEARING by removal from the finished file:
 *
 *   drop the pin on the first TravelTo          14 differing, one line long
 *   drop the paired stack-argument locals        5 differing
 *   drop only the r0 pin in the SetSpeed block   3 differing
 *
 * PIN DECLARATION ORDER IS ARGUMENT ORDER, and it was the last two
 * instructions. The ROM sets `mov r0, #0xa` FIRST and the two pooled constants
 * after; declaring the pins as r1, r2, r0 emits them in that order and leaves
 * r0 three slots late. Declaring r0 first lands it. This is the same reading as
 * the recorded "declaration order is argument-setup order" for pinned
 * registers, and it is worth stating that it applies to the ORDER OF THE
 * DECLARATIONS rather than to the order of the arguments in the call.
 *
 * The two stack arguments are a named PAIR because the ROM materialises both
 * before storing either -- the eager-issue face of the named-local rule applied
 * to stack arguments, first recorded on OvlFunc_883_200d950.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_200840c(void);

void OvlFunc_932_2008c9c(void)
{
    unsigned char *a;
    int p, q;

    __CutsceneStart();
    p = 0x18;
    q = 0x1a;
    __Func_8010704(0x18, 0x1b, 2, 1, p, q);
    __PlaySound(0xb9);
    {
        register int v0 __asm__("r0") = 0xa;
        register int v1 __asm__("r1") = 0x3333;
        register int v2 __asm__("r2") = 0x1999;
        __MapActor_SetSpeed(v0, v1, v2);
    }
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    a = __MapActor_GetActor(0xa) + 0x5a;
    *a &= ~1;
    __MapActor_SetAnim(0, 8);
    {
        register int t1 __asm__("r1") = 0xc8;
        register int t2 __asm__("r2") = 0xd4;
        register int t0 __asm__("r0") = 0;
        t1 <<= 1;
        t2 <<= 1;
        __MapActor_TravelTo(t0, t1, t2);
    }
    __MapActor_TravelTo(0xa, 0xcc << 1, 0xd4 << 1);
    __MapActor_WaitMovement(0xa);
    __MapActor_SetAnim(0, 1);
    OvlFunc_932_200840c();
    __CutsceneEnd();
}
