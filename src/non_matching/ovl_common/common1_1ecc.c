/* OvlFunc_common1_1ecc -- NON-MATCHING, 3 encodings of 100 differ (97 of 100 identical).
 * SIZE IDENTICAL (100 = 100 encodings).  Re-verified at 3 in batch 295.
 *
 * ONE OF THE THREE IS NOT CODEGEN.  Renaming the table extern to a name gcc will
 * never generate (`__asm__(".L9999")`) measures 2 of 100 -- so the third
 * difference is only the `.L5` capture described under LABEL below, and the real
 * residue is TWO ENCODINGS: one adjacent transposition.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common1_1ecc.c \
 *     asm/overlays/common/common1_c_a_c_c_a_c_c.s --func OvlFunc_common1_1ecc
 *
 * Allocates the 0x7170-byte work block (__galloc_ewram id 0x3b), records the seven
 * arguments at +0xde..+0xec, mirrors actor b about actor a on x (unless flag 0x109),
 * decompresses .L5 into a 0x200 scratch buffer, claims a sprite slot, uploads, and
 * starts task OvlFunc_common1_1b08.
 *
 * SPLIT SHAPE (batch 295).  The reference .s holds THREE functions --
 * OvlFunc_common1_1928 (lines 4-241), OvlFunc_common1_1b08 (243-701) and
 * OvlFunc_common1_1ecc (703-805) -- and NO data section, so datacheck.py prints
 * nothing and no data label has to be rehomed.  Landing 1ecc is a TEXT/TEXT split
 * of the last function off the end.  Required export, exactly one:
 *     .global OvlFunc_common1_1b08
 * in the retained .s -- 1ecc reads it at line 784 (`ldr r0, =OvlFunc_common1_1b08`)
 * and the file currently carries ZERO .global lines.  1928 is read by nobody in the
 * file and needs no export.
 *
 * AND THE LINKER SIDE IS SMALL, which the park had left open: `common1` is named by
 * exactly THREE overlay scripts -- overlays/rom_7db0c8/overlay.ld,
 * overlays/rom_7ddb88/overlay.ld and overlays/rom_7e0928/overlay.ld -- not the
 * nineteen that common0 needs.  In each, the object is one line
 * (rom_7db0c8/overlay.ld:73), and the `_TBL_L*` aliases are already grouped there
 * (rom_7db0c8:121 etc. carry _TBL_L10..L13 and _TBL_L16).  So the whole linker cost
 * is 3 stem lines + 3 alias lines.
 *
 * BLOCKER: a sched2 LUID tie at the __UploadSpriteGFX argument setup, and batch 295
 * establishes it is NOT REACHABLE FROM SOURCE.
 *     rom   mov r1,#0x80 / lsl r0,#16 / mov r2,r11 / lsl r1,#2 / asr r0,#16
 *     ours  mov r1,#0x80 / lsl r0,#16 / mov r2,r11 / asr r0,#16 / lsl r1,#2
 * The -fsched-verbose=6 forward-dependence table for the call block reads
 *     insn  code  bb  dep  prio  cost            depends
 *      264   113   0    2    68     1   : 387 276 273     (asr r0,#16)
 *      377   112   0    2    68     1   : 387 278 273     (lsl r1,#2)
 * so rank_for_schedule's FIRST tie-breaker, INSN_PRIORITY, is 68 = 68, and its
 * SECOND reachable one, forward depend_count, is 3 = 3.  Every earlier test either
 * does not apply after reload (INSN_REG_WEIGHT is guarded by !reload_completed) or
 * is class-3 for both (neither depends on the last-scheduled insn 272, mov r2,fp).
 * It therefore falls to INSN_LUID, and luid order here is fixed by the ARGUMENT
 * INDEX: both precompute_register_parameters (calls.c:810) and
 * load_register_parameters (calls.c:1692) iterate `for (i = 0; i < num_actuals; i++)`,
 * LOAD_ARGS_REVERSED is not defined for ARM, and arg0's conversion insns are emitted
 * by the convert_modes at calls.c:836 before arg1 is ever looked at.  Nothing a
 * source spelling can do puts arg1's insns first.  sched1 does not run (no .16.sched
 * dump appears under -da), so there is no earlier scheduler to reorder them either.
 *
 * DELTA to the inert list, batch 295 -- 17 further spellings, ALL at exactly 3:
 *   * short-typed first parameter in the prototype, passing the int slot;
 *   * short-typed first parameter, passing w->fd8;
 *   * `(int)(short)slot`;
 *   * `size = 0x200` in the join block COUPLED with the w->fd8 argument, and the
 *     same coupled with `(short)slot` -- the park had measured each alone, and the
 *     pair does not pay;
 *   * the size as a one-member struct member and as a union member (the
 *     nonzero_bits carriers); the size as `size = 0x200` used as the argument's
 *     own assignment expression (`__UploadSpriteGFX((short)slot, size = 0x200, buf)`);
 *   * the sign extension split across statements (`t = slot << 16;` then `t >> 16`);
 *   * the whole slot expression folded into arg0 (`(short)(w->fd8 = slot = __AllocSpriteSlot())`).
 * DROPPING the (short) cast is 31 of 100 -- the extension is genuinely wanted.
 *
 * AND THE DOCUMENTED SPLIT-PLUS-BARRIER LEVER IS DESTRUCTIVE HERE, measured:
 * `size = 0x80; __asm__ volatile ("" : "+r"(size)); size <<= 2;` gives 26 at the
 * join, 87 at the top of the function, 24 immediately before the call, 27 without
 * the split, 26 with the shift moved into the argument.  The reason is visible in
 * the diff: the barrier forces the value into its own register (r5) and costs an
 * extra `adds r1, r5, #0` before the call, because the ROM builds the size IN the
 * argument register (`movs r1,#0x80 / lsls r1,r1,#2`) and only
 * load_register_parameters can do that.  A pre-call statement can never be the
 * argument load, so the two requirements -- earlier luid, and built in r1 -- are
 * mutually exclusive.  THAT IS THE NEGATIVE.
 *
 * INERT from earlier batches (unchanged): passing w->fd8 vs a named `int slot` with
 * `(short)slot`; `short` vs `int` prototype for the slot parameter and for
 * __AllocSpriteSlot's return; `w->fd8 = slot = ...` chained; the whole assignment as
 * the argument; naming the size `int size = 0x200` in the entry block, after the
 * flag test, in both arms, or right before the call.  A `short slot` local is
 * catastrophic (35 of 100).
 *
 * LABEL: `.L5` is a LOW-numbered gcc label, so `extern ... __asm__(".L5")` collides
 * with gcc's own `.L5` -- objcmp shows R_ARM_ABS32 `.text` where the ROM has `.L5`,
 * and the captured pool word (`.word 0xd8`, the offset of gcc's own .L5) is the
 * third differing encoding.  `.L5` is EXTERNAL to this object: it is defined in the
 * sibling asm/overlays/common/common1_c_a_c_c_a_c_b.s:52 and referenced here twice
 * (lines 325 and 771).  The remedy is the one the landed sibling
 * src/overlays/common/common1_c_a_c_c_a_c_b.c already used for `.L16`: add
 * `_TBL_L5 = .L5;` to the three overlay.ld files beside _TBL_L10..L13/_TBL_L16 and
 * spell it `extern unsigned char L5[] __asm__("_TBL_L5");` here.
 *
 * SHIMS -- NONE:
 *   register class:  0
 *   .equ class:      0
 *   other __asm__:   0   (the `__asm__(".L5")` on the extern is a symbol NAME, not
 *                         an .equ and not a register pin)
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

struct Work {
    unsigned char pad00[0xd8];
    short fd8;
    short fda;
    short fdc;
    short fde;
    short fe0;
    short fe2;
    short fe4;
    short fe6;
    int fe8;
    int fec;
};

extern unsigned char L5[] __asm__(".L5");
extern void OvlFunc_common1_1b08(void);
extern void *__galloc_ewram(int id, int size);
extern void *__Func_8004970(int size);
extern struct Actor *__MapActor_GetActor(int id);
extern int __GetFlag(int flag);
extern void __DecompressLZ(void *src, void *dst);
extern int __AllocSpriteSlot(void);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern void __StartTask(void (*f)(void), int n);
extern void __free(void *p);

void OvlFunc_common1_1ecc(int a, int b, int c, int d, int e, int f, int g)
{
    struct Work *w;
    void *buf;
    struct Actor *pa;
    struct Actor *pb;
    int slot;

    w = __galloc_ewram(0x3b, 0x7170);
    buf = __Func_8004970(0x200);
    w->fde = a;
    w->fe0 = b;
    w->fe2 = f;
    w->fe4 = g;
    w->fe6 = c;
    w->fe8 = d;
    w->fec = e;
    pa = __MapActor_GetActor(a);
    pb = __MapActor_GetActor(b);
    if (!__GetFlag(0x109)) {
        pb->f8 = d * 2 - pa->f8;
        pb->f10 = pa->f10;
    }
    w->fda = 0;
    w->fdc = 0;
    __DecompressLZ(L5, buf);
    slot = __AllocSpriteSlot();
    w->fd8 = slot;
    __UploadSpriteGFX((short)slot, 0x200, buf);
    __StartTask(OvlFunc_common1_1b08, 0xc76);
    __free(buf);
}
