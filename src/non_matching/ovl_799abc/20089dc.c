/* OvlFunc_905_20089dc  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c.s
 * Best screen: 2 instructions in disagreeing regions, of 15 (streams same length).
 *
 * BLOCKER CLASS: argument precompute -- the compiler difference traced to
 * calls.c:805 in batch 62. NOT FIXABLE FROM C.
 *
 *      rom   mov r1, #0x80 / mov r0, #0xd / lsl r1, #1 / mov r2, #0
 *      ours  mov r1, #0x80 / mov r0, #0xd / mov r2, #0 / lsl r1, #1
 *
 * The first call mixes a cheap constant with a shifted argument, which is
 * exactly the predicted failing shape: gcc precomputes the expensive argument
 * into a pseudo before any hard register is loaded, and the cheap `mov` lands
 * after it. See src/non_matching/ovl_780898/2008dc0.c for the full derivation
 * and the list of eight source spellings and eight flags that do not reach it.
 *
 * Screened rather than assumed, because the predictor in
 * tools/pool_candidates.py is right about 90% of the time and this function is
 * a 15-instruction leaf that was cheap to check. It came out where predicted.
 *
 * The other two calls in this function pass only cheap constants and match
 * exactly, which is the same split seen inside OvlFunc_921_20099bc.
 */
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void __Func_8091f14(int a, int b);

void OvlFunc_905_20089dc(void)
{
    int k;

    k = 0x80;
    __MapActor_Emote(0xd, k << 1, 0);
    __MapActor_Jump(0xd, 2, 0);
    __Func_8091f14(0xc, 0x28);
}
