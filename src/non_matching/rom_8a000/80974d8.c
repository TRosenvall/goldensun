/*
 * Func_80974d8  (PrepareEncounterTransition) -- asm/rom_8a000/rom_97384_c_a_a_a.s
 *
 * BLOCKER: instruction scheduling. 48 lines against 48, TWO differing, and
 * they are one adjacent pair:
 *
 *      rom   ldr r2, [r6, #0x4] / str r3, [r6, #0x0]
 *      ours  str r3, [r6, #0x0] / ldr r2, [r6, #0x4]
 *
 * The ROM loads out[1] BEFORE storing out[0]; we store first. Both are
 * correct -- the indices provably do not alias -- so gcc is free either way
 * and picks the other one.
 *
 * SETTLED, and it is the reusable half:
 *
 *   The second global is reached by DERIVING its address. The ROM loads
 *   &iwram_3001ebc, reads through it, then `sub r3, #0x4c` and reads again,
 *   giving the global at 0x3001e70. Two separate externs cannot produce that;
 *   gcc has no reason to believe they are 0x4c apart. Writing
 *   `*(unsigned char **)((char *)&iwram_3001ebc - 0x4c)` reproduces it exactly.
 *   Second use of this lever, after Func_801eea0.
 *
 *   gcc tail-merges the `str r3, [r6, #8]` from both arms by itself. Writing
 *   `out[2] = ...` in each branch and writing it once through a temporary
 *   produce BYTE-IDENTICAL output, so the merge is not something the source
 *   has to arrange. The direct form is kept here because it is the clearer C.
 *
 * TRIED AND REJECTED, all measured, all on the same two instructions:
 *
 *   * `c = out[1];` named before the out[0] store, to force the load early.
 *     WORSE -- 5 differing; gcc hoists it too far, ahead of the out[0] load.
 *   * `out[0] -= a;` instead of `out[0] = out[0] - a;`. No change at all.
 *   * Computing into a temporary and sinking the store past both loads
 *     (`t = out[0] - a; ...; out[0] = t;`). WORSE -- 9 differing.
 *
 * Three spellings that move the store, and none lands it in the one slot
 * between the subtract and the out[2] load. This looks like scheduler
 * behaviour rather than anything the source controls, but at 2 of 48 it is
 * worth another attempt when a new lever appears.
 */
extern unsigned char *iwram_3001ebc;
extern void PhysMove(int *out, int *buf);

void Func_80974d8(int *out)
{
    int buf[3];
    unsigned char *p;
    unsigned char *q;
    int a;
    int b;

    p = iwram_3001ebc;
    if (*(short *)(p + (0xcf << 1)) == 3) {
        PhysMove(out, buf);
        out[0] = buf[0] << 16;
        out[2] = buf[1] << 16;
    } else {
        q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
        a = *(int *)(q + 0xe4) & 0xffff0000;
        b = *(int *)(q + 0xe8) & 0xffff0000;
        out[0] = out[0] - a;
        out[2] = out[2] - out[1] - b;
    }
    out[1] = 0;
}
