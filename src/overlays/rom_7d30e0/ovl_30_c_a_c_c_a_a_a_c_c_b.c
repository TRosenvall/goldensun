/* Overlay 948: the same idle-animation reset, for slot 15.
 *
 * Split out of asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_c.s; the
 * remaining part stays as assembly and is listed before this one in
 * overlays/rom_7d30e0/overlay.ld, so the ROM layout is unchanged.
 *
 * Identical in shape to OvlFunc_934_2009378 in overlay 934, differing only in
 * the slot -- these cutscene stubs are duplicated per map rather than shared.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_948_2008ec8(void)
{
    __CutsceneStart();
    __MapActor_SetAnim(0xf, 0);
    __CutsceneEnd();
}
