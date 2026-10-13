/* OvlFunc_887_20093e4  [ovl_787e04]  --  0x020093e4
 *
 * NON-MATCHING, 127 of 144 encodings  (re-measured batch 347 brief B).
 *   THE FIGURE IS NOT A DISTANCE.  16-bit INSTRUCTION COUNT ref 111, ours 109 --
 *   an INSTRUCTION GAP OF -2.  SIZE ref 352 bytes, ours 348.  144 == 144 is the
 *   STREAM length (pool words, `bl` calls and pads included).  ALIGNED residue
 *   28 in 18 hunks.
 *   The old header contained BOTH claims, four lines apart: "COUNT DIFFERS (ref
 *   144, ours 142) -- so this positional figure measures MISALIGNMENT, not
 *   distance" and then "Batch 322's re-classification of this figure is WITHDRAWN. *** THAT CLAIM WAS FALSE HERE. *** Re-measured by the coordinator after batch 348: ref 111 / ours 109 16-bit instructions, a gap of -2, so this figure is a POSITIONAL count measuring PHASE, not distance. Batch 322's re-classification sweep made this claim on 54 parks and was WRONG ON 34 OF THEM, this one included; the 18 it got right keep the claim. Close the length gap before reading the figure, then rank on tools/aligncmp.py rather than on the raw count."
 *   The second is false and is removed.  The first was right in substance and
 *   wrong in evidence: 144/142 is the stream, the instruction counts are 111/109.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_787e04/20093e4.c \
 *     asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c_c.s --func OvlFunc_887_20093e4
 *   ... and tools/aligncmp.py with the same two paths plus the function name.
 *
 * THE GAP IS ONE SITE OF 2, AND IT IS REGION 1 OF THE FOUR BELOW.  Counted per
 * region from aligncmp, which is the only way to tell one site of 2 from two
 * sites of 1:
 *
 *     region 1, the `-1` triple      ref 7 instructions, ours 5   -2
 *     region 2, the iwram store      ref 6, ours 6                 0
 *     region 3, `mov r1, #0x80`      ref 1, ours 1                 0
 *     region 4, four arg-interleaves ref 12, ours 12               0
 *
 * So all three count-neutral regions are PURE ORDER, and the whole length
 * shortfall is the triple.  The old header's "140 against 140" was the stream.
 *
 * REGION 1, verbatim:
 *     rom    mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0
 *            neg r1,r1 / neg r2,r2 / neg r0,r0
 *     ours   mov r2,#1 / neg r2,r2 / mov r3,#0 / mov r0,r2 / mov r1,r2
 * gcc builds -1 once and copies twice; the ROM builds it three times.
 *
 * THE PARK'S MECHANISM WAS THE WRONG PASS.  It cited `update_equiv_regs` in
 * local-alloc.c and the pair of conditions `REG_N_REFS == 2` and
 * `REG_BASIC_BLOCK < 0`.  Those are real -- local-alloc.c:886-889 in this tree's
 * gcc (relative lines 220-223 of the function that starts at local-alloc.c:667) --
 * but they gate the wrong transformation: `reg_equiv_replace` REBUILDS a constant
 * at a single use, which is the direction we WANT, and its own comment says "If
 * the register is only used in one basic block, this can't succeed or combine
 * would have done it."  It is not what commons three -1s into one.
 *
 * WHAT ACTUALLY DOES IT, with the line that decides it.  cse substitutes a
 * register for a constant when the register is cheaper, and the cost comes from
 * arm_rtx_costs's CONST_INT case, config/arm/arm.c:2077-2095:
 *
 *     case CONST_INT:
 *       if (outer == SET)
 *         {
 *           if ((unsigned HOST_WIDE_INT) INTVAL (x) < 256) return 0;
 *           if (thumb_shiftable_const (INTVAL (x)))        return COSTS_N_INSNS (2);
 *           return COSTS_N_INSNS (3);
 *         }
 *
 * -1 is 0xffffffff unsigned, so not < 256, and it is not thumb_shiftable_const
 * (arm.c:8579), so its SET cost is THREE instructions' worth -- against one for a
 * register copy -- even though gcc then emits it in TWO (`mov #1 / neg`).  The
 * cost model overstates the constant, so cse always wins the comparison.  No
 * spelling of -1 in C changes rtx_cost of (const_int -1).
 *
 * THE BOUND, RESTATED WITH A MEASURED CEILING RATHER THAN AS AN ABSOLUTE.
 * The old header said "In plain C it is unreachable ... register pinning -- a
 * fakematch -- is the only way through."  The substance survives, but it is now
 * a number instead of an assertion: DEFINITION ORDER BUYS EXACTLY ONE OF THE TWO
 * MISSING BUILDS AND NO MORE.  Naming the three as `int` locals assigned BEFORE
 * `__CutsceneStart()` -- a call they are live across, which is the discriminator
 * in docs/elevation.md's definition-order lever -- gives ours 110 against ref
 * 111, i.e. gap -1.  FOUR placements all plateau there:
 *     three names before the first call                 110,  aligned 33
 *     the same with the arguments reversed              110,  aligned 33
 *     one name before the call, two after               110,  aligned 33
 *     two names + one literal                           110,  aligned 33
 * and what gcc actually does is build -1 ONCE into r5 (callee-saved, so it
 * survives the call) and copy it three times -- 2 + 3 instead of 2 + 2.  The
 * ceiling is structural: one C variable is one pseudo, so N names can only ever
 * add ONE build.  All four are WORSE on the aligned residue (28 -> 33), so the
 * park body below is still the best form.
 *
 * MEASURED INERT at production flags (all bit-identical at 127, aligned 28):
 *   `0 - 1` / `~0` / `-1` mixed in one argument list; the callee prototyped
 *   `(short, short, short, int)`; three names defined immediately before the
 *   call rather than before the preceding call.
 * FLAG AXIS CLOSED, and two rows are traps: -O1 reads ndiff 132 and takes the
 *   aligned residue 28 -> 54; -fno-schedule-insns2 reads 125, better than the
 *   base 127, and takes it 28 -> 49.  The other six groups are bit-identical.
 * ANCHOR AXIS CLOSED: all 45 statement boundaries, best 127 against base 127.
 *
 * ---- retained from the previous header, all re-measured and still true ----
 * Regions 2-4 are order, not construct:
 *   2. THE IWRAM STORE, 6 instructions.  `add r3,r2 / add r2,#0x42 / str r2,[r3]`
 *      is reproduced with the two registers' roles swapped, because gcc schedules
 *      the pointer's load after the offset's shift and the ROM does it before.
 *   3. `mov r1, #0x80` displaced by three positions in the __UploadSpriteGFX
 *      argument block.
 *   4. FOUR ARG-INTERLEAVE SITES, 12 instructions:
 *        rom  mov r1,#0xf0 / mov r0,#0x12 / lsl r1,#1 / mov r2,#0xb0
 *        ours mov r1,#0xf0 / lsl r1,#1 / mov r0,#0x12 / mov r2,#0xb0
 *      The class in src/non_matching/overlays/interleaved_arg_setup.c, here four
 *      times in one function.
 * KEEP IN ANY FURTHER ATTEMPT: the zero stored at +0x55 and +0x27 is ONE named
 * local used twice; the iwram store advances a named pointer and reuses the
 * offset variable as the stored value (`off += 0x42`); `~0x20` is a named `int`.
 * A 140-instruction straight-line cutscene, 29 calls, no branches; attempted as
 * a size experiment (see reports/large-functions.md) and 80% right on a first
 * transcription.
 */
struct Spr { unsigned char pad_00[5]; unsigned char f5; unsigned char pad_06[3];
             unsigned char f9; unsigned char pad_0a[0x12]; unsigned char f1c;
             unsigned char pad_1d[0xa]; unsigned char f27; };

extern unsigned int iwram_3001ebc;
extern unsigned char gScript_887__02009eac[];
extern unsigned char gScript_887__02009ecc[];

extern void __CutsceneStart(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int slot, int b);
extern void *__CreateActor(int a, int b, int c, int d);
extern void *__galloc_iwram(int a, int n);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int a, int b, void *p);
extern void __gfree(int a);
extern void __MapTransitionIn(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Actor_SetScript(void *a, void *s);
extern void __Actor_WaitScript(void *a);
extern void __CutsceneWait(int n);
extern void __DeleteActor(void *a);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int a);

void OvlFunc_887_20093e4(void)
{
    void *a;
    struct Spr *s;
    unsigned char *p;
    int z;
    unsigned char *q;
    unsigned char *base;
    unsigned int off;
    int m;

    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(0x12, 0xf0 << 17, 0xca << 16);
    __WaitFrames(1);
    __SetCameraTarget(0x12, 1);
    a = __CreateActor(0x16, 0xa4 << 17, 0x80 << 10, 0xc3 << 16);
    z = 0;
    *((unsigned char *)a + 0x55) = z;
    s = *(struct Spr **)((unsigned char *)a + 0x50);
    *(int *)((unsigned char *)a + 0xc) = 0xa0 << 11;
    s->f27 = z;
    m = ~0x20;
    s->f5 = m & s->f5;
    s->f9 = 0xf & s->f9;
    p = (unsigned char *)__galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(0xe0);
    p += 0x80 << 3;
    __UploadSpriteGFX(s->f1c, 0x80, p);
    __gfree(0x11);
    base = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    q = base + off;
    off += 0x42;
    *(unsigned int *)q = off;
    __MapTransitionIn();
    __MapActor_SetSpeed(0x12, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(0x12, 0xf0 << 1, 0xb0);
    __Func_80921c4(0x12, 0xd2 << 1, 0xa4);
    __Func_80921c4(0x12, 0xa3 << 1, 0xb9);
    __Func_8092adc(0x12, 0x80 << 7, 0xa);
    __Actor_SetScript(a, gScript_887__02009eac);
    __Actor_WaitScript(a);
    __Actor_SetScript(a, gScript_887__02009ecc);
    __Actor_WaitScript(a);
    __CutsceneWait(0x14);
    __DeleteActor(a);
    __MapActor_Jump(0x12, 2, 0x14);
    __Func_8092adc(0x12, 0, 0x28);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x16);
}
