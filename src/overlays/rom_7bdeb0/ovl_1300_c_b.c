/* Overlay 934: put slot 8 back to its idle animation, inside a cutscene.
 *
 * Split out of asm/overlays/rom_7bdeb0/ovl_1300_c.s; the neighbouring parts
 * stay as assembly and are listed around this one in
 * overlays/rom_7bdeb0/overlay.ld, so the ROM layout is unchanged.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_934_2009378(void)
{
    __CutsceneStart();
    __MapActor_SetAnim(8, 0);
    __CutsceneEnd();
}
