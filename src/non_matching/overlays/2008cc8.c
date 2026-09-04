/* OvlFunc_968_2008cc8 -- 0x02008cc8,
 * asm/overlays/rom_7f2f14/ovl_30_a_c_c_c_a.s
 *
 * Tries to hop the player forward. Probes one tile ahead and then two, each
 * time snapping the position to a 0x100000 grid, offsetting by half a tile,
 * stepping along the facing rounded to a quarter turn, and rejecting the move
 * if either the collision test or OvlFunc_968_200832c objects. If both probes
 * are clear it plays the hop -- pose, sound, three motion fields, sprite flags
 * off, teleport to the landing tile, flags back -- and returns 1. Any rejection
 * returns 0 without a cutscene.
 *
 * 104 of 140, and OURS IS THREE INSTRUCTIONS LONGER.
 *
 * BLOCKER, as far as it was taken: the operations are all correct and in the
 * ROM's order. The residue is a systematic REGISTER ROTATION -- r2 against r0,
 * r3 against r2, and so on through the whole body -- which is why the count is
 * 104 of 140 rather than something proportional to the real disagreement. Once
 * the first assignment differs every later one does.
 *
 * WHAT I DID NOT ESTABLISH, and this park should not pretend otherwise: WHY WE
 * ARE THREE INSTRUCTIONS LONG. A pure rotation does not change the length, so
 * something else is also wrong, and I did not isolate it. That is the first
 * thing to attack if this is revisited -- find the three, and the rotation may
 * well follow.
 *
 * The setup difference visible at the first divergence is suggestive but not
 * proven to be the cause. The ROM establishes the flag pointer and reads the
 * saved byte through it, THEN loads the mask, THEN parks the pointer in r8:
 *
 *     mov r2, #0x55 / add r2, r5 / ldrb r3, [r2] / ldr r7, =0xfff00000
 *     mov r9, r3 / ldr r3, [r5, #8] / mov r8, r2
 *
 * where we interleave the same operations differently and land the pointer in
 * a different register.
 *
 * TRIED:
 *   a  the natural writing, both probe blocks spelled out in full   104 differing
 *   b  a, with the three reused constants named as locals --
 *      0xfff00000, 0x80 << 12 and 0xc0 << 8, which the ROM holds in
 *      r7, r2 and r10 respectively                                 133, and
 *      FOUR instructions long
 *
 * That (b) is worse is worth recording. The ROM plainly does hold those three
 * constants in registers across the whole body, so naming them looks like the
 * obvious move -- and it makes things worse, because gcc was already hoisting
 * them and the extra pseudos only add pressure. This is the same trap recorded
 * on OvlFunc_927_2009c34: a constant living in a register across calls is not
 * evidence that the source named it, and here the check is easy, since the ROM
 * builds each constant ONCE at its first use rather than materialising it into
 * a register up front.
 *
 * The two probe blocks are written out in full rather than shared, per the
 * duplicated-code rule; that part is not in question.
 */

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
    unsigned char *a;
    unsigned char *fp;
    int v[3];
    int saved;

    a = __MapActor_GetActor(0);
    fp = a + 0x55;
    saved = *fp;
    v[0] = (*(int *)(a + 8) & 0xfff00000) + (0x80 << 12);
    v[1] = *(int *)(a + 0xc);
    v[2] = (*(int *)(a + 0x10) & 0xfff00000) + (0x80 << 12);
    __vec3_translate(0x80 << 13,
                     (*(unsigned short *)(a + 6) + (0x80 << 6)) & (0xc0 << 8), v);
    if (__TestCollision(a, v) == 1)
        return 0;
    if (OvlFunc_968_200832c(v, a) != 0)
        return 0;
    v[0] = (*(int *)(a + 8) & 0xfff00000) + (0x80 << 12);
    v[1] = *(int *)(a + 0xc);
    v[2] = (*(int *)(a + 0x10) & 0xfff00000) + (0x80 << 12);
    __vec3_translate(0x80 << 14,
                     (*(unsigned short *)(a + 6) + (0x80 << 6)) & (0xc0 << 8), v);
    if (OvlFunc_968_200832c(v, a) != 0)
        return 0;
    if (__TestCollision(a, v) != 0)
        return 0;
    __CutsceneStart();
    __Actor_SetAnim(a, 6);
    __WaitFrames(6);
    __PlaySound(0x98);
    __Actor_SetAnim(a, 7);
    *(int *)(a + 0x30) = 0xc0 << 10;
    *(int *)(a + 0x34) = 0x80 << 10;
    *(int *)(a + 0x28) = 0x80 << 11;
    *fp &= 0x7e;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092158(0, ((short *)v)[1], ((short *)v)[5]);
    __Actor_SetAnim(a, 6);
    __Actor_SetSpriteFlags(a, 1);
    *fp = saved;
    __CutsceneEnd();
    return 1;
}
