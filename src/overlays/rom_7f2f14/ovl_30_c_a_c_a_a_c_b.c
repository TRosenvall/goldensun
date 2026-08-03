/* Overlay 968: a talk sequence with a positioned speaker.
 *
 * Split out of asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_a_c.s; the neighbouring
 * parts stay as assembly and are listed around this one in
 * overlays/rom_7f2f14/overlay.ld, so the ROM layout is unchanged.
 *
 * This overlay has -O1 rules for two other stems (ovl_30_c_a_c_a_c_c% and
 * ovl_30_c_a_c_a_c_a%). This file matches neither and builds at the default
 * -O2 -- checked before landing, since screening a -O1 unit at -O2 produces a
 * clean-looking match that then fails the build.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __Func_8093040(int slot, int a, int b);
extern void __Func_80925cc(int slot, int a);
extern void __ActorMessage(int slot, int arg);

void OvlFunc_968_2008fbc(void)
{
    __CutsceneStart();
    __MessageID(0x2670);
    __Func_8093040(0xb, 0, 0x14);
    __Func_80925cc(0xb, 2);
    __ActorMessage(0xb, 0);
    __CutsceneEnd();
}
