/* OvlFunc_966_200920c -- PARKED at 25 differing instructions of 302 (tryc
 * NON-MATCHING, 256 encodings of 306.  NOT a distance (ref 716 bytes / 306 encodings against
 * ours 704 / 301).  READ `--align`: 25 of 302.  All 47 relocations present with an identical
 * symbol multiset and identical types; only offsets shift.  Whole-file conversion -- one
 * function, no export, the .o keeps its name and slot.  Three register pins (one PIN3 block).
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * AN ORDERING CONSTRAINT ON THE NEXT ATTEMPT, proven rather than guessed.  Eight of the 25 are
 * one missing pseudo: the ROM carries a SECOND zero-valued pseudo, born at actor 9 and live to
 * the last `strb`.  This candidate has one, because cse's record_jump_equiv equivalence for the
 * __GetFlag result covers every zero in the else branch, and any second zero written in source is
 * folded back by reload1.c's reload_cse_regs once local-alloc puts both quantities in the same
 * register.  Forcing them apart with a barrier takes 25 -> 21 and ALSO removes a `mov` copy,
 * because with the register occupied to the end the commoned pool constant has no callee-saved
 * register and reload rematerialises it at both uses -- which is what the ROM does.
 * SO: do not retry the pool-constant residue until the second zero exists.  The same mechanism
 * costs four more positions in the head.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f148c/200920c.c \
 *     asm/overlays/rom_7f148c/ovl_30_c_c_c_c.s --func OvlFunc_966_200920c
 * --align), 256 of 306 encodings and size 704 against the ROM's 716 (objcmp
 * --whole).  NOT a true distance: our instruction count is 300 against 302.
 *
 * WHOLE-FILE CONVERSION.  `grep -ci func_start
 * asm/overlays/rom_7f148c/ovl_30_c_c_c_c.s` = 1, measured by me.  The .s carries
 * .data but the function reads no data label, so no new export is needed and the
 * .o keeps its name and its slot in overlays/rom_7f148c/overlay.ld.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7f148c/200920c.c \
 *     asm/overlays/rom_7f148c/ovl_30_c_c_c_c.s --whole
 *
 * Path: plain C 70 -> 74 -> 60 -> 45 -> 29 -> 25.
 *
 * ================= WHAT IS SETTLED, WITH ITS SINGLE-DROP =================
 *
 * 1. THE gState BASE IS A LOCAL, spelled out over a typed GlobalState, the same
 *    idiom as src/overlays/rom_79aad8/ovl_314_a.c; otherwise 0x1c2 folds into
 *    the pool addend as `ldr =gState+450` and the ROM's four-instruction
 *    `ldr =gState / mov #0xe1 / lsl #1 / add` collapses to one.  `off = 0`
 *    before the SIGNED read is load-bearing a second time: it makes that read a
 *    REGISTER-OFFSET address, distinct from the unsigned read's
 *    immediate-offset one, so cse keeps both loads.  Written `*(short *)p` the
 *    two reads common into ONE `ldrh` and the 0x5a compare narrows to HImode
 *    too -- 74 of 302 instead of 60.
 *
 * 2. NEW, AND THE ONE I WOULD PUT IN elevation.md: THE HImode COMPARE SURVIVES
 *    ONLY BECAUSE ITS CARRIER ALSO HELD A SYMBOL.  `mov #0xb6 / lsl r3,r2,#16 /
 *    lsl #15 / cmp r3,r0` is gcc's HImode equality compare (0x5b0000 built at
 *    the smallest legal shift, 15, by arm.md's thumb movsi split).  combine's
 *    simplify_comparison deletes that shift pair whenever `reg_nonzero_bits`
 *    proves the carrier is zero-extended, and combine.c's
 *    set_nonzero_bits_and_sign_copies UNIONS over every set of the pseudo -- two
 *    `ldrh` sets union to 0xffff, so a clean `int v` carrier gives
 *    `mov r3,r2 / cmp r3,#0x5b` (measured, probe scratch_elev/b294/D/probe/s.c
 *    t2).  REUSING the variable that held `(unsigned int)&gState` as the
 *    halfword carrier adds a SYMBOL_REF set whose nonzero_bits are unknown, and
 *    the shift pair comes back byte-exact (probe t1).  Reusing the __GetFlag
 *    carrier (probe q5) also works but forces the halfword into a callee-saved
 *    register where the ROM has r2, because that carrier is live across calls.
 *    A declared `unsigned short` carrier is NOT an option: ARM's PROMOTE_MODE
 *    (arm.h:597) gives HImode `UNSIGNEDP = TARGET_MMU_TRAPS != 0` = 0 here, so
 *    it emits `ldrsh` where the ROM has `ldrh` (probes s4/q4/q6, all three).
 *
 * 3. THE 0x14/0x15 TAIL TEST IS NOT A RANGE FOLD ON THE HALFWORD.  Spelled
 *    `v == 0x14 || v == 0x15` over the dereference, fold-const.c's
 *    build_range_check does the subtraction in the operand's own type --
 *    unsigned short -- so -0x14 becomes 0xffec, has no `sub #imm8` form and
 *    POOLS: `ldr r1,=0xffec / add r3,r1`.  The ROM's `sub r3,#0x14` is an int
 *    subtraction under a HImode compare, i.e. `(unsigned short)(w - 0x14) <= 1`
 *    with `w` an int (probe p.c s7 exact, s6 the pooled control).
 *
 * 4. THE TWO SPRITE MASKS ARE BITFIELDS.  `s->f5_b5 = 0` gives the ROM's
 *    `mov #0x21 / neg` (a 32-bit ~0x20) and `s->f9_hi = 0` the bare `mov #0xf`
 *    that cse then shares into r8 with the 0xf argument of the second
 *    __Func_8010704.  Written as ordinary masking on an `unsigned char *`,
 *    `s[5] &= ~0x20` narrows to `mov r3,#0xdf` -- v6, +2.  Same pair as the
 *    sibling src/overlays/rom_7b4558/ovl_30_c_c_c_c_a.c.
 *
 * 5. THE ACTOR IS AN `unsigned char *`, THE SPRITE STRUCT IS TYPED.  With a
 *    typed actor struct gcc's alias sets let cse common the two `[r0,#0x50]`
 *    loads the ROM keeps apart -- the store through the loaded pointer must kill
 *    them, which only happens when the load's MEM is char-based (alias set 0).
 *    Worth 2 loads x 2 actors.
 *
 * 6. ONE FUNCTION-SCOPE `unsigned char *a` FOR EVERY ACTOR POINTER, not a
 *    block-local per group: the block-locals cost a `mov r2,r0` copy each where
 *    the ROM works out of r0 (v7 45 -> v8 29, together with the pin).
 *
 * 7. A NAMED `int one` RATHER THAN THE LITERAL 1, assigned immediately before
 *    its first store.  The ROM routes even the FIRST use through r10
 *    (`mov r1,#1 / mov sl,r1 / mov r2,sl / strb r2`); with literals reload
 *    reuses the r1 copy for the first store and copies to sl later.  29 -> 25,
 *    and it is the only lever that moved after v8.
 *
 * 8. THE HImode CONSTANT STORES NEED AN `int` CARRIER: written inline,
 *    `*(unsigned short *)(...) = 0xc0 << 8` pools (`ldr r3,=0xc000`) because
 *    thumb movhi has no CONST_INT alternative; `n = 0xc0 << 8;` immediately
 *    before the store gives the ROM's `mov #0xc0 / lsl #8`.
 *
 * 9. `__UploadSpriteGFX` IS DELIBERATELY UNDECLARED -- the second-declaration
 *    lever that puts r0 last in its own argument block, as in the sibling.
 *
 * 10. THE if/else POLARITY IS THE ROM'S: writing `if (f == 0) { else-body }`
 *    is 133 of 302 (v14b).  `f = __GetFlag(0x9a7); if (f != 0)` and the direct
 *    `if (__GetFlag(0x9a7) != 0)` are byte-identical (v14a inert): cse's
 *    record_jump_equiv makes the result the zero the else branch stores, which
 *    is what puts the `mov r6, r0` copy in the ROM.
 *
 * 11. `ldrh r6, <pool label>` in our .s and `ldr r6, =0` in the reference are
 *    the SAME ENCODING -- both assemble to `ldr r6,[pc,#60]` over a `.word 0`
 *    with no relocation (checked with arm-none-eabi-as/objdump on both).  Not a
 *    difference to chase, exactly as the brief warned.
 *
 * ======================== THE BLOCKER, NAMED ========================
 *
 * ONE MISSING PSEUDO EXPLAINS 8 OF THE 25, AND IT IS reload's REMATERIALISATION
 * CHAIN.  The ROM carries a SECOND zero-valued pseudo, born at actor 9
 * (`ldr r6, =0`, index 196) and live to the last `strb r6` at index 291.  Ours
 * has only one: cse's record_jump_equiv equivalence for the __GetFlag(0x9a7)
 * result covers every zero in the else branch, and whatever second zero I write
 * (`int z2 = 0`, v11b) is folded straight back onto it -- reload1.c's
 * `reload_cse_regs` deletes the redundant `mov r6,#0` once local-alloc has put
 * both quantities in r6.
 *
 * That single pseudo is what makes the ROM's tail differ, and the coupling is
 * PROVEN, not guessed: forcing the second zero apart with
 * `__asm__ volatile ("" : "+r" (z2))` (scratch_elev/b294/D/t2_v15shim.c) takes
 * 25 -> 21 and removes exactly the two `strb r5`-against-`strb r6` positions AND
 * the `mov r5, r0` copy of the __GetFlag(0x9b8) result.  With r6 occupied to the
 * end, the commoned 0x9b8 pool constant has no callee-saved register left, so
 * the ROM's reload REMATERIALISES it at both uses -- `ldr r0,=0x9b8` twice,
 * where ours keeps it in a register and copies (`ldr r5,=0x9b8 / mov r0,r5`
 * twice).  gcc always commons two identical pool loads into a callee-saved
 * register when one is free: probe scratch_elev/b294/D/probe/x.c, three
 * spellings, all three common.  So the 0x9b8 residue is NOT a separate problem
 * and must be retried only AFTER the second zero exists.
 *
 * WHAT IS LEFT, and none of it is worth a shim:
 *   - 4 positions in the head: the ROM gives the gState symbol r3 and the
 *     ldrsh's zero offset the short-lived r1, and materialises `z = 0` again at
 *     index 48.  Ours gives `off` and `z` the same hard register r7 and
 *     `reload_cse_regs` deletes the second `mov`.  Same mechanism as the
 *     blocker above, one register earlier.
 *   - 2 positions: `mov r10, r1` one slot later than the ROM (sched2).
 *   - 8 positions: at both `strh r?,[r?,#0x1e]` sites the ROM holds the
 *     constant in r3 and the sprite pointer in r2, ours the reverse.
 *   - 3 positions: the missing `ldr r6,=0` and the `b L5 / L5:` minipool break
 *     that gcc inserts to place its word.
 *
 * MEASURED AND INERT (each a single drop from this file, all 25):
 *   `f` written into the last two `[0x5b]` stores; dropping `z` for literal
 *   zeros; an extra `int z2 = 0`; block-local `int n` per group; a named
 *   `unsigned char *t` for the sprite pointer; `unsigned int n`; hoisting the
 *   `n =` assignment above the f26 store or above the `|= 2`; three positions
 *   for `int n` in the declaration list (nothing spills here -- the frame is
 *   only the 8 outgoing-argument bytes, so the declaration-order lever has no
 *   surface); `off = 0` moved after the carrier load; `z = 0` hoisted to the top
 *   of the function; the direct `if (__GetFlag(0x9a7) != 0)` form; and a PIN1 on
 *   `__SetFlag(0x9b8)` (dropped from this file for that reason).
 *   REGRESSIONS: swapping the f1e and f26 stores (31), inverting the if/else
 *   (133).
 *
 * SHIMS: ONE, in the `register ... __asm__` class -- the PIN3 block at the
 * __MapActor_SetPos site, worth 27 -> 25 on a single drop and the same
 * ascending-fill convention as src/overlays/rom_7f148c/ovl_30_c_c_c_a_a_b.c.
 * ZERO shims in the `__asm__(".equ ...")` class.  No .sym entry is implied:
 * 0x9a7, 0x9bb, 0x9bf and 0x9b8 are all unshiftable (odd or with too wide a bit
 * span) so they pool either way and discriminate nothing, and the reference
 * carries no R_ARM_ABS32 in this function beyond gState and iwram_3001ebc --
 * which our relocation list reproduces exactly, same two symbols at the same
 * two pool offsets 0x198 and 0x1a4.
 */
struct S {
    unsigned char pad00[5];
    unsigned char f5_lo : 5;
    unsigned char f5_b5 : 1;
    unsigned char f5_hi : 2;
    unsigned char pad06[3];
    unsigned char f9_lo : 2;
    unsigned char f9_mid : 2;
    unsigned char f9_hi : 4;
    unsigned char pad0a[0x12];
    unsigned char f1c;
    unsigned char pad1d[0xa];
    unsigned char f27;
};

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void *__galloc_iwram(int tag, int n);
extern void __gfree(int tag);
extern void __LoadItemIcon(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_966_2008218(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN3 PIN1; \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

int OvlFunc_966_200920c(void)
{
    struct S *s;
    void *buf;
    int z;
    int f;
    int w;
    int n;
    int one;
    unsigned char *a;
    unsigned int base;
    unsigned int off;
    unsigned int p;

    base = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    p = base + off;
    off = 0;
    base = *(unsigned short *)p;
    if (*(short *)((char *)p + off) == 0x5a) {
        __SetFlag(0x9a7);
        __SetFlag(0x9bf);
        base = *(unsigned short *)p;
    }
    if ((unsigned short)base == 0x5b)
        __SetFlag(0x9a7);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x18;
    __MapActor_SetAnim(0x13, 3);
    z = 0;
    __MapActor_GetActor(0x13)[0x59] = z;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
    __MapActor_SetAnim(0x14, 3);
    __MapActor_GetActor(0x14)[0x59] = z;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x14), 0);
    __MapActor_SetAnim(0x15, 3);
    __MapActor_GetActor(0x15)[0x59] = z;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    a = __MapActor_GetActor(0x19);
    {
      one = 1;
      a[0x5c] = one;
      a[0x55] = z;
      s = *(struct S **)(a + 0x50);
      *(int *)(a + 0xc) = 0xa0 << 12;
      s->f27 = z;
      s->f5_b5 = 0;
      s->f9_hi = 0; }
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(0xf2);
    __UploadSpriteGFX(s->f1c, 0x80, (char *)buf + (0x80 << 3));
    __gfree(0x11);
    f = __GetFlag(0x9a7);
    if (f != 0) {
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
        __MapActor_GetActor(0x18)[0x59] = z;
        __Func_8010704(0x14, 0x17, 1, 1, 0xe, 4);
        __Func_8010704(0x14, 0x17, 1, 1, 0xf, 4);
        __Func_8010704(0x14, 0x17, 1, 1, 0x10, 4);
        if (__GetFlag(0x9bb) != 0) {
            PIN3; q1 = 0xe0; q2 = 0xb8; q0 = 0x12; q1 <<= 14; q2 <<= 16;
            __MapActor_SetPos(q0, q1, q2);
        }
    } else {
        a = __MapActor_GetActor(8);
        {
          a[0x59] = 0;
          a[0x23] |= 2;
          (*(unsigned char **)(a + 0x50))[0x26] = 0;
          n = 0xc0 << 8;
          *(unsigned short *)(*(unsigned char **)(a + 0x50) + 0x1e) = n; }
        a = __MapActor_GetActor(9);
        {
          a[0x59] = 0;
          a[0x23] |= 2;
          (*(unsigned char **)(a + 0x50))[0x26] = 0;
          n = 0x80 << 7;
          *(unsigned short *)(*(unsigned char **)(a + 0x50) + 0x1e) = n; }
        __Func_8010704(0x14, 0x17, 1, 1, 0xd, 0x17);
        __Func_8010704(0x14, 0x17, 1, 1, 0xe, 0x17);
        __Func_8010704(0x14, 0x17, 1, 1, 0x4e, 0x17);
        __Func_8010704(0x14, 0x17, 1, 1, 0x11, 0x17);
        __Func_8010704(0x14, 0x17, 1, 1, 0x12, 0x17);
        w = *(unsigned short *)p;
        if ((unsigned short)(w - 0x14) <= 1) {
            if (__GetFlag(0x9b8) == 0) {
                __SetFlag(0x9b8);
                a = __MapActor_GetActor(0xb);
                a[0x5b] = one;
                a = __MapActor_GetActor(0x11);
                a[0x5b] = one;
                OvlFunc_966_2008218();
                a = __MapActor_GetActor(0xb);
                a[0x5b] = 0;
                a = __MapActor_GetActor(0x11);
                a[0x5b] = 0;
            }
        }
    }
    return 0;
}
