/* Overlay 901: a talk stub taking its slot as an argument.
 *
 * Split out of asm/overlays/rom_797990/ovl_314_c_c_a_a_a.s; the neighbouring
 * parts stay as assembly and are listed around this one in
 * overlays/rom_797990/overlay.ld, so the ROM layout is unchanged.
 *
 * Most of the cutscene stubs in this corpus hard-code their slot; this one is
 * parameterised, which is why the slot survives in r5 across all three calls.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __ActorMessage(int slot, int arg);

void OvlFunc_901_20084b4(int slot)
{
    __CutsceneStart();
    __MapActor_SetAnim(slot, 1);
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}
