/* OvlFunc_common1_2060 -- NON-MATCHING, 91 differing STREAM encodings of 163.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b258/common/final/park_common1_2060.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 163 / ours 163 stream encodings; 146/146 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_common/common1_2060.c \
 *     asm/overlays/common/common1_c_c_b_a.s --func OvlFunc_common1_2060
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
/* OvlFunc_common1_2060 -- asm/overlays/common/common1_c_c_b.s
 *
 * PARKED at 20 of 162 instructions in disagreeing regions, LENGTH EXACT
 * (162 lines against 162, 163 encodings against 163, size identical).
 * objcmp, measured 3x:
 *   XX ENCODINGS differ in 91 place(s) (ref 163, ours 163)
 *      first at index 11: ref 6818  ours 2100
 *   XX RELOCATIONS differ
 * The 91 is positional: one two-instruction displacement near the top shifts
 * everything after it.  The relocation line differs only in OFFSETS -- both
 * sides carry exactly gState and .L9, nothing is missing or extra -- which is
 * the same two-instruction drift moving the literal pool.
 *
 * BLOCKER CLASS: RELOAD ROUND ROBIN (docs "A RELOAD SCRATCH REGISTER IS ROUND
 * ROBIN OVER A SET THE SOURCE CONTROLS"), plus one constant-placement site.
 *
 * DISCRIMINATOR THAT FIRED: -fno-schedule-insns2 REGRESSES 20 -> 42.  Read the
 * sign: sched2 is already producing the ROM's order and alias is the wrong
 * axis.  -fno-rerun-cse-after-loop is INERT at 20.  Every remaining difference
 * is which low register a reload got.
 *
 * THE FLOOR, site by site (total 20):
 *
 *   a. 4 -- `zero` is materialised at the TOP of the function where the ROM
 *      materialises it at .L2118, interleaved with the `b + 0x22` address
 *      build (`mov r3, #0 / mov r2, r6 / add r2, #0x22 / mov r11, r3`).
 *      Moving the assignment to .L2118 DOES put it there, but costs an extra
 *      `mov r3, sl` at the top (163 lines) -- a coupling I could not break.
 *      Eleven placements measured: after __MapActor_GetActor and after
 *      `tbl =` are both 20; at the top / after `off =` / after `mask =` 23;
 *      at .L2118 either side of the strb 27; before __TestCollision 32;
 *      before `speed =` 82.  Literal zeros at both stores (letting gcse
 *      common them) 31; two separate zero locals 27; `zero` uninitialised 25.
 *
 *   b. 9 -- `lsl r2, r1, #0x10` where the ROM has the DESTRUCTIVE `lsl r1,
 *      #0x10`, at all THREE copies of the v[] fill block.  Both registers are
 *      free; it is purely which one the shift's output pseudo got.  INERT:
 *      a `lo` local, a `hi` local, both, `t = t << 16`, `t <<= 16`,
 *      `t * 0x10000`, `(unsigned)t << 16`, `unsigned int t`,
 *      `unsigned int mask`, `(mask & t)`, a struct for v[], `&v[0]`.
 *      WORSE: pinning `t` to r0/r1/r2/r3 (44-61), reversing the `+` operands
 *      (44).
 *
 *   c. 2 -- `mov r2, r9` where the ROM has `mov r0, r9`, at the THIRD table
 *      read only (the first two match).
 *   d. 2 -- `ldr r3, [r7, #8]` one position late before __Actor_TravelTo.
 *   e. 4 -- `mov r1, r11` where the ROM has `mov r2, r11` at the closing
 *      zero stores, with the two `str` following.
 *
 * c, d and e are the same round-robin phase as b.  Per the documented cure the
 * fix is to ADD A RELOAD SOMEWHERE ELSE, not to respell these sites; I could
 * not find the place.  Everything tried and INERT at 20: `int *pv = v`,
 * `&v[0]`, a `short`/`int` local for the table index, a dummy extra stack
 * local, `int *tbl` instead of `unsigned char *`, and ALL EIGHTEEN
 * combinations of prototype/int-return/implicit over OvlFunc_common1_2018,
 * __TestCollision and __MapActor_GetActor (the call fill orders already match,
 * so the declaration lever has nothing to buy here).
 *
 * SOLVED AND WORTH KEEPING -- the push mask.  The first transcription pushed
 * r8-r10 where the ROM pushes r8-r11, and the missing value is the .L9 table
 * BASE: written `tbl[idx]` gcc commons `&tbl[idx]` into one register and loads
 * `ldr r1, [r5]`, where the ROM keeps base in r9 and idx*4 in r5 and loads
 * `ldr r1, [r2, r5]`.  SPLITTING the address into a base pointer and a
 * byte offset (`off = (h >> 12) * 4; t = *(int *)(tbl + off);`) restores the
 * register-offset form and the fourth push: 41 -> 27.  That is the 148-park
 * "name the address, not the offset" lever run in its OTHER direction, and it
 * is the second time in this batch that the same axis decided a function.
 *
 * ALSO SOLVED: `extern int L9[] __asm__(".L9");` is the right spelling for the
 * in-file data label -- `.L9` is already `.global` in the reference's .data
 * section and the relocation comes out against `.L9` on both sides.
 *
 * LANDING, if it is ever closed: common1_c_c_b.s is WHOLE+DATA and NEEDS A
 * HAND SPLIT.  It holds OvlFunc_common1_2060, OvlFunc_common1_21c8, and
 * .data + .data1 + .bss (31 .lcomm symbols and eleven .incbin ranges out of
 * overlays/rom_7db0c8/orig.bin).  2060 is FIRST, so the shape is
 * common1_c_c_b_a.c (2060) + common1_c_c_b_b.s (21c8 and ALL THREE data
 * sections), and FOUR linker lines change in EACH of the three overlay.ld
 * files that name the object -- the .text line splits in two and the .data,
 * .data1 and .bss lines all move to _b.  2060 itself reads .L9 out of that
 * .data, so the data must stay in the asm sibling.
 */

extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *OvlFunc_common1_2018(int *v, unsigned char *a);
extern int __TestCollision(unsigned char *a, int *v);
extern void __Actor_SetAnim(unsigned char *a, int anim);
extern void __WaitFrames(int n);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __PlaySound(int id);
extern void __Actor_WaitMovement(unsigned char *a);
extern int L9[] __asm__(".L9");

void OvlFunc_common1_2060(void)
{
    int v[3];
    unsigned char *actor;
    unsigned char *b;
    unsigned char *c;
    unsigned char *g;
    unsigned char *q;
    unsigned char *tbl;
    int off;
    int t;
    int mask;
    int zero;
    int speed;
    int n;

    g = gState;
    q = g + 0xfa * 2;
    n = *(int *)q;
    zero = 0;
    actor = __MapActor_GetActor(n);
    tbl = (unsigned char *)L9;
    off = (*(unsigned short *)(actor + 6) >> 12) * 4;
    t = *(int *)(tbl + off);
    mask = 0xffff0000;
    v[0] = *(int *)(actor + 8) + (t & mask);
    v[1] = *(int *)(actor + 0xc);
    v[2] = *(int *)(actor + 0x10) + (t << 16);
    b = OvlFunc_common1_2018(v, actor);
    if (b == 0)
        return;
    t = *(int *)(tbl + off);
    mask = 0xffff0000;
    v[0] = *(int *)(b + 8) + (t & mask);
    v[1] = *(int *)(b + 0xc);
    v[2] = *(int *)(b + 0x10) + (t << 16);
    c = OvlFunc_common1_2018(v, b);
    if (c != 0 && (*(c + 0x59) & 1) != 0)
        return;
    v[0] = *(int *)(b + 8);
    v[1] = *(int *)(b + 0xc) + (0x80 << 13);
    v[2] = *(int *)(b + 0x10);
    c = OvlFunc_common1_2018(v, b);
    if (c != 0 && (*(c + 0x59) & 1) != 0)
        return;
    *(b + 0x22) = 2;
    t = *(int *)(tbl + off);
    mask = 0xffff0000;
    v[0] = *(int *)(b + 8) + (t & mask);
    v[1] = *(int *)(b + 0xc);
    v[2] = *(int *)(b + 0x10) + (t << 16);
    if (__TestCollision(b, v) > 0)
        return;
    speed = 0x3333;
    __Actor_SetAnim(actor, 8);
    __WaitFrames(0xf);
    *(int *)(b + 0x30) = speed;
    *(int *)(b + 0x34) = speed;
    __Actor_TravelTo(b, v[0], v[1], v[2]);
    *(int *)(actor + 0x30) = speed;
    *(int *)(actor + 0x34) = speed;
    __Actor_TravelTo(actor, v[0], v[1], v[2]);
    __PlaySound(0xee);
    __Actor_WaitMovement(b);
    __PlaySound(0x90 * 2);
    *(int *)(b + 8) = v[0];
    *(int *)(b + 0x10) = v[2];
    *(int *)(b + 0x24) = zero;
    *(int *)(b + 0x2c) = zero;
    __Actor_SetAnim(actor, 1);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_common1_2060 -- asm/overlays/common/common1_c_c_b.s
 *
 * LENGTH EXACT (163 against 163 encodings). Two numbers are recorded because
 * they measure different things and both appeared in the attempt: 20 by aligned
 * instruction distance, 91 by objcmp's encoding count. Trust the objcmp figure
 * as the gate number; the aligned figure is what the sweep was steered by.
 * Candidate: scratch_elev/b258/common/final/park_common1_2060.c.
 *
 * ONE LEVER IS ALREADY LOAD-BEARING AND IS THE OPPOSITE OF ITS FILE-MATES'.
 * "Name the address, not the offset" ran BOTH WAYS in one batch: common1_2c4
 * and common1_148 need the address named, and this one needs it SPLIT into base
 * plus offset -- worth 41 -> 27, and it restores a fourth push. The ROM's
 * load/store form is the discriminator, not a preference. Do not transplant the
 * file-mates' spelling here.
 *
 * `.L9` is read out of this file's own `.data`, and the verified spelling is
 * `extern int L9[] __asm__(".L9");`.
 *
 * *** LANDING IS EXPENSIVE: WHOLE+DATA, HAND SPLIT, AND THREE OVERLAYS. ***
 * asmfacts.py says `2 functions  split first` and UNDERSTATES it. The .s also
 * carries:
 *      .data   eleven .incbin ranges plus gOvlCommon1_3fe4
 *      .data1
 *      .bss    31 .lcomm
 * and 2060 reads `.L9` out of that .data, so the data MUST stay in an asm
 * sibling -- split_s.py cannot cut this, exactly the recorded WHOLE+DATA shape.
 *
 * The object is shared across three overlays, so FOUR linker lines change in
 * EACH of three scripts. That is twelve line edits by hand, which is the most
 * expensive landing shape in the tree. Worth closing the file-mate first so the
 * cut pays for two functions rather than one.
 *
 * For contrast, the sibling file common1_a_a_a_a_a_c_a.s had no data, so
 * split_s.py cut it and rewrote all six of ITS linker lines automatically. The
 * difference between those two landings is entirely the .data section.
 *)

*/
#endif
