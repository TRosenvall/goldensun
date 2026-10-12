/* OvlFunc_967_2008308 -- NOT MATCHING, a LENGTH gap: ref 60 instructions against ours 59 (-1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L8308.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008308.c \
 *     asm/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_c.s --func OvlFunc_967_2008308
 *
 * The original prose park is preserved below.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, void *b);
extern unsigned char ActorCmd_ARRAY_944__02009314[];

void OvlFunc_967_2008308(void)
{
    unsigned char *a;
    int m;
    short w;

    a = __MapActor_GetActor(0);
    m = *(unsigned short *)(a + 6) + (0x80 << 6);
    m &= 0xffffc000;
    w = m;
    __SetFlag(0xc0 << 2);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x26ec);
    __CutsceneWait(0x32);
    __MapActor_Emote(0xe, (0x81 << 1), 0x32);
    __Func_809280c(0xe, 0, 0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    if ((w << 16) == (0x80 << 24)) {
        __Func_8092304(0, 0, 0x10);
        __Func_8092adc(0, 0xc0 << 8, 0);
        __CutsceneWait(0x14);
    }
    __MapActor_SetBehavior(0xe, ActorCmd_ARRAY_944__02009314);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_967_2008308 -- 0x02008308,
 * asm/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_c.s
 *
 * 88 vs 86 lines.  Candidate at scratch/L8308.c.
 *
 * SOLVED:
 *   - The mask needs an `int` intermediate; assigned straight into the `short`
 *     it narrows and gcc pools 0xc000 where the ROM has 0xffffc000.
 *   - The comparison is done in the HIGH HALF.  The ROM sign-extends the short
 *     (`lsl #16 / asr #16`) and then shifts LEFT AGAIN to compare against
 *     `0x80 << 24`.  Spelling it `(w << 16) == (0x80 << 24)` reproduces that;
 *     `w == (short)0x8000` gives a pooled 0xffff8000 and a direct compare.
 *     Together these moved the first difference from line 8 to line 19.
 *
 * BLOCKER: the argument-setup interleave at __MapActor_Emote --
 *      rom   mov r1,#0x81 / mov r0,#0xe / lsl r1,#1
 *      ours  mov r1,#0x81 / lsl r1,#1   / mov r0,#0xe
 * -- at a site that precedes every conditional branch in the function, so the
 * batch-127 lever has no dominating block to work from.  Naming the constants
 * at the top instead makes gcc keep them in callee-saved registers and adds two
 * pushes (85 lines, 81 differing), exactly as the straight-line parks show.
 *
 * Worth noting for the class: the interleaved single-instruction argument here
 * is `mov r0, #0xe`, not a zero.  The detector in docs/elevation.md only looks
 * for `mov r0, #0`, so the 248-function sizing undercounts.
 *)

(* RETRIED (batch 147).  Four more spellings, none moved it off 60 differing:
 *      `em = 0x81 << 1;` at the very top, used at the Emote call only
 *          -- 87 lines, 78 differing, WORSE.  The dominating-block lever that
 *          works on OvlFunc_926_200a484 needs the def in a block the use does
 *          not share; here there is no branch before the call, so hoisting only
 *          lengthens the live range and gcc allocates instead of rematerialising.
 *      the actor slot 0xe as a named local used at every call site   60
 *      the actor slot 0xe as a named local used at the Emote only    60
 *      the emote constant written as the plain literal 0x102         60
 *
 * Three spellings leaving the count EXACTLY unchanged says the r0 position here
 * is decided below the source, same conclusion as ovl_7ddb88/20092f0.c reached
 * on six sites of the same shape.  The two parks are the same blocker.
 *
 * ALSO WORTH RECORDING: of the 60 differing, only TWO are real.  The rom emits
 * `b L0 / L0:` to jump over an early literal pool and we put the pool at the
 * end, which shifts every later line and inflates the count.  Do not read 60 as
 * sixty problems -- the whole function is right except the Emote interleave and
 * the pool dump point, and the pool point is downstream of nothing we control
 * from a single-function translation unit.
 *)

*/
#endif
