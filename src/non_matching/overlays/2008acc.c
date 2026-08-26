/* OvlFunc_898_2008acc -- NOT MATCHING, and the closest park in the tree.
 *
 * Source asm: goldensun/asm/overlays/rom_793768/ovl_314_c_c_a_c_c_c_a_a.s
 *
 * BLOCKER CLASS: literal-pool PLACEMENT. 44 instructions against the ROM's 44,
 * in the same order, with the same operands and the same registers. The only
 * difference in the whole function is WHERE the pool is dumped:
 *
 *      rom    bl __WaitFrames / bl __CutsceneEnd / b .Lb28 / <pool> / .Lb28:
 *      ours   bl __WaitFrames / b .L4 / <pool> / .L4: / bl __CutsceneEnd
 *
 * One instruction. The pool holds the same two words in the same order
 * (`_CONST_2`, then 0x133b), and the pool-skip branch is the same instruction;
 * gcc simply dumps it one `bl` earlier.
 *
 * THE ADDRESS ARITHMETIC SAYS THE ROM'S POINT IS THE ALIGNED ONE. The function
 * starts at 0x02008acc. Counting to the ROM's dump point gives 84 bytes (26
 * two-byte instructions and 8 four-byte `bl`s), so the pool sits at 0x02008b20
 * -- 4-aligned, no padding -- and the two words put the continuation at
 * 0x02008b28, which is what the ROM's label .Lb28 is named after. Ours dumps at
 * 82, is not 4-aligned, and gcc emits `.align 2, 0` to pad. So gcc is choosing
 * a WORSE point than the original build did, and choosing it for a reason that
 * is not range: the first pending constant is only 58 bytes back and
 * *thumb_movsi_insn* has a pool_range of 1020.
 *
 * WHAT WAS TRIED, all giving byte-identical output (44 lines, 2 real diffs):
 *   - `a->f64 = a->f64 & 1` instead of the walked pointer
 *   - `*p = 1 & *p` (operand order)
 *   - `*p |= two` instead of `*p = two | *p`
 *   - `int two` instead of `unsigned short two`
 *   - -fno-schedule-insns2 (worse, 10 differ), -fno-rerun-cse-after-loop
 *     (identical), -O1 (worse, 14 differ)
 *
 * So the dump point does not move with the source spelling of the tail, which
 * is what makes this a placement blocker rather than a reading problem. It is
 * the same class as src/non_matching/rom_15000/rom_1c154.c and the pool-skip
 * note on Func_80b09fc, but this is the cleanest instance yet found: everything
 * else about the function is right, so anyone who works out what governs the
 * dump point can close it by changing nothing but the compiler invocation.
 *
 * BATCH 93 -- THE PLACEMENT IS NOT A FAMILY-WIDE BUG. OvlFunc_901_2008640
 * (src/non_matching/overlays/2008640.c) is the same cutscene bookend with the
 * same `b .L / <pool> / .L:` shape in its tail, and gcc puts ITS pool exactly
 * where the ROM does. So whatever moves this one is specific to this function
 * -- most likely the byte count from the function start to the dump point,
 * since the ROM's point here is 4-aligned and ours is not -- and not a general
 * disagreement about where thumb pools go. Do not spend time looking for a
 * global flag.
 *
 * THE REST OF THE FUNCTION IS SETTLED and should not be re-derived:
 *   - the pooled 2 is `_CONST_2` from const.sym, the same call as the rest of
 *     this overlay family -- see src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c
 *   - `short f6` (not `unsigned short`): the ROM reads it with the register-
 *     offset `ldrsh r2, [r6, r3]`, which is the only ldrsh thumb has
 *   - the actor pointer is kept AND a pointer to +0x64 is taken, because both
 *     +6 and +0x64 are touched; that is the `mov r5, r6 / add r5, #0x64` pair
 *   - saving f6 into r8 before the calls and restoring it after is a plain
 *     local, and it is what forces the r8 push/pop
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_2008acc(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    short saved;

    a = __MapActor_GetActor(0xf);
    saved = a->f6;
    p = &a->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x133b);
    __MapActor_SetAnim(0xf, 0);
    OvlFunc_898_200973c(0xf, 0, 2);
    OvlFunc_898_2009724(0xf, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = *p & 1;
}
