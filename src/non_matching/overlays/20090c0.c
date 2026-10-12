/* OvlFunc_924_20090c0 -- NOT MATCHING, a SIZE mismatch: ref 164 bytes against ours 160.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L90c0.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/20090c0.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_b.s --func OvlFunc_924_20090c0
 *
 * The original prose park is preserved below.
 */
extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8012350(void);
extern void __Func_8091e9c(int n);

void OvlFunc_924_20090c0(void)
{
    unsigned int q;
    unsigned int off;
    int a1, a2, a3;
    int m, n;

    a1 = 0x80 << 9;
    a2 = 0x80 << 9;
    a3 = 0x80 << 9;
    if (__GetFlag(0xc4 << 2) != 0 && __GetFlag(0x311) != 0 && __GetFlag(0x312) != 0) {
        __SetFlag(0x876);
        __CutsceneWait(0x1e);
        __Func_8012330(a1, a2, a3);
        __PlaySound(0x8d);
        __CutsceneWait(0x3c);
        q = (unsigned int)iwram_3001ebc;
        off = 0xe0;
        off <<= 1;
        q += off;
        off -= 0xc0;
        *(int *)q = off;
        __MapTransitionOut();
        __WaitMapTransition();
        __PlaySound(0x121);
        m = 1;
        m = -m;
        n = 1;
        n = -n;
        __Func_8012330(m, n, 0xe666);
        __Func_8012350();
        __Func_8091e9c(0xd);
    } else {
        __ClearFlag(0x876);
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_924_20090c0 -- 0x020090c0,
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a.s
 *
 * 55 vs 56 lines, 22 differing.  Candidate at scratch/L90c0.c.
 *
 * SOLVED: the three-guard cascade, the three separately-named copies of
 * 0x80 << 9 for the first __Func_8012330, and the derived store
 * (`add r3, r2 / sub r2, #0xc0 / str r2, [r3]`, 0x1c0 - 0xc0 = 0x100) which
 * reproduces from a mutated offset variable as the batch-123 rule predicts.
 *
 * BLOCKER: register roles in the iwram address chain, plus the order of two
 * mov+neg builds.
 *      rom   mov r2,#0xe0 / ldr r3,[r3] / lsl r2,#1 / add r3,r2 / sub r2,#0xc0
 *      ours  ldr r2,[r3]  / mov r3,#0xe0 / lsl r3,#1 / add r2,r3 / sub r3,#0xc0
 * The ROM keeps the base in r3 and the offset in r2; ours swaps them, which
 * then swaps the store's operands.
 *
 * TRIED: swapping the declaration order of the base and offset locals;
 * reordering the two `m = 1; m = -m;` pairs so both movs precede both negs;
 * --no-rerun-cse.  All 22.
 *
 * Same wall as src/non_matching/overlays/200807c.c and
 * src/non_matching/rom_a1000/80a5fe0.c: the source can move an allocation but
 * cannot choose it.
 *)

*/
#endif
