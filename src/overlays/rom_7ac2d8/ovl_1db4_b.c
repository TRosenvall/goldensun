/* OvlFunc_924_200a1cc  --  0x0200a1cc
 *
 * The tail of goldensun/asm/overlays/rom_7ac2d8/ovl_1db4.s. HAND-SPLIT: the
 * file's `.section .data` follows this function, and split_s.py keeps trailing
 * data with the function it follows, which would have carried the whole
 * overlay's script blobs into this .c and dropped them. The two functions ahead
 * of it AND all the data stay in _a.s. All four blobs are already .global, so
 * this side needs only plain externs. Verified byte-neutral with make compare
 * green before any C landed.
 *
 * A camera move followed by a guard chain that picks one of four scripts.
 *
 * MATCHED ON THE FIRST SPELLING, at default flags, which makes the [cse] marker
 * wrong for the SIXTH time in seven. The repeats it fired on are four flag ids
 * in mutually exclusive `else if` arms -- the shape already recorded as inert.
 * Worth stating as a systematic case rather than another anecdote: THE MARKER
 * SEES TEXTUAL CONSTANT REPETITION AND CANNOT SEE ARM EXCLUSIVITY, so a chain of
 * `else if` guards testing overlapping flag ids is a structural false positive.
 *
 * A REPEATED EQUALITY TEST IN AN else-if CHAIN IS SOURCE-VISIBLE, NOT A
 * CROSS-JUMP ARTEFACT. The ROM re-tests the same scalar against the same value
 * in a later arm, having already tested it in an earlier one, which reads like
 * redundant codegen. It is not: it is literally
 * `else if (y == K && flag_a)` following `if (y == K && !flag_b && ...)`.
 * Nesting the second arm under one shared test -- the tidier spelling -- loses
 * the compare. WHEN THE ROM REPEATS A SCALAR COMPARE THAT A SMARTER NESTING
 * WOULD ELIDE, WRITE THE REDUNDANT TEST; the else-if chain IS the spelling.
 *
 * The three shared tails then cross-jump on their own, and the fourth does not
 * merge because different code follows it. No goto and no lever was needed for
 * any of that.
 *
 * CHECK THE IMMEDIATES BEFORE REACHING FOR A NAMED LOCAL. The interleaved
 * `mov / mov / lsl / lsl` argument build came out free from bare shifted
 * literals, with no named locals -- because both constants share the same
 * `mov` immediate and gcc batches identical values. A sibling calling the same
 * helper DID need named locals for the same shape, and the difference is
 * exactly that its two immediates differ. The reflex to name shifted constants
 * is wrong when they share a byte.
 *
 * The `asr #20 / bge / add` sequence is plain signed division by 0x100000,
 * confirmed against the matched sibling ovl_22c4_c_c_a.c.
 */
extern int *__MapActor_GetActor(int slot);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern int __GetFlag(int id);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __CutsceneWait(int n);

extern unsigned char gScript_924__0200df20[];
extern unsigned char gScript_924__0200df60[];
extern unsigned char gScript_924__0200dfa8[];
extern unsigned char gScript_924__0200dff0[];

int OvlFunc_924_200a1cc(void)
{
    int x;
    int y;

    x = __MapActor_GetActor(9)[2] / 0x100000;
    y = __MapActor_GetActor(9)[4] / 0x100000;
    __Func_80933d4(0xa0 << 11, 0xa0 << 8);
    __Func_80933f8(0xcc << 18, -1, 0xb2 << 18, 1);
    __Func_8093530();
    if (!__GetFlag(0x877)) {
        if (x == 0x32 && __GetFlag(0x319)) {
            __MapActor_SetBehavior(9, gScript_924__0200df20);
        } else if (x == 0x31) {
            if (y == 0x2c && !__GetFlag(0x319) && !__GetFlag(0x31a) && !__GetFlag(0x31b)) {
                __MapActor_SetBehavior(9, gScript_924__0200df60);
            } else if (y == 0x2c && __GetFlag(0x319)) {
                __MapActor_SetBehavior(9, gScript_924__0200dff0);
            } else if (y == 0x2e && __GetFlag(0x31a)) {
                __MapActor_SetBehavior(9, gScript_924__0200dfa8);
                __CutsceneWait(0x1e);
                return 1;
            }
        }
    }
    __CutsceneWait(0x1e);
    return 0;
}
