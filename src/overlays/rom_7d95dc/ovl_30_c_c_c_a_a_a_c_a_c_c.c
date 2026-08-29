/* OvlFunc_953_200855c  --  0x0200855c
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * changes name in goldensun/overlays/rom_7d95dc/overlay.ld and nothing else
 * moves.
 *
 * The tutorial conversation with the Vale elder: run it once, and afterwards
 * just pause for the same twenty frames instead.
 *
 * BUILT WITH CSE_CFLAGS. Under the default flags this is 50 differing of 52,
 * because 0x3c1 is read once and written once and gcc keeps it in a
 * callee-saved register across the intervening calls, growing a `push {r5}`
 * the ROM does not have. `-fno-rerun-cse-after-loop` matches it exactly with no
 * change to the C -- and there is no change to make: two calls taking the same
 * flag id is what the function does.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void OvlFunc_953_2009c48(int slot);
extern void OvlFunc_953_2009c5c(int slot, int v);

void OvlFunc_953_200855c(void)
{
    __CutsceneStart();
    __Func_80925cc(0x10, 2);
    __MessageID(0x211b);
    __Func_8093040(0x10, 0, 0x14);
    if (__GetFlag(0x3c1)) {
        __CutsceneWait(0x14);
    } else {
        OvlFunc_953_2009c5c(0x11, 0);
        __Func_80925cc(0x11, 1);
        OvlFunc_953_2009c48(0x11);
        __Func_8092848(0x11, 0, 0x14);
        __MapActor_SetAnim(0x11, 4);
        OvlFunc_953_2009c48(0x11);
        __MapActor_Emote(0x11, 0x105, 0x28);
        OvlFunc_953_2009c48(0x11);
        OvlFunc_953_2009c5c(0x11, 0xa0 << 7);
        __SetFlag(0x3c1);
    }
    __CutsceneEnd();
}
