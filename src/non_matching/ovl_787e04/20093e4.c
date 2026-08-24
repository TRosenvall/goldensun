/* OvlFunc_887_20093e4  [ovl_787e04]  --  0x020093e4
 *
 * Source asm: goldensun/asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c_c.s
 *
 * A 140-instruction straight-line cutscene, 29 calls, no branches. Attempted
 * DELIBERATELY as an experiment rather than picked as a candidate: nothing over
 * 65 instructions had been tried, and 45% of the remaining work sits in the 244
 * functions over 400. See reports/large-functions.md.
 *
 * RESULT: 140 against 140, with the two streams agreeing everywhere except
 * FOUR small regions totalling 28 instructions. Eighty percent of the function
 * is right on a first transcription. Nothing about the LENGTH is the problem.
 *
 * The four regions, and what each one is:
 *
 * 1. THE `-1` TRIPLE, 7 instructions. `__Func_80933f8(-1, -1, -1, 0)`.
 *
 *        rom    mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0
 *               neg r1,r1 / neg r2,r2 / neg r0,r0
 *        ours   mov r2,#1 / neg r2,r2 / mov r3,#0 / mov r0,r2 / mov r1,r2
 *
 *    gcc builds -1 once and copies; the ROM builds it three times. This is the
 *    rematerialisation class from src/non_matching/ovl_77dd1c/200c5b8.c, where
 *    it is the whole blocker. Tried here: `0 - 1`, `~0`, three named int
 *    locals, and the callee undeclared. All identical output.
 *
 * 2. THE IWRAM STORE, 6 instructions. Structurally correct -- the ROM's
 *    `add r3,r2 / add r2,#0x42 / str r2,[r3]` is reproduced -- but with the two
 *    registers' roles swapped, because gcc schedules the pointer's load after
 *    the offset's shift and the ROM does it before. A scheduling difference
 *    inside four instructions, not a wrong construct.
 *
 * 3. `mov r1, #0x80` displaced by three positions in the __UploadSpriteGFX
 *    argument block. Ordinary argument-order; the declaration lever was not
 *    swept on it because regions 1 and 4 dominate.
 *
 * 4. FOUR ARG-INTERLEAVE SITES, 12 instructions.
 *
 *        rom    mov r1,#0xf0 / mov r0,#0x12 / lsl r1,#1 / mov r2,#0xb0
 *        ours   mov r1,#0xf0 / lsl r1,#1 / mov r0,#0x12 / mov r2,#0xb0
 *
 *    Exactly the class in src/non_matching/overlays/interleaved_arg_setup.c,
 *    which has four members and no lever. Here it appears FOUR TIMES IN ONE
 *    FUNCTION, in three calls to __Func_80921c4 and one to __Func_8092adc.
 *
 * THAT IS THE FINDING. Both of the blockers that stop this function are
 * already-parked classes, and neither is new. What is new is the density: a
 * blocker that appears at one call site in twenty is nearly certain to appear
 * somewhere in a function with twenty-nine call sites. Measured across the
 * whole remaining corpus, 23% of functions under 20 instructions carry at least
 * one of three known blocker shapes, against 99% of those over 400.
 *
 * So this function is not blocked by being long. It is blocked by containing
 * five instances of two classes we already could not solve at any size.
 *
 * WHAT WAS ESTABLISHED and should be kept in any further attempt:
 *   - the zero stored at +0x55 and +0x27 is ONE named local used twice; the
 *     ROM keeps it in a callee-saved register across both stores. Rebuilt at
 *     each store it also poisons the `~0x20` mask, which gcc then builds as
 *     `sub r3,#0x21` from the live zero instead of `mov #0x21 / neg`.
 *   - the iwram store advances a named pointer and reuses the offset variable
 *     as the stored value (`off += 0x42`), which is what the ROM does.
 *   - `~0x20` is a named `int`; the ROM has `mov r3,#0x21 / neg r3,r3`.
 *   - every other constant is written inline, including the four `<<` shifts
 *     that gcc folds and rematerialises.
 *
 * Progress by variant, measured with `tryc.py --align` (which is in the tree
 * because this function needed it -- the positional count said "132 differ" for
 * all of them):
 *
 *     first transcription                                32 of 140
 *     + shared zero, + pointer-walk iwram store          28 of 140
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
