// fakematch
/* OvlFunc_943_20092f0  --  0x020092f0
 *
 * Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a.s.
 *
 * Eighty instructions, exact on the first screen. Twelve pinned call sites and
 * one crossed __Func_80933f8 fill that takes two volatile-asm barriers, placed
 * from the listing before the first compile rather than found by iterating.
 * `tools/crossed.py` reported `crossed-sites=1 BARRIER` during selection, which
 * is the verdict working as intended since batch 206 corrected it -- a route to
 * the lever, not a reason to skip.
 *
 * The __CopyMapTiles call passes two stack arguments and they are two named
 * locals, because the pair has to be live at once for the ROM's
 * `str r3, [sp] / str r2, [sp, #4]`. Note the ROM builds `0x6c` TWICE, once
 * into r1 and once into r3, rather than copying one to the other; two plain
 * literals give that.
 *
 * Both __MapActor_SetSpeed and the first __Func_80921c4 are anchored even
 * though their ROM order is plain ascending r0/r1/r2, because their later
 * arguments are POOL LOADS -- the rule from batch 208, and it applied here
 * without a re-screen to discover it.
 */
extern void OvlFunc_943_2008bf0(void);

extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_943_20092f0(void)
{
    int e0, e1;

    __Func_80933d4(0x19999, 0x3333);
    {
        PIN4;
        q0 = 0xd8; __asm__ volatile ("" : : "r" (q0));
        q1 = 1; __asm__ volatile ("" : : "r" (q1));
        q2 = 0xce; q1 = -q1; q2 <<= 18; q3 = 1; q0 <<= 16;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __Func_8093530();
    __CutsceneWait(0x14);
    OvlFunc_943_2008bf0();
    e0 = 1;
    e1 = 2;
    __CopyMapTiles(0x1e, 0x6c, 0xd, 0x6c, e0, e1);
    __CutsceneWait(0xa);
    { PIN3; q1 = 0xd8; q2 = 0xc8; q0 = 0x14; q1 <<= 16; q2 <<= 18;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0xd8; q2 = 0x32e; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0xa; q0 = 0; q1 = 0x14; __Func_809280c(q0, q1, q2); }
    __MapActor_DoAnim(0x14, 4);
    __Func_809259c(0x14, 2);
    { PIN3; q1 = 0x80; q0 = 0x14; q1 <<= 1; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x14; q1 = 0; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x14; __Func_809259c(q0, q1); }
    __MessageID(0x1d8d);
    __Func_8093040(0x14, 0, 0x14);
    { PIN3; q1 = 0x81; q1 <<= 1; q2 = 0; q0 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    __SetFlag(0x923);
}
