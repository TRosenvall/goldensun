/* OvlFunc_932_200a6c0 (0x0200a6c0) -- NON-MATCHING, 23 differing STREAM encodings of 135.
 *
 * MEASURED batch 343 brief D.  ref 135 / ours 135 stream encodings, 107/107 16-bit
 * INSTRUCTIONS (28 of the stream are pool words and pads), 25/25 RELOCATIONS
 * IDENTICAL.  IN PHASE on both counts, so 23 is a TRUE DISTANCE.
 * PINS: 0 (tools/shimcount.py).  datacheck.py: no data under the reference.
 * tools/piece.py: the reference .s holds THIS FUNCTION ALONE, so one candidate
 * is the whole piece.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b9cb4/200a6c0.c \
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_a_a.s --func OvlFunc_932_200a6c0
 *
 * ------------------------------------------------------------------- 28 -> 23
 * THE LEVER: ASSIGN THE *SIXTH* ARGUMENT TO A LOCAL AND PASS THE FIFTH AS A
 * LITERAL.  `f = <arg6>; __Func_8010704(a,b,c,d, <arg5>, f);` at all six
 * six-argument call sites.  Measured 28 -> 23 and it kills the park's whole
 * residue class (b) -- the twenty-two encodings the old header attributed to
 * "gcc reuses one register and interleaves the stores".
 *
 * WHY IT WORKS, and it is a LIVE-RANGE OVERLAP, not a scheduling trick.
 * With ACCUMULATE_OUTGOING_ARGS the two stack arguments are stored by
 * store_one_arg in argument order (arg5 to [sp], arg6 to [sp,#4]), and expand
 * emits `set p=const` immediately before each store -- verified in the .00.rtl
 * dump, insns 101/102/104/105: set p45=0x11 / str p45,[sp] / set p46=0xa /
 * str p46,[sp,#4].  Those two pseudo ranges are DISJOINT, so local-alloc gives
 * both the same hard register and sched2 then cannot separate the stores.
 * Hoisting arg6 into a local puts its `set` BEFORE arg5's, so arg6 is live
 * across arg5's set and the first store: the ranges now OVERLAP, the two
 * constants get two hard registers, and both `str`s come out byte-identical to
 * the ROM.
 *
 * DO NOT hoist the FIFTH argument instead -- that keeps the ranges disjoint
 * (the local's range still dies at the first store) and measures 28, unchanged.
 * DO NOT hoist BOTH: `e = arg5; f = arg6;` measures 34.  Its ORDER is the ROM's
 * (0x11 materialised first) but its REGISTERS are transposed, so the two
 * spellings are complementary and neither is strictly better.
 *
 * ----------------------------------------------------------- WHAT 23 NOW IS
 * The residue is NO LONGER "two patterns".  It is FOUR, all of them a two-way
 * r2/r3 exchange or a two-insn order swap, and every index is accounted for:
 *
 *   A  3 enc  idx 17-19   the 0x1a,0x1a site.  Both values equal, so ONE
 *                         register serves both on both sides -- the ROM picks
 *                         r3, we pick r2, and the two `str`s follow it.
 *   B  2 enc  idx 20-21   `movs r2,#1 / movs r3,#1` (ROM) vs r3 then r2 (ours),
 *                         args 3 and 4 of that same call.
 *   C  6 enc  idx 28-29, 53-54, 79-80
 *                         the `orr` operand roles, unchanged from the old park:
 *                           rom   ldrb r2,[r0] / movs r3,#2 / orr r3,r2
 *                           ours  ldrb r3,[r0] / movs r2,#2 / orr r3,r2
 *                         The orr and the strb MATCH; only which register
 *                         receives the LOAD and which the CONSTANT differ.
 *                         Thumb `orr Rd,Rm` ties operand 0 to operand 1
 *                         (arm.md iorsi3, constraint "%0"), so the ROM's RTL is
 *                         `(ior p_const p_loaded)` -- the CONSTANT is operand 1.
 *                         `fold` canonicalises a commutative operation's
 *                         constant to the SECOND operand, which is why
 *                         `*a |= 2`, `*a = 2 | *a`, `*a = *a | 2`,
 *                         `t = *a; *a = 2 | t;` and `*a = (char)(2 | (int)*a)`
 *                         ALL give the same stream -- five spellings, all 23.
 *   D 12 enc  idx 41-42, 67-68, 91-92, 110-111, 100-101, 119-120
 *                         the two stack constants are now in the ROM's TWO
 *                         registers and the two `str`s MATCH; only the ORDER of
 *                         the two `movs` is swapped.  Pure rank_for_schedule
 *                         tie-break: equal INSN_PRIORITY, equal class
 *                         (haifa-sched.c:4068-4095), equal dependent count
 *                         (:4097-4110), so the decision falls to INSN_LUID
 *                         (:4112-4116) and the local's `set` has the lower LUID.
 *                         To flip it the local would have to be emitted AFTER
 *                         arg5's set and still be live across the first store,
 *                         which no statement order in C provides.
 *
 * MEASURED THIS ROUND (all at 135/135 encodings, 107/107 instructions, rel ok):
 *   hoist arg6 only (THIS BODY)                                 23  <- best
 *   hoist arg6, then arg5, pass (e,f)                           23  (identical)
 *   hoist arg6 + `t = *a; *a = 2 | t;`                           23  (C inert)
 *   hoist arg6 + an extra early local to shift reload parity     23  (inert)
 *   baseline (both stack args literal)                           28
 *   hoist arg5 only                                              28
 *   `*a = *a | 2`, `*a = (char)(2 | (int)*a)`, `t=*a; *a=2|t;`   28  (on base)
 *   __attribute__((const)) on both six-arg callees               28
 *     -- this was an INSTRUMENT for calls.c:1452 precompute_arguments, which
 *        precomputes EVERY argument when ECF_CONST is set.  It is INERT, so the
 *        expand-order route is closed as well as semantically wrong here.
 *   nested anchor before each six-arg call                       28
 *   hoist arg5 then arg6, pass (e,f)                             34
 *   hoist both with an anchor between them                       34
 *   hoist all six arguments                                      34
 *   declare `int f, e;` instead of `int e, f;`                   34
 *   nested anchor after each six-arg call                        36
 *   anchor crossed with the e/f hoist                            42
 *
 * BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, batch 343): all eight flag
 * groups and all 23 top-level anchor placements are inert or worse on the
 * 28-body.  -O1 40, -fno-schedule-insns2 40, every other flag 28.
 *
 * WHAT IS RIGHT: all 24 calls, both six-argument call shapes INCLUDING the two
 * `str`s at every site, the three `__GetFlag`-guarded blocks, `0x80 << 2` /
 * `0x81 << 2` / `0xc8 << 4`, the nested
 * `__Actor_SetSpriteFlags(__MapActor_GetActor(9), 0)`, and the actor pointer
 * ADVANCED IN PLACE (`a = __MapActor_GetActor(9) + 0x23;`).
 */
extern char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(char *a, int f);
extern void OvlFunc_932_200840c(void);
extern void OvlFunc_932_200b460(int slot);
extern int __GetFlag(int);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern int __StartTask(void *fn, int pri);
extern void OvlFunc_932_200b428(void);

void OvlFunc_932_200a6c0(void)
{
    char *a;
    int f;

    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    OvlFunc_932_200840c();
    OvlFunc_932_200b460(9);
    if (__GetFlag(0x80 << 2) != 0) {
        __MapActor_SetAnim(9, 5);
        f = 0x1a;
        __Func_8010704(0, 0, 1, 1, 0x1a, f);
        a = __MapActor_GetActor(9) + 0x23;
        *a |= 2;
    }
    OvlFunc_932_200b460(0xb);
    if (__GetFlag(0x201) != 0) {
        __MapActor_SetAnim(0xb, 5);
        f = 0xa;
        __Func_8010704(1, 0, 1, 1, 0x11, f);
        a = __MapActor_GetActor(0xb) + 0x23;
        *a |= 2;
    }
    OvlFunc_932_200b460(0xc);
    if (__GetFlag(0x81 << 2) != 0) {
        __MapActor_SetAnim(0xc, 5);
        f = 0xf;
        __Func_8010704(1, 0, 1, 1, 0x1a, f);
        a = __MapActor_GetActor(0xc) + 0x23;
        *a |= 2;
    }
    __StartTask(OvlFunc_932_200b428, 0xc8 << 4);
    if (__GetFlag(0x327) != 0) {
        f = 0x51;
        __Func_8010704(0x1e, 0x52, 1, 1, 0x1d, f);
        f = 2;
        __CopyMapTiles(0x2e, 0x1c, 0x1d, 0x11, 1, f);
    } else {
        f = 0x51;
        __Func_8010704(0x1c, 0x52, 1, 1, 0x1d, f);
        f = 2;
        __CopyMapTiles(0x2f, 0x1c, 0x1d, 0x11, 1, f);
    }
}
