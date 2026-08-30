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
  *
 * whodoesthis.py RESULT: a well-controlled zero, which is the strongest
 * evidence assembled for any residue here so far -- and still not proof.
 *
 * The residue needs a load from [base, #4] emitted while a store to [base] is
 * still pending. Across every generated .s in the tree:
 *
 *     str rN, [rM]                                  -> 263 functions
 *     ldr [rB,#4] ... str (any)   within 2 insns    ->  22 functions
 *     ldr [rB,#4] ... str [rB,#N] within 2 insns    ->   8 functions
 *     ldr [rB,#4] ... str [rB]    within 2 insns    ->   0 functions
 *
 * The first three are positive controls proving the search works and the
 * pattern syntax is right. The fourth is the shape needed. All eight hits from
 * the third are read-modify-write on the SAME slot, not a hoist.
 *
 * Reading that as gcc's aliasing being conservative about a store through an
 * `int *` with no offset: it will not move a later load above it.
 *
 * FOLLOWS FROM THAT, and measured: if the load cannot be hoisted, the source
 * must read out[1] BEFORE the store is issued. Written that way --
 * `t = out[0] - a; u = out[1]; out[0] = t; out[2] = out[2] - u - b;` -- the
 * load does move, and the first difference goes from line 35 to line 29. But
 * the count goes from 2 differing to 6, because naming the two temporaries
 * shifts which register holds the masked value. The 2-differing version below
 * is kept as best.
 *
 * Retyping the object as a struct -- the alias-set lever that closed
 * OvlFunc_964_2008cd0 -- is byte-identical here. Distinct fields of one struct
 * are not a distinct alias set for this purpose.
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
