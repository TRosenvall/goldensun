/* OvlFunc_884_20083b4  --  0x020083b4
 * OvlFunc_884_2008444  --  0x02008444
 *
 * Cut from goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a.s;
 * preserved between ovl_30_c_a_a_a_c_c_a_c_a_a.o and
 * ovl_30_c_a_a_a_c_c_a_c_a_c.o in goldensun/overlays/rom_784360/overlay.ld.
 *
 * A talk that picks one of three lines from two save bits, and a set-piece that
 * poses an actor, pans the camera and speaks. Both matched on the first screen.
 *
 * `__Func_8092c40` IS DELIBERATELY UNDECLARED in the first function. With a
 * prototype gcc fills its argument registers r0 then r1; the ROM fills r1 then
 * r0. That is the second declaration lever from docs/elevation.md -- leaving
 * the MISMATCHING call implicit puts r0 last in its own argument block -- and
 * it is the same call that needed it in OvlFunc_963_2008730 (batch 82).
 *
 * THE SHARED TAIL IS gcc'S. The ROM joins two arms at .L40e, where a slot id
 * already in r0 falls through into `mov r1, #0 / bl __Func_8093054`. Written as
 * two independent arms each ending in their own call, cross-jumping produces
 * exactly that; a `goto` would be transcribing the optimiser.
 *
 * The second function is thirteen calls in a line and needed nothing. Its
 * argument fill order varies between calls to the SAME callee -- r0 first for
 * one `__Func_809280c` and last for another -- which is gcc reporting what the
 * preceding call's return type is, not something to fight.
 */
extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8093054(int slot, int n);
/* __Func_8092c40 intentionally implicit */

void OvlFunc_884_20083b4(void)
{
    char *base;

    __CutsceneStart();
    if (__GetFlag(0x87a)) {
        __MessageID(0x1be8);
        __Func_8092c40(0xf, 0);
        if (__Func_8091c7c(0, 0) == 1) {
            __ActorMessage(0xf, 0);
        } else {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1)))++;
            __Func_8093054(0xf, 0);
        }
    } else if (__GetFlag(0x815)) {
        __MessageID(0x1191);
        __Func_8093054(0xb, 0);
    } else {
        __MessageID(0xea8);
        __Func_8093054(0xb, 0);
    }
    __CutsceneEnd();
}

extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_SetBehavior(int slot, int b);
extern void OvlFunc_884_200a2c8(int slot, int n);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int slot, int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

void OvlFunc_884_2008444(void)
{
    __CutsceneStart();
    __MapActor_SetAnim(0x1a, 1);
    __Func_809280c(0x1a, 0, 0x14);
    __Func_809280c(0x1a, 0x15, 0x28);
    __MessageID(0x11c7);
    OvlFunc_884_200a2c8(0x1a, 0x14);
    __Func_80933d4(0x19999, 0x3333);
    __Func_80933f8(0x1510000, -1, 0x88 << 17, 1);
    __CutsceneWait(0x14);
    __Func_80925cc(0x1a, 2);
    __CutsceneWait(0x14);
    __Func_809280c(0x1a, 0, 0xa);
    OvlFunc_884_200a2c8(0x1a, 0x28);
    __MapActor_SetBehavior(0x1a, 2);
    __CutsceneEnd();
}
