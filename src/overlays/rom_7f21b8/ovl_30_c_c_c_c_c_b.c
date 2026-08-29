/* Cluster OvlFunc_967_2008eec..OvlFunc_967_2008eec extracted from goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c.s.
 *
 * Total .text for this TU = 352 bytes (= 0x0160).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_a.o and asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7f21b8/overlay.ld.
 *
 * The original .s held three functions and a .data section.  The data follows
 * the THIRD function, so it travels with _c and this TU is pure text.
 *
 * Nine other overlay directories contain a file also named ovl_30_c_c_c_c_c.s;
 * they are unrelated and untouched.  Match on the full path, never the
 * basename, when checking which linker scripts reference a TU.
 *
 * __Func_8092c40 IS DELIBERATELY LEFT UNDECLARED -- do not add a prototype.
 * With one, gcc loads r0 before setting r1 at its single call site; the ROM
 * sets r1 first.  See the no-prototype lever in docs/elevation.md.
 */
extern int _MSG_2880;

extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_967_2008eec(void)
{
    __MessageID((int)&_MSG_2880);
    __CutsceneWait(0x14);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_809280c(1, 0, 0x32);
    __MapActor_Emote(0, 0x105, 0x3c);
    __CutsceneWait(0xa);
    __Func_8092adc(1, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x14);
    __CutsceneWait(0x19);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_809280c(2, 0, 0x1e);
    __Func_8092c40(0x2002, 0);
}
