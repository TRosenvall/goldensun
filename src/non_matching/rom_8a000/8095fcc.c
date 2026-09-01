/* Func_8095fcc (0x08095fcc) -- NON-MATCHING.
 * Blocker class: post-reload scheduling of ONE register copy.
 *
 * 54 lines against the ROM's 54, THREE differing, and the three are one
 * instruction moved by one slot:
 *
 *     rom    ldrh r1,[r3] / sub r1,#1 / strh r1,[r3] / mov r6, r0
 *     ours   ldrh r1,[r3] / mov r6, r0 / sub r1,#1 / strh r1,[r3]
 *
 * `mov r6, r0` saves the MapActor_GetActor result into the local the three
 * later field reads use. Its first use in the source is already after the
 * halfword decrement; gcc simply fills the ldrh/sub gap with it.
 *
 * THE LEVER THAT GOT IT HERE, and it is the whole difference between 53
 * differing and 3: THE gState OFFSET MUST BE BUILT, NOT FOLDED.
 * `*(int *)(gState + (0xfa << 1))` pools `gState+500` as one symbol and the
 * function comes out 51 lines against 54. Naming the offset first --
 * `off = 0xfa << 1; ... gState + off` -- gives the ROM's
 * `mov r1, #0xfa / lsl r1, #1 / add r3, r1` and takes it to 54 lines and 3.
 * The body was already exact underneath; the three missing instructions were
 * hiding the fact.
 *
 * MEASURED against the remaining copy, all 54 lines and 3 differing:
 *   `p = v;` moved before the decrement
 *   the named vector pointer deleted entirely (`v[0]`, `v[2]` direct)
 *   the address local `ph` deleted (`*(short *)(e + 0x64)` written out)
 *   `a` declared last among the locals
 *   -fno-schedule-insns
 * And two that are worse: a named `t` for the decremented value (55 lines, 44),
 * -fno-schedule-insns2 (28 differing from instruction 1).
 *
 * Four independent source spellings at exactly 3 is the signature of the
 * post-reload scheduler, which docs/elevation.md already records as out of
 * reach from statement order (see src/non_matching/ovl_797990/2008c1c.c, the
 * same class).
 *
 * WHAT IS RIGHT: the halfword decrement through a `short *` gives the ROM's
 * `ldrh` for the read and the `lsl #16 / asr #16` sign-extension at the later
 * use with no reload, exactly as the solved Func_80993b0 does for its angle;
 * v[1] is genuinely never written; and the 0xffff0000 addition pools as the
 * ROM does.
 *
 * NEXT: nothing source-level outstanding.
 */
extern unsigned char gState[];
extern unsigned char *MapActor_GetActor(int slot);
extern void vec3_translate(int a, int b, int *v);
extern void _DeleteActor(unsigned char *e);

void Func_8095fcc(unsigned char *e)
{
    unsigned char *a;
    short *ph;
    int v[3];
    int *p;
    int x;
    int off;

    off = 0xfa << 1;
    a = MapActor_GetActor(*(int *)(gState + off));
    ph = (short *)(e + 0x64);
    *ph = *ph - 1;
    p = v;
    p[0] = *(int *)(a + 8);
    p[2] = *(int *)(a + 0x10);
    vec3_translate(*ph * 0x6666, (*ph << 11) + *(short *)(e + 0x66), p);
    *(int *)(e + 8) = p[0];
    *(int *)(e + 0x10) = p[2];
    x = *(int *)(e + 0xc) + 0xffff0000;
    *(int *)(e + 0xc) = x;
    if (x < *(int *)(a + 0xc) + (0xa0 << 13))
        _DeleteActor(e);
}
