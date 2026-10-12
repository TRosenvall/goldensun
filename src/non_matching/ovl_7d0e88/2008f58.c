/* OvlFunc_947_2008f58 -- NON-MATCHING, 8 differing STREAM encodings of 53.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/N8f58b_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 53 / ours 53 stream encodings; 48/48 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7d0e88/2008f58.c \
 *     asm/overlays/rom_7d0e88/ovl_314_c_a_a_a_c_a.s --func OvlFunc_947_2008f58
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
struct Blk {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
};

extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2008ddc(int a, int *b, int *c, struct Blk *d, int *e, int *f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_947_2008528(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_947_2008f58(int a)
{
    struct Blk t;
    int v14;
    int v10;
    int vc;
    int v8;
    unsigned char *e;
    int p;
    int q;

    e = __MapActor_GetActor(a);
    if (OvlFunc_947_2008ddc(a, &v14, &v10, &t, &vc, &v8) == 0)
        return 0;
    q = t.f10;
    p = t.f8;
    __Func_8010704(vc + p, v8 + q, v14, v10, p, q);
    OvlFunc_947_2008528(0, t.f8, t.f10, v14, v10, 0xff);
    __Actor_SetAnim(e, 1);
    e[0x23] &= 0xfd;
    return 1;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_947_2008f58 -- 0x02008f58,
 * asm/overlays/rom_7d0e88/ovl_314_c_a_a_a_c.s
 *
 * 55 of 55 lines, EIGHT differing, and all eight are two held values in swapped
 * registers.  Candidate at scratch/N8f58b_best.c.
 *
 *      rom   ldr r4, [r6, #0x10] / ldr r5, [r6, #0x8] ... add r1, r4 / add r0, r5
 *      ours  ldr r4, [r6, #0x8]  / ldr r5, [r6, #0x10] ... add r0, r4 / add r1, r5
 *
 * The two struct fields feed both an addition and a stack slot each, so they are
 * named locals by the batch-149 reading -- and that part is right: the
 * `str r5, [sp] / str r4, [sp, #4]` pair reproduces.  Only which field gets
 * which register differs.
 *
 * SCREENED AND INERT, all still 8: the two assignments in either order, the two
 * declarations in either order, both together, and the two sums computed into
 * named locals in the ROM's evaluation order to force which addition happens
 * first.  This is the coin flip of src/non_matching/ovl_7ced6c/2009c84.c,
 * .../200a16c.c and .../20096a8.c -- four functions now, and the source has no
 * handle on it in any of them.
 *
 * SOLVED and not to be re-derived: the 0x30-byte frame is eight bytes of
 * outgoing stack-argument area, four address-taken int locals, and a 0x18-byte
 * struct also passed by address to OvlFunc_947_2008ddc; the six-argument call
 * takes its two stack slots from the SAME registers the additions use, so they
 * are shared locals rather than a fresh pair; and the trailing `e[0x23] &= 0xfd`
 * gives the ROM's `mov r3, #0xfd / and r3, r2 / strb`.
 *
 * WHY IT IS HERE: five calls, where the candidate filter required eight until
 * this round.  Like OvlFunc_951_20096a8 it is a fair test of the relaxed floor
 * and it failed on a wall that has nothing to do with call density.
 *)

*/
#endif
