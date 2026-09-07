/* OvlFunc_968_2008cc8 -- overlay 968 (rom_7f2f14), ovl_30_a_c_c_c_a.
 *
 * EXACT ON THE FIRST DRAFT, zero iterations.  Re-run seven times:
 *
 *   OK OvlFunc_968_2008cc8 -- 316 bytes, 140 encodings and 17 relocations identical
 *
 * TEMPLATE: OvlFunc_964_2008cd0, src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_a.c.
 * tools/neighbour.py ranked it 10 of 11 shared symbols and it supplied the
 * ENTIRE body -- struct, locals, the `p = v` array alias, the two
 * `(field & 0xfff00000) + (0x80 << 12)` probe coordinates, the
 * `(f6 + (0x80<<6)) & (0xc0<<8)` angle, the `*f = *f & 0x7e` byte mask and the
 * whole cutscene tail -- with no lever added and no pin anywhere.
 *
 * THREE THINGS DIFFER FROM THE TEMPLATE AND ALL THREE ARE PLAIN TRANSCRIPTION:
 *   - a SECOND probe round: the coordinates are rebuilt and `__vec3_translate`
 *     is called again at 0x80<<14 instead of 0x80<<13, with the
 *     OvlFunc_968_200832c / __TestCollision tests in the opposite order;
 *   - the two collision tests are spelled differently BECAUSE THE ROM SPELLS
 *     THEM DIFFERENTLY -- `cmp r0,#1 / beq` in the first round is `!= 1`, and
 *     `cmp r0,#0 / bne` in the second is `== 0`.  They are not interchangeable;
 *   - the return values are INVERTED against the template (1 on success, 0 on
 *     every failure path), which the ROM shows as `mov r0,#1 / b .Ldee` over a
 *     `.Ldec: mov r0,#0`.
 *
 * `__Func_8092158(0, ((short *)p)[1], ((short *)p)[5])` is the ROM's two
 * register-offset `ldrsh`es off the frame vector.  The template computes the
 * same tile indices arithmetically (`((a[0] >> 20) << 4) + 8`) because ITS ROM
 * does; here the halfword-load spelling is what the ROM shows and Thumb has no
 * immediate-offset `ldrsh`, so the `mov r3,#0xa / ldrsh` pair is automatic.
 *
 * LANDING: WHOLE, no split, no linker edit.
 *   asmfacts.py: WHOLE  convert directly
 *   One function, no `.section`/`.global`/`incbin`/`.lcomm`, no cross-object
 *   `.L` or data symbol.
 *     overlays/rom_7f2f14/overlay.ld:44
 *         asm/overlays/rom_7f2f14/ovl_30_a_c_c_c_a.o(.text)   <- VERBATIM
 *   tryc.makefile_flags() = set(); no rom_7f2f14 wildcard reaches this stem.
 *   Unique basename across the tree's .ld files.
 *
 * NOTHING WAS MEASURED WORSE because nothing else was measured: the first
 * candidate was exact and the discipline is to stop.  Recorded for the
 * template-reuse ledger: this is the fourth zero-iteration match produced by
 * reading a neighbour before writing anything.
 *
 * -- worked in scratch_elev/b253/f968/cc8
 */
struct Ent {
    unsigned char pad0[6];
    unsigned short f6;
    unsigned int f8;
    unsigned int fc;
    unsigned int f10;
};
extern unsigned char *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);
extern int __TestCollision(unsigned char *e, int *v);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(unsigned char *e, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *e, int f);
extern void __Func_8092158(int a, int b, int c);
extern int OvlFunc_968_200832c(int *v, unsigned char *e);

int OvlFunc_968_2008cc8(void)
{
    int v[3];
    int *p;
    unsigned char *e;
    unsigned char *f;
    int saved;
    int n0;
    int n1;

    e = __MapActor_GetActor(0);
    f = e + 0x55;
    saved = *f;
    p = v;
    p[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(e + 0xc);
    p[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0x80 << 12);
    n0 = (((struct Ent *)e)->f6 + (0x80 << 6)) & (0xc0 << 8);
    __vec3_translate(0x80 << 13, n0, p);
    if (__TestCollision(e, p) != 1) {
    if (OvlFunc_968_200832c(p, e) == 0) {
    p[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(e + 0xc);
    p[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0x80 << 12);
    n1 = (((struct Ent *)e)->f6 + (0x80 << 6)) & (0xc0 << 8);
    __vec3_translate(0x80 << 14, n1, p);
    if (OvlFunc_968_200832c(p, e) == 0) {
    if (__TestCollision(e, p) == 0) {
    __CutsceneStart();
    __Actor_SetAnim(e, 6);
    __WaitFrames(6);
    __PlaySound(0x98);
    __Actor_SetAnim(e, 7);
    *(int *)(e + 0x30) = 0xc0 << 10;
    *(int *)(e + 0x34) = 0x80 << 10;
    *(int *)(e + 0x28) = 0x80 << 11;
    *f = *f & 0x7e;
    __Actor_SetSpriteFlags(e, 0);
    __Func_8092158(0, ((short *)p)[1], ((short *)p)[5]);
    __Actor_SetAnim(e, 6);
    __Actor_SetSpriteFlags(e, 1);
    *f = saved;
    __CutsceneEnd();
    return 1;
    }
    }
    }
    }
    return 0;
}
