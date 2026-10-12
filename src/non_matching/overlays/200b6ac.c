/* OvlFunc_924_200b6ac -- NON-MATCHING, 16 differing STREAM encodings of 90.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Lb6ac.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 90 / ours 90 stream encodings; 69/69 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200b6ac.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_a.s --func OvlFunc_924_200b6ac
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned int gState;
extern int _AREA_36;
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void OvlFunc_common0_18(int a, int b, int c, int d);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092708(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_924_200b6ac(void)
{
    unsigned char *a;
    unsigned int q;
    unsigned int off;
    int v;
    int a1, a2, b1, c0, c2, d1, d2;
    int e, f;

    a1 = 0xec << 1;
    a2 = 0x96 << 2;
    b1 = 0x80 << 7;
    c0 = 0xe8 << 17;
    c2 = 0xa4 << 18;
    d1 = -1;
    d2 = -1;
    __CutsceneStart();
    q = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    q += off;
    v = *(short *)q;
    if (v == (int)(&_AREA_36)) {
        __Func_80921c4(0, a1, a2);
        __Func_8092adc(0, b1, 0xa);
        __Func_80933f8(c0, d1, c2, 1);
        a = __MapActor_GetActor(0);
        __Actor_SetSpriteFlags(a, 0);
        a = __MapActor_GetActor(0);
        OvlFunc_common0_18(*(int *)(a + 8), 0, 0x2be0000, 0xdf);
        e = 3;
        f = 2;
        __CopyMapTiles(0x5c, 0x2e, 0x5c, 0x28, e, f);
        a = __MapActor_GetActor(0);
        *(int *)(a + 0x48) = 0x80 << 8;
        __Func_8092b08(0, 2);
        __Func_8092708(0, 6, d1);
        q = (unsigned int)iwram_3001ebc;
        off = 0xe0;
        off <<= 1;
        q += off;
        off += 0x43;
        *(int *)q = off;
        __CutsceneWait(0x3c);
        __Func_8091e9c(8);
    } else {
        __Func_8092708(0, 6, d2);
    }
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_924_200b6ac -- 0x0200b6ac,
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_a.s
 *
 * 87 of 87 lines, 15 differing.  Candidate at scratch/Lb6ac.c.
 *
 * SOLVED, and worth reusing:
 *   - All FOUR interleaved argument sites reproduce with the batch-127 lever
 *     (named locals at the top, the function's `bne` between them and the
 *     calls), including the four-argument one where r0, r1 and r2 are all split
 *     builds: `mov r0,#0xe8 / mov r1,#1 / mov r2,#0xa4 / lsl r2,#18 /
 *     mov r3,#1 / neg r1,r1 / lsl r0,#17`.
 *   - The area id is a SYMBOL.  The ROM pool-loads 0x36 where `cmp r2, #0x36`
 *     would do, which is the tell area.sym's header documents; `_AREA_36` was
 *     already defined.  Spelling the test `v == (int)(&_AREA_36)` took the
 *     screen from 78 differing to 20.
 *   - The two stack arguments of __CopyMapTiles must be NAMED LOCALS assigned
 *     adjacent to the call.  The ROM materialises both before storing either
 *     (`mov r3,#3 / mov r2,#2 / str r3,[sp] / str r2,[sp,#4]`); inline
 *     literals give store-then-rebuild.  20 -> 15.  Same note as the
 *     OvlFunc_959_200a308 template.
 *   - The value stored at iwram+0x1c0 is DERIVED from the offset
 *     (`add r3,r2 / add r2,#0x43 / str r2,[r3]`, 0x1c0 + 0x43 = 0x203) and
 *     reproduces from a mutated offset variable, as the batch-123 rule predicts.
 *
 * BLOCKER: register roles in the two address chains, plus one argument
 * evaluation order.
 *      rom   ldr r3,=gState / mov r1,#0xe0 / lsl r1,#1 / add r3,r1
 *      ours  ldr r2,=gState / mov r3,#0xe0 / lsl r3,#1 / add r2,r3
 * The ROM keeps the base in r3 and the offset in r1/r2; ours swaps them.  Same
 * swap in the iwram chain.  And at OvlFunc_common0_18 the ROM emits the memory
 * load LAST of the four arguments where gcc emits it first.
 *
 * A NEGATIVE WORTH RECORDING: giving each address chain its own pair of locals
 * -- which is the batch-124 rule, and which was exact on OvlFunc_923_200a370 --
 * makes this WORSE (84 lines, 86 differing).  The two chains here are in
 * different basic blocks and never live at once, so splitting them only adds
 * pseudos.  The rule holds for chains that overlap, not for chains that merely
 * both exist.
 *
 * ALSO TRIED: swapping the two scratch declarations; naming the loaded argument
 * as a local before the call.  Both 15.
 *)

*/
#endif
