/* Cluster OvlFunc_931_2008360..OvlFunc_931_2008360 extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a.s.
 *
 * Split out of that .s; the sibling parts stay as assembly.
 *
 * A three-way talk: before flag 0x242 is set, one line; after it, either a
 * hand-off to __Func_80b0278 or a second line. Thirteen calls in thirty-nine
 * instructions.
 *
 * NOTE WHERE THE SECOND `if` ENDS. Flag 0x909 guards only the extra
 * __MessageID; the __ActorMessage after it runs either way. Written with both
 * inside the guard, the function differs from the ROM by a single byte -- the
 * beq's offset -- and that difference was invisible to tools/tryc.py until the
 * screen was taught to keep label POSITIONS. See
 * src/non_matching/ovl_7b8cb0/2008360.c for that account; this file is the
 * corrected version.
 *
 * __Func_8093054 is left undeclared so its r0 is filled last; __Func_80b0278
 * and __ActorMessage are declared so theirs comes first.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_80b0278(int a, int b);
extern int OvlFunc_931_2008338(void);

void OvlFunc_931_2008360(void)
{
    if (!__GetFlag(0x242)) {
        __CutsceneStart();
        __MessageID(0x18e7);
        __Func_8093054(0xf, 0);
        __CutsceneEnd();
    } else if (OvlFunc_931_2008338()) {
        __Func_80b0278(0x13, 0xf);
    } else {
        __CutsceneStart();
        __MessageID(0x18ea);
        if (__GetFlag(0x909))
            __MessageID(0x1941);
        __ActorMessage(0xf, 0);
        __CutsceneEnd();
    }
}
