/* OvlFunc_952_2008264 -- 0x02008264  (asm/overlays/rom_7d768c/ovl_30_c_a_a_a_c.s)
 *
 * BLOCKER: the GetFlag/SetFlag rule and the DERIVED-CONSTANT shape want
 * OPPOSITE flags in the same function, and neither can be had without losing
 * the other. 33 of 85, exact length. This is a boundary on CSE_CFLAGS, not a
 * new codegen class.
 *
 * The function carries both shapes at once:
 *
 *   0x966 feeds a __GetFlag guard and a __SetFlag inside the guarded block.
 *   That is the CSE_CFLAGS shape: at -O2 gcc commons the two pool loads into
 *   r6 where the ROM reloads from the pool twice.
 *
 *   0x2241 is a message-id BASE held in r6 across several calls, with the ROM
 *   deriving `add r0, r6, #1` and `add r0, r6, #2` for the two arms. That
 *   requires the constant to stay in a register.
 *
 * MEASURED, and the two are in direct opposition:
 *
 *   default flags                33 differ, 85 lines
 *     0x2241 is held and derived CORRECTLY -- `add r0, r6, #1` exactly --
 *     and 0x966 is wrongly commoned into r6, which shifts the whole stream.
 *
 *   -fno-rerun-cse-after-loop    57 differ, 84 lines
 *     0x966 is now reloaded twice, EXACTLY as the ROM does, and the prologue
 *     drops to `push {r5, lr}` matching... but 0x2241 stops being held, and
 *     gcc emits `ldr r0, =0x2242` / `ldr r0, =0x2243` where the ROM has the
 *     two `add`s. The fix for one half breaks the other.
 *
 *   -fno-gcse                    84 differ, 86 lines  (much worse)
 *   -fno-cse-follow-jumps        33 differ            (inert)
 *   -fno-cse-skip-blocks         33 differ            (inert)
 *   -fno-expensive-optimizations 33 differ            (inert)
 *
 * So the flag is too blunt: it suppresses the pass for the whole translation
 * unit, and this unit needs the pass ON for one constant and OFF for another.
 * No source spelling separates them either -- both are compile-time constants,
 * so `m = 0x2241; ... m + 1` folds under the flag exactly as
 * docs/elevation.md's "INVERSE constant problem" section records.
 *
 * WORTH RECORDING FOR THE RULE: "GetFlag(id) guarding a block that ends
 * SetFlag(id) means CSE_CFLAGS" is a good predictor -- it has now been right on
 * five-plus functions -- but this is the first counter-example where applying it
 * makes the function WORSE overall. Check what ELSE the unit commons before
 * adding the rule; a held-and-derived pooled base is the thing that loses.
 *
 * Everything else reproduces: three argument interleaves (0x80 << 7,
 * 0x80 << 9 / 0x80 << 8, and the -0x40 mov/neg) all came out on the first
 * screen from entry-block naming, and the two message arms are in the ROM's
 * fall-through order.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __Func_808e118(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092304(int a, int b, int c);

void OvlFunc_952_2008264(int actor)
{
    int m;
    int s1;
    int p1;
    int p2;
    int d;

    s1 = 0x80 << 7;
    p1 = 0x80 << 9;
    p2 = 0x80 << 8;
    d = -0x40;
    __CutsceneStart();
    __Func_808e118();
    if (__GetFlag(0x966) == 0) {
        __SetFlag(0x966);
        __SetFlag(0x967);
        __Func_8092adc(actor, s1, 0);
        __Func_80921c4(0, 0x78, 0x60);
        __Func_8092adc(0, 0xc0 << 8, 0);
        __CutsceneWait(0x14);
        m = 0x2241;
        __MessageID(m);
        __Func_8092c40(actor, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(m + 1);
        } else {
            __MessageID(m + 2);
        }
        __ActorMessage(actor, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(actor, 3);
        __CutsceneWait(0x14);
        __MapActor_SetSpeed(actor, p1, p2);
        __Func_8092304(actor, d, 0);
        __Func_8092304(actor, 0, 0x30);
    } else {
        __MessageID(0x2245);
        __Func_8092c40(actor, 0);
    }
    __CutsceneEnd();
}
