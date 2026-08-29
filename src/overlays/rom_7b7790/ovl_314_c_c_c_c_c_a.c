/* Cluster OvlFunc_929_2008524..OvlFunc_929_2008524 extracted from goldensun/asm/overlays/rom_7b7790/ovl_314_c_c_c_c_c_a.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in goldensun/overlays/rom_7b7790/overlay.ld is unchanged.
 *
 * TalkStaged. Slot 9 delivers a line, turns to face slot 0x0a for sixty
 * frames, turns back to slot 0 for twenty, then delivers a closing line.
 *
 * THE PROTOTYPES BELOW ARE LOAD-BEARING -- see docs/elevation.md.
 * An IMPLICITLY declared callee returns int, so gcc-2.96 treats r0 as holding
 * a live return value across the call and defers writing r0 for the next
 * one. Declaring every callee, with its void return type, is what reproduces
 * the ROM's argument fill order here. Written without them, the two calls
 * that pass their first argument in r0 first come out with r0 last.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_8093040(int slot, int b, int c);
extern void __Func_809280c(int slot, int target, int c);

void OvlFunc_929_2008524(void) {
    __CutsceneStart();
    __MessageID(0x1a64);
    __Func_8093040(9, 0, 0x14);
    __Func_809280c(9, 0xa, 0);
    __CutsceneWait(0x3c);
    __Func_809280c(9, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    __CutsceneEnd();
}
