/* OvlFunc_924_20097a8 (0x020097a8) -- NON-MATCHING: 113 encodings of 151 differ (objcmp).
 * SAME LENGTH (336 bytes, 151 encodings, all 15 relocations identical and in order).
 *
 * asm/overlays/rom_7ac2d8/ovl_f84_c_a_a.s -- ONE function, no data.  CONVERTS WHOLE
 * (use --whole, which is the real verdict and also checks the section tail).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/20097a8.c asm/overlays/rom_7ac2d8/ovl_f84_c_a_a.s --whole
 * (in the container: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build ...)
 *
 * FAMILY: the near-twin of src/non_matching/ovl_7ac2d8/2009db4.c -- same bank, same
 * OvlFunc_common0_10c 8-argument call, the same `((__Random() << 3) >> 16) * 0x3333 -
 * 0xcccc` pair, the same `struct P` padded to 0x28, and the same `down -= 0x10000`
 * accumulator.  Everything that park's header lists as inert/worse was not re-tried.
 * What is NEW here is a second call in the inner loop (OvlFunc_924_200bb24) whose
 * argument is NUMERICALLY THE SAME VALUE as the accumulator, computed a different way.
 *
 * ================================================================
 * NEW LEVER, MEASURED: THE INNER LOOP MUST BE A `goto` LOOP, AND THE REASON IS
 * loop.c:4544 -- NOT A SPELLING
 * ================================================================
 *
 * Written as a real `do { } while (j <= 7)`, gcc strength-reduces the 200bb24 argument
 * into a SECOND induction variable alongside the accumulator.  It never combines the
 * two (both get `add rN, #-0x10000`), which spills and grows the frame from 0x3c to
 * 0x40.  The ROM has ONE induction variable and recomputes the other argument.
 *
 * The gate is loop.c:4544:
 *     if (! flag_reduce_all_givs && v->lifetime * threshold * benefit < insn_count
 *         && ! bl->reversed)   ->  v->ignore = 1;   (recompute instead of reduce)
 * with threshold = (has_call ? 1 : 2) * (3 + n_non_fixed_regs) (loop.c:3862).  With a
 * call in the loop threshold is still ~16, benefit ~4-5 and insn_count ~60, so
 * 16*4 > 60 and the giv is ALWAYS reduced.  Nothing in the source moves that product.
 *
 * MEASURED, all against ref 151 encodings / 336 bytes:
 *   do/while inner + `0xb6*0x40000 - (k+j)*0x10000`         97 of 151, 151 insns  (t5_c)
 *   do/while inner + `((-j - k) << 16) + 0xb6*0x40000`     123 of 151, 154 insns  (spills)
 *   the same with a named local for the argument           123 of 151, 154 insns  (inert)
 *   the same with the 200bb24 call in BOTH if/else arms     84 of 151, 156 insns
 *   BOTH loops as goto loops                               126 of 151, 149 insns
 *   inner goto + `0xb6*0x40000 - (k+j)*0x10000`            117 of 151, 149 insns
 *   inner goto + `((-j - k) << 16) + 0xb6*0x40000`         113 of 151, 151 insns  <- THIS
 *   THIS + the 200bb24 call in both arms                   109 of 151, 162 insns
 *   THIS + block-scoped locals for CopyMapTiles' 2 and 1   122 of 151, 151 insns
 *   THIS + `z = ((-(int)(i*16)) << 16) + 0xb6*0x40000`      89 of 151, 156 insns
 *
 * THIS CANDIDATE IS PARKED RATHER THAN t5_c's 97, AND THAT IS DELIBERATE.  Both first
 * diverge at index 33 and both have the right length, but with the inner loop as a goto
 * loop the WHOLE 200bb24 argument comes out instruction-for-instruction as the ROM's
 *     neg r6,r7 / mov r2,r9 / sub r0,r6,r2 / mov r3,#0xb6 / lsl r3,#18 / lsl r0,#16 /
 *     mov r2,#0x92 / lsl r2,#18 / add r0,r3 / mov r1,#0 / bl
 * (verified against a -fno-strength-reduce build of the do/while version, which
 * reproduces that block exactly).  t5_c's lower count carries an entire surplus
 * induction variable.  A count is not a difference.
 *
 * ALL 24 DECLARATION ORDERS OF {i, j, k, z} ARE INERT (113 every time) -- the
 * documented rule that permutation is inert when the locals are all register-resident.
 *
 * WHAT IS LEFT, read off a side-by-side against the ROM (four items, all in the loop):
 *  1. REGISTER ROLES.  ROM: i=r10, j=r7, z=r8, i*16=r9.  Ours: i=r8, j=r6, z=r7,
 *     i*16=r9, and the constant 1 parked in r10.
 *  2. THE CONSTANT 1 IS CSE'd ACROSS THE INNER LOOP.  Ours keeps `1` live in r10 for
 *     both `j & 1` and CopyMapTiles' sixth argument (`mov r3,r10 / str r3,[sp,#4]`);
 *     the ROM re-materialises it at both (`mov r2,#1` in the loop, `mov r3,#1` at the
 *     call).  With the inner loop a goto loop this is gcse, not LICM.
 *  3. `neg rN, j` IS DUPLICATED INTO BOTH ARMS in the ROM (`b .L186a` over a one-insn
 *     else block) and appears once, at the join, in ours.  Writing the 200bb24 call in
 *     both arms does NOT reproduce it -- jump2 leaves two full copies because the
 *     register choices differ by then (162 insns).
 *  4. `z`'s INITIALISER IS ONE INSN SHORT.  ROM: `lsl r3,r2,#20 / mov r2,#0xb6 /
 *     neg r3,r3 / lsl r2,#18 / add r2,r3` (neg then add); ours folds to `sub`.  The
 *     `((-(int)(i*16)) << 16) + base` spelling gets neg+add but costs 5 elsewhere.
 *
 * NEXT: items 1 and 2 look like one decision (what the allocator keeps live across the
 * inner loop), the same shape as src/non_matching/overlays/2008098.c's blocker.  Item 3
 * is unexplained: no source shape tried produces an instruction duplicated into both
 * arms of an if/else.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern unsigned int __Random(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern int __StartTask(void *fn, int n);
extern void __Func_8012350(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct P *p);
extern void OvlFunc_924_200bb24(int a, int b, int c);
extern void OvlFunc_924_20095e0(int a, int b, int c);
extern void OvlFunc_924_2009790(void);

void OvlFunc_924_20097a8(int a)
{
    struct P p;
    unsigned int i;
    unsigned int j;
    int k;
    int z;

    __CopyMapTiles(0x4e, 0x3b, 0x6e, 0x24, 1, 1);
    __CopyMapTiles(0x4c, 0x3b, 0x6d, 0x24, 1, 1);
    p.f4 = 7;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    i = 0;
    do {
        k = i * 16;
        z = 0xb6 * 0x40000 - (i << 20);
        j = 0;
    loop_j:
            if (j & 1) {
                OvlFunc_common0_10c(z, 0, 0x92 * 0x40000,
                                    ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                    ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                __CutsceneWait(1);
            }
            OvlFunc_924_200bb24(((-(int)j - k) << 16) + 0xb6 * 0x40000, 0, 0x92 * 0x40000);
            z -= 0x10000;
            j++;
        if (j <= 7)
            goto loop_j;
        __CopyMapTiles(0x4c, 0x3b, 0x6c - i, 0x24, 2, 1);
        i++;
        OvlFunc_924_20095e0(a, i - 1, i);
    } while (i <= 1);
    __CutsceneWait(a);
    OvlFunc_924_20095e0(0, i, i + 1);
    __PlaySound(0xd3);
    __StartTask(OvlFunc_924_2009790, 0xc80);
    __Func_8012350();
}
