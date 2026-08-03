/* Cluster OvlFunc_901_20084d8..OvlFunc_901_20084d8 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Talk with staging: turns slot 8 toward the player, records save bit 0x305,
 * and delivers line 0x1cab.
 *
 * __Func_809280c is deliberately left undeclared -- the ROM fills its r0 LAST,
 * which is the implicitly-declared shape (docs/elevation.md). __ActorMessage
 * IS declared, because the ROM fills its r0 first. Both halves of that rule
 * are needed in this one function.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);

void OvlFunc_901_20084d8(void)
{
    __CutsceneStart();
    __Func_809280c(8, 0, 2);
    __SetFlag(0x305);
    __MessageID(0x1cab);
    __ActorMessage(8, 0);
    __CutsceneEnd();
}
