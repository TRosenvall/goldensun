// fakematch
/* OvlFunc_934_20094ac  --  0x020094ac
 *
 * Was goldensun/asm/overlays/rom_7bdeb0/ovl_1300_c_c_a_c.s, which held it
 * alone.
 *
 * A cutscene: the player and actor 9 walk toward each other at a shared speed,
 * exchange poses and sounds, actor 9 gets two flag bytes reset and re-set, then
 * two calls place scenery and flag 0x202 records that it happened.
 *
 * Picked by the tools/templated.py criteria as they now stand -- 13 shared
 * symbols, the strongest template on the list, AND zero r8-r11 traffic. The
 * neighbour supplied almost the entire extern block including both
 * overlay-local callees, so no signature had to be derived.
 *
 * FAKEMATCH, and it is the SetSpeed-pair shape for the third time:
 * 0x1999 and 0x80 << 8 feed two __MapActor_SetSpeed calls with calls between
 * them, and gcc parks both in callee-saved registers so they survive -- the
 * recorded exemption for an intervening call does not apply. 0x90 << 1 is a
 * second, independent instance at two __PlaySound calls.
 *
 * FOUR PIECES, ALL CONFIRMED LOAD-BEARING by removing each from the finished
 * file:
 *
 *   drop the pin on the SECOND SetSpeed          -- (it is the last lever, 2)
 *   drop the pin on the first __PlaySound        40 differing
 *   drop the named zero                          70 differing, one line long
 *   drop only the r0 pin in the first SetSpeed    2 differing
 *
 * The r0 pin is worth exactly two instructions, which is the smallest
 * contribution any pin has made across these functions and is a reminder that
 * "load-bearing" is not the same as "important" -- the teardown gives a size,
 * not just a yes or no.
 *
 * THE SHARED ZERO IS A NAMED LOCAL, and the tell is the recorded one. The ROM
 * materialises it into callee-saved r6 and THEN stores it -- `mov r6, #0 /
 * strb r6, [r0]` -- and reuses that register for a byte store, a word store and
 * a stack argument. Written as three literal zeroes gcc emits a fresh `mov` per
 * site and lands them in different registers: 70 differing, and a line long.
 * Contrast OvlFunc_927_2009c34, where the ROM stored the literal straight to
 * its destination and the register copy came after -- that one is a hoist and
 * naming it is wrong. The order of the first use separates them.
 *
 * A NOTE ON READING THE SCREEN. Two intermediate candidates here reported 93
 * and 72 differing when the itemised regions showed 5 and 2 real instructions.
 * A single extra instruction near the top shifts every later line and difflib
 * aligns almost nothing. Read the trailing "N instruction(s) in disagreeing
 * regions" line; the header count is an alignment artefact whenever the lengths
 * differ.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern int OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_934_2008cd0(unsigned char *p);

void OvlFunc_934_20094ac(void)
{
    unsigned char *a;
    int z;

    __CutsceneStart();
    __CutsceneWait(0xa);
    {
        register int v1 __asm__("r1") = 0x80;
        register int v2 __asm__("r2") = 0x1999;
        register int v0 __asm__("r0") = 0;
        v1 <<= 8;
        __MapActor_SetSpeed(v0, v1, v2);
    }
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(0xf);
    __Func_809228c(0, 8, 0);
    __CutsceneWait(4);
    {
        register int s0 __asm__("r0") = 0x90;
        s0 <<= 1;
        __PlaySound(s0);
    }
    __PlaySound(0xef);
    {
        register int w1 __asm__("r1") = 0x80;
        register int w2 __asm__("r2") = 0x1999;
        register int w0 __asm__("r0") = 9;
        w1 <<= 8;
        __MapActor_SetSpeed(w0, w1, w2);
    }
    __MapActor_SetAnim(9, 2);
    z = 0;
    *(__MapActor_GetActor(9) + 0x55) = z;
    *(int *)(__MapActor_GetActor(9) + 0x44) = z;
    __Func_809228c(9, 0xc, 0);
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 1);
    __MapActor_WaitMovement(9);
    __PlaySound(0x90 << 1);
    __PlaySound(0xd5);
    __MapActor_SetAnim(9, 3);
    *(__MapActor_GetActor(9) + 0x55) = 3;
    __Func_809228c(9, 6, 0);
    a = __MapActor_GetActor(9);
    OvlFunc_934_2008cd0(a);
    __MapActor_SetAnim(9, 8);
    __Func_8092b08(9, 3);
    *(__MapActor_GetActor(9) + 0x23) = 2;
    OvlFunc_934_2008528(0, 0xc, 0x10, 1, 4, z);
    OvlFunc_934_2008528(0, 0xd, 0x10, 1, 4, z);
    __SetFlag(0x202);
    __PlaySound(0xf0);
    __CutsceneEnd();
}
