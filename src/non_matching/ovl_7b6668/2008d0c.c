/* OvlFunc_928_2008d0c (0x02008d0c) -- NON-MATCHING.
 * Blocker class: instruction placement (pre-reload scheduling), TWO lines.
 *
 * 95 lines against the ROM's 95, and the entire residue is the order of two
 * independent instructions at the start of the angle computation:
 *
 *     rom    mov r3, #0x80 / ldrh r1, [r5, #6] / lsl r3, #6 / add r1, r3
 *     ours   ldrh r1, [r5, #6] / mov r3, #0x80 / lsl r3, #6 / add r1, r3
 *
 * Everything else -- the flag guard, the three vector stores, the collision
 * test, and the whole nineteen-call tail -- is exact.
 *
 * WHAT MAKES THIS WORTH A FILE: the exemplar has the SAME EXPRESSION and it
 * matched. OvlFunc_946_2009a44 (src/overlays/rom_7ced6c/...a_a.c) writes
 * `ang = (*(unsigned short *)(p + 6) + (0x80 << 6)) & (0xc0 << 8);` verbatim
 * and gcc emits the constant first there. So the spelling is not the variable;
 * the surrounding pressure is. This function differs from the exemplar by an
 * enclosing __GetFlag guard and by fetching its actor rather than receiving it,
 * and one of those changes what the scheduler does with two instructions that
 * have no dependency between them.
 *
 * MEASURED, all 95 lines:
 *   as written, matching the exemplar's expression               2 differing
 *   `(0x80 << 6) + *(unsigned short *)(p + 6)` (operands swapped)  2
 *   the 0x80 << 6 named in a local assigned just before           2
 *   the 0xc0 << 8 mask named                        first diff at 9, 11
 *   the whole `ang = ...` moved above the three vector stores
 *                                                   first diff at 16, 24
 *   -fno-schedule-insns                                           2
 *   -fno-schedule-insns2                            first diff at 3, 21
 *
 * The last line is the documented "destroying the evidence" signature -- the
 * first difference jumps toward instruction 1 and the count multiplies -- so
 * per the rule recorded in batch 168 that flag is not the answer here, and the
 * two spellings that move the count at all both move it the wrong way.
 *
 * BATCH 204 -- THE SCHEDULING BARRIER DOES NOT REACH THIS, and the reason
 * bounds a lever that had just worked elsewhere. `do { } while (0)` closed
 * seven of eleven differing lines in src/non_matching/overlays/200db90.c, whose
 * blocker this park's own text describes in nearly the same words. It is inert
 * here, in three forms, all byte-identical to the 2 already recorded:
 *
 *     a barrier immediately before the `ang = ...` statement          2
 *     the 0x80 << 6 named in a local WITH a barrier after it          2
 *     that local pinned to r3, which is the register the ROM uses     2
 *
 * THE DIFFERENCE IS WHAT THE BARRIER HAS TO SEPARATE. In 200db90 the hoist
 * crossed STATEMENT BOUNDARIES -- two loads belonging to the last statement had
 * migrated up past three stores -- and a barrier between those statements put
 * them back. Here the two instructions are the operands of ONE expression,
 * `*(unsigned short *)(p + 6) + (0x80 << 6)`, and there is no statement
 * boundary between them to place a barrier on. Naming one operand in its own
 * statement does not help either, because the scheduler settles their order
 * after the source shape is gone.
 *
 * So the barrier orders STATEMENTS, not the operands within one. That is
 * consistent with what it is -- a scheduling-region split -- and it is the
 * cheap test for whether to reach for it: is there a statement boundary the
 * two instructions belong to opposite sides of?
 *
 * NEXT: nothing source-level. If the exemplar's context is ever characterised
 * well enough to say WHICH of the two differences (the guard, or the fetched
 * actor) frees the scheduler, this closes with it.
 */
extern char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(char *, int);
extern void __WaitFrames(int);
extern void __PlaySound(int);
extern void __Actor_SetSpriteFlags(char *, int);
extern void __Func_8092158(int, int, int);
extern void __vec3_translate(int, int, int *);
extern int __TestCollision(char *, int *);

void OvlFunc_928_2008d0c(void)
{
    char *p;
    int vec[3];
    int *v;
    unsigned char *f;
    int saved;
    int ang;

    p = __MapActor_GetActor(0);
    f = (unsigned char *)(p + 0x55);
    saved = *f;
    if (__GetFlag(0x80 << 2) != 0) {
        v = vec;
        v[0] = (*(int *)(p + 8) & 0xfff00000) + (0x80 << 12);
        v[1] = *(int *)(p + 0xc);
        v[2] = (*(int *)(p + 0x10) & 0xfff00000) + (0x80 << 12);
        ang = (*(unsigned short *)(p + 6) + (0x80 << 6)) & (0xc0 << 8);
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
}
