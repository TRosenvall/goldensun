/* Func_8021d88 (0x08021d88) -- NON-MATCHING.
 *
 * NON-MATCHING, 40 of 53 encodings.  PIN-FREE, SHIM-FREE, NO asm.
 *   SIZE EXACT (116 bytes both).  ENCODING COUNT EXACT (53 = 53).
 *   Relocations are THE SAME TWO SYMBOLS AT A SHIFTED OFFSET
 *   (Func_8021c64 at 0x46 against 0x48), i.e. a distance, not a dirty
 *   relocation.  So unlike the previous body's 42, THIS 40 IS A DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8021d88.c \
 *     asm/rom_15000/rom_20198_c_c_c_c_c.s --func Func_8021d88
 *
 * THE PREVIOUS BODY'S 42 WAS MISALIGNED AND THE PARK MISNAMED WHY.  It said
 * "COUNT DIFFERS (ref 53, ours 52) -- so this positional figure measures
 * MISALIGNMENT" and "49 lines against 50".  The encodings did differ 53/52 and
 * the size by 4 bytes, but the INSTRUCTION counts were 49 = 49: the entire delta
 * was ONE POOL WORD.  That pool word is now there.
 *
 * ========== CAUSE B, WHICH THE PARK DID NOT MENTION: MASK NARROWING =========
 *
 * The ROM masks with a POOLED `0xfffffc00`:
 *     ldr r3,=0x3ff / ldrh r2,[r6,#8] / ands r0,r3 / ldr r3,=0xfffffc00
 *       / ands r3,r2 / orrs r3,r0 / strh r3,[r6,#8]
 * The previous body got `ldrh r2,[r6,#8] / movs r3,#252 / lsls r3,r3,#8 / ...`
 * -- gcc had narrowed the mask to HImode `0xfc00`.
 *
 * MECHANISM.  combine's `simplify_and_const_int` narrows an AND's constant by
 * `nonzero_bits (varop, mode)`.  `LOAD_EXTEND_OP` is UNCONDITIONALLY
 * `ZERO_EXTEND` on Thumb (config/arm/arm.h:2330), so a loaded halfword carries
 * `nonzero_bits == 0xffff` and `0xfffffc00` collapses to `0xfc00` -- which is
 * `0xfc << 8`, so constraint `K` of `*thumb_movsi_insn` matches ahead of the
 * pool alternative `mi` and the constant CAN NEVER REACH THE POOL.  That is the
 * `_MSG_182` argument from docs/owner-decisions.md, running in reverse.
 *
 * `nonzero_bits`' REG case gates its answer (combine.c:7987-8000): it returns
 * `reg_last_set_nonzero_bits` only when the tracking is current AND
 * `reg_last_set_label[regno] == label_tick` OR the pseudo has
 * `REG_N_SETS (regno) == 1`.  SO A MULTI-SET PSEUDO LOSES THE TRACKED NONZERO
 * BITS AND THE WIDE CONSTANT SURVIVES TO THE POOL.
 *
 * TWO EDITS, both predicted from that reading and then measured:
 *   1. the OR operands swapped -- `(r & 0x3ff) | (w & 0xfffffc00)` -- which puts
 *      the 0x3ff pool load ahead of the `ldrh`, matching indices 35-37 exactly.
 *      Worth 42 -> 33 alone (on the misaligned stream).
 *   2. `v = *(unsigned short *)(p + 8);` -- REUSING THE EXISTING LOCAL `v`, so
 *      that pseudo has REG_N_SETS == 2.  This is what pools `0xfffffc00`, and it
 *      is what makes the size and the encoding count exact.
 *
 * > GENERALISABLE LEVER, new: to keep a wide mask constant out of
 * > `simplify_and_const_int`'s narrowing -- and therefore IN the pool -- give the
 * > masked value a pseudo with MORE THAN ONE SET.  Reusing an existing local is
 * > the pin-free way to do it.  config/arm/arm.h:2330 + combine.c:7987.
 *
 * ========== CAUSE A, THE REMAINING 40: HIGH REGISTER VERSUS SPILL ===========
 *
 * The residue is 5 runs -- [1..17], [21..25], [32..42], [45..47], [49..52] -- and
 * every one is downstream of ONE allocation decision:
 *
 *   ROM   push {r5,r6,lr} + `mov r6,r8 / push {r6}`      ONE high register,
 *         with `a` held in r4 and SPILLED around each call:
 *         `sub sp,#4`, `str r4,[sp,#0]`, `ldr r4,[sp,#0]`, `add sp,#4`.
 *   ours  push {r5,r6,lr} + `mov r6,sl / mov r5,r8 / push {r5,r6}`
 *         TWO high registers (r8 and sl), no frame, no spill.
 *
 * Four values cross both calls and `-fcall-used-r4` makes r4 call-clobbered, so
 * holding `a` in r4 costs a spill per call.  gcc finds one more register than the
 * ROM's compiler did.  THE PARK'S STATED BLOCKER SURVIVES; it is the fifth
 * specimen of the class, with rom_9000/800bbc0.c, ovl_7e3e08/2008f94.c,
 * rom_15000/8020150.c and rom_b5000/80b6a60.c.
 *
 * The park's "NEXT: nothing source-level" was wrong as a blanket claim -- cause B
 * was entirely source-level and the park never named it -- but it stands for A.
 *
 * NOTE FOR ANYONE USING crossfire.py HERE: `MEM` fires on EVERY row, BASE
 * included, and it is correct to ignore.  The reference profile is
 * ldr=4 ldrh=2 str=4 strh=1; the two extra ldr/str are the ROM's spill pair,
 * which is cause A itself, not a wrong program.
 *
 * MEASURED AND INERT (candidate prerequisites, not dead ends):
 *   `a += k;` before the halfword read                    0
 *   `m = i << 4;` after `p += 0x82 << 1;`                  0
 *   `~0x3ff` written for `0xfffffc00`                      0
 * MEASURED AND WORSE:
 *   `j = k + (0x8e << 1);` hoisted above the first call  +15, and the
 *                          instruction count changes
 *   -fcall-saved-r4 (previous body)   46 lines, 43 differing -- it lets gcc keep
 *                                     the value in r4 with no spill at all
 *
 * WHAT IS RIGHT and should be kept: the r8 holding the third argument; the named
 * index for the `str r2, [r4, r3]` register-plus-register store; the `i * 28`
 * built as `(i * 8 - i) << 2`; and the two-statement `p = a + k; p += 0x82 << 1;`.
 *
 * NEXT: nothing source-level for cause A.  It needs the spill-versus-high-register
 * class solved, which is tracked across five parks.
 */
extern void Func_8021cb8(void *p, int c, int m);
extern int Func_8021c64(int v, int c);

void Func_8021d88(unsigned char *a, int i, int c)
{
    unsigned char *p;
    int k;
    int j;
    int m;
    int v;
    int r;

    k = (i * 8 - i) << 2;
    p = a + k;
    m = i << 4;
    p += 0x82 << 1;
    Func_8021cb8(p, c, m);
    j = k + (0x8e << 1);
    *(int *)(a + j) = c;
    *(int *)(p + 4) = 0x80002000;
    *(int *)(p + 8) = 0;
    k += 0x88 << 1;
    v = *(unsigned short *)(a + k);
    r = Func_8021c64(v, c);
    v = *(unsigned short *)(p + 8);
    *(unsigned short *)(p + 8) = (r & 0x3ff) | (v & 0xfffffc00);
}
