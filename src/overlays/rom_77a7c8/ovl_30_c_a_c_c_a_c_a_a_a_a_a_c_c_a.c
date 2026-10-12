/* OvlFunc_881_20097a4 -- 0x020097a4, BYTE-EXACT with CSE_CFLAGS (batch 339 brief E).
 *
 * 88 bytes, 38 encodings and 6 relocations identical; 32 / 32 instructions.
 * PIN-FREE and DEVICE-FREE -- shimcount.py prints no rows, nothing for
 * fakematch.txt.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-rerun-cse-after-loop \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_a.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_a.s --func OvlFunc_881_20097a4
 *
 * Split shape: NONE NEEDED.  The reference .s holds exactly ONE function
 * (tools/piece.py, and `grep -c thumb_func_start` = 1), and tools/datacheck.py
 * reports no data residue.
 *
 * THREE INGREDIENTS, ALL LOAD-BEARING.  The park read 25 of 38 at 30 / 32
 * instructions -- two instructions SHORT, so a statement was missing.
 *
 * 1. THE LITERAL GOES INSIDE THE COMPARISON, not through a local.  That is the
 *    missing -2.  Thumb's `cmp` has no 0xa0000 immediate, so the comparison
 *    constant is materialised as a RELOAD of a CONST_INT into a scratch
 *    register -- which is why the ROM has it in r2 (call-clobbered) and why the
 *    then-block REBUILDS it in r3:
 *        rom   mov r3,#0xa0 / lsl r3,#0xc / str r3,[r5,#0xc]
 *        park  str r2, [r5, #0xc]            <-- reuses the compare's register
 *    The park named the constant as a local (`lim = 0xa0 << 12;`), so a pseudo
 *    holding the value was live at the store and cse substituted the register.
 *    Deleting the name restores the ROM's second materialisation.  The
 *    scheduler still splits `mov r2,#0xa0` and `lsl r2,#0xc` around the two
 *    struct copies exactly as the ROM does, with no statement there at all.
 *
 *    THE PARK'S PROSE NAMED THE WRONG CONSTANT.  It blamed `0xa0 << 12` being
 *    held in a CALLEE-SAVED register across __GetFlag and cited the r6 push.
 *    r6 actually holds `0x80 << 2`, the FLAG ID; `0xa0 << 12` is commoned into
 *    r2, which is not callee-saved.  Two different constants, and only the
 *    second is the length gap.
 *
 * 2. `int one = 1;` FOR THE HALFWORD STORE.  `*(short *)p = 1;` emits a POOL
 *    WORD -- 7 against the ROM's 6, size 92 against 88.  This is a
 *    COUNTER-EXAMPLE to docs/elevation.md's narrowed HImode-literal rule
 *    (docs/elevation.md:5214), which says `1` needs no local.  It does here,
 *    and the distinguishing feature is that the destination is reached through
 *    a CAST POINTER rather than a declared u16 member.
 *
 * 3. CSE_CFLAGS, and this is the family's SIXTH member.  With 1 and 2 the
 *    figure is 16 of 38 at 32 / 32 and all 16 are gcc hoisting flag id
 *    `0x80 << 2` = 0x200 into r6 and keeping it live across __GetFlag, paying a
 *    wider push/pop and two `mov r0, r6` copies.  That is verbatim
 *    OvlFunc_936_20095b4 (batch 317, Makefile:570) -- same id, same register,
 *    same two copies.  Also OvlFunc_936_2009930 (batch 302, Makefile:554, id
 *    0x200), OvlFunc_948_20097ac (batch 219, Makefile:507).  And rom_77a7c8
 *    ALREADY carries a CSE_CFLAGS rule, ovl_30_c_a_c_c_a_c_c_c_c.o at
 *    Makefile:743, named in batch 219's comment as "the same rule for the same
 *    reason" -- so this is the second file in the bank, not a new assertion
 *    about it.
 *
 * LADDER, every rung measured with objcmp:
 *      park body, no flag                        30 / 32   25 of 38
 *      park body + CSE                           30 / 32   24 of 38  (STILL -2)
 *      literal in compare + literal store        32 / 32   pool 7 vs 6, 92/88
 *      + int one                                 32 / 32   16 of 38, 88/88
 *      + int one + CSE                           32 / 32   EXACT
 *
 * MEASURED INERT at 16 of 38 with the two source fixes: -fno-gcse,
 * -fno-rerun-loop-opt.  WORSE: -fno-schedule-insns2 (18).
 * AUTOSWEEP on the park body: flag axis closed (only CSE moved it, 25 -> 24,
 * and that row is MISALIGNMENT -- the counts still differ); anchor axis all 13
 * placements inert.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Actor_SetAnim(unsigned char *a, int n);

void OvlFunc_881_20097a4(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int one;
    a = __MapActor_GetActor(0xf);
    b = __MapActor_GetActor(0xe);
    *(int *)(a + 8) = *(int *)(b + 8);
    *(int *)(a + 0x10) = *(int *)(b + 0x10);
    if (*(int *)(a + 0xc) < (0xa0 << 12)) {
        *(int *)(a + 0xc) = 0xa0 << 12;
        if (__GetFlag(0x80 << 2) == 0) {
            __PlaySound(0x91);
            __Actor_SetAnim(a, 3);
            __SetFlag(0x80 << 2);
            p = a + 0x64;
            one = 1;
            *(short *)p = one;
        }
    }
}
