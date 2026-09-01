/* OvlFunc_957_2008de8 (0x02008de8) -- NON-MATCHING.
 * Blocker class: instruction placement (pre-reload scheduling), TWO lines.
 * Companion to src/non_matching/ovl_7b6668/2008d0c.c -- same exemplar, same
 * two-line residue, OPPOSITE DIRECTION. That pairing is the finding.
 *
 * 84 lines against the ROM's 84. The whole residue:
 *
 *     rom    str r3, [r6, #8] / mov r1, #0xf0
 *     ours   mov r1, #0xf0    / str r3, [r6, #8]
 *
 * gcc hoists the angle mask's constant build one slot, into the tail of the
 * three vector stores. The ROM emits it after them.
 *
 * THE MIRROR. 2008d0c is the same exemplar's shape and also sits at exactly 2
 * differing, and there the ROM wants the constant build EARLIER than gcc puts
 * it (`mov r3, #0x80` before `ldrh r1, [r5, #6]`, where ours emits the load
 * first). Two functions, one exemplar, two-line residues pointing opposite
 * ways, and in both cases every spelling tried moves the count the wrong way
 * or not at all.
 *
 * That is what makes this a scheduling wall rather than a missing lever: if a
 * source construct controlled where an independent constant build lands, one
 * of the two would have yielded to it.
 *
 * MEASURED here, all 84 lines:
 *   as written                                                  2 differing
 *   the mask expression written inline in the call argument     6
 *   the halfword read named into its own local first            4
 *
 * WHAT IS RIGHT: the gState offset is NAMED (`off = 0xfa << 1;`), without which
 * `gState + (0xfa << 1)` folds to a single pooled symbol and the function
 * screens nowhere near. That lever is now worth checking first on anything
 * that indexes gState.
 *
 * NEXT: nothing source-level. See the companion park for the same conclusion
 * reached from the other side.
 */
extern unsigned char gState[];
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(char *, int);
extern void __WaitFrames(int);
extern void __PlaySound(int);
extern void __Actor_SetSpriteFlags(char *, int);
extern void __Func_8092158(int, int, int);
extern void __vec3_translate(int, int, int *);
extern int __TestCollision(char *, int *);

void OvlFunc_957_2008de8(void)
{
    char *p;
    int vec[3];
    int *v;
    unsigned char *f;
    int saved;
    int ang;
    int off;

    off = 0xfa << 1;
    p = __MapActor_GetActor(*(int *)(gState + off));
    f = (unsigned char *)(p + 0x55);
    saved = *f;
    v = vec;
    v[0] = *(int *)(p + 8);
    v[1] = *(int *)(p + 0xc);
    v[2] = *(int *)(p + 0x10);
    ang = *(unsigned short *)(p + 6) & (0xf0 << 8);
    __vec3_translate(0x80 << 14, ang, v);
    if (__TestCollision(p, v) == 0) {
        __CutsceneStart();
        __Actor_SetAnim(p, 6);
        __WaitFrames(6);
        __PlaySound(0x98);
        __Actor_SetAnim(p, 7);
        *(int *)(p + 0x30) = 0xc0 << 10;
        *(int *)(p + 0x34) = 0x80 << 10;
        *(int *)(p + 0x28) = 0x80 << 11;
        *f = *f & 0x7e;
        __Actor_SetSpriteFlags(p, 0);
        __Func_8092158(0, *(short *)((char *)v + 2), *(short *)((char *)v + 0xa));
        __Actor_SetAnim(p, 6);
        __Actor_SetSpriteFlags(p, 1);
        *f = saved;
        __CutsceneEnd();
    }
}
