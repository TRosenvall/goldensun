/* Cluster OvlFunc_947_2009544..OvlFunc_947_2009544 extracted from goldensun/asm/overlays/rom_7d0e88/ovl_1528_a_a_a_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_7d0e88/overlay.ld, so the ROM layout does not move.
 *
 * A six-word struct is filled by one routine and passed BY VALUE to another.
 * That is what the ldmia/stmia pair in the ROM is:
 *
 *     mov r2, sp / add r3, sp, #0x18 / ldmia r3!, {r0, r1} / stmia r2!, {r0, r1}
 *     ldr r0, [r5] / ldr r1, [r5, #4] / ldr r2, [r5, #8] / ldr r3, [r5, #0xc]
 *
 * -- the first four words go in r0-r3 and the last two are copied to the
 * bottom of the stack as the fifth and sixth arguments. Reading it as a
 * by-value struct argument rather than as an open-coded memcpy is what makes
 * the whole thing fall out; the block move is the ABI, not the source.
 *
 * The struct is spelled `int w[6]` because its field layout is not known --
 * only its size and that it is copied whole. That is deliberately weaker than
 * naming fields we cannot see.
 */
struct Six { int w[6]; };

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int OvlFunc_947_2008758(struct Six *out);
extern void OvlFunc_947_20088ec(struct Six s);

void OvlFunc_947_2009544(void)
{
    struct Six s;

    __CutsceneStart();
    if (OvlFunc_947_2008758(&s))
        OvlFunc_947_20088ec(s);
    __CutsceneEnd();
}
