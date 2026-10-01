/* OvlFunc_common1_1ecc -- asm/overlays/common/common1_c_a_c_c_a_c_c.s, third of
 * three functions.
 *
 * *** ZERO ENCODINGS DIFFER.  objcmp at production flags (plain -O2) reports
 * *** 100 of 100 encodings identical, 0 differing slots, histogram identical.
 * *** The ONLY line objcmp still prints is `XX RELOCATIONS differ`, and it is
 * *** the _TBL_L5 / .L5 NAME, not code: objcmp treats two names as one symbol
 * *** only when a LINKED ELF proves the same address, and the alias is not in
 * *** the three overlay.ld files yet.  ADD THE THREE ALIAS LINES AND THIS IS A
 * *** BYTE-IDENTICAL MATCH.  Measured proof that nothing else remains: with the
 * *** extern spelled `__asm__(".L9999")`-style (any name gcc will never emit)
 * *** objcmp drops to 1 differing encoding, the captured pool word; with
 * *** `_TBL_L5` the encoding count is 0 of 100.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_common/common1_1ecc.c \
 *     asm/overlays/common/common1_c_a_c_c_a_c_c.s --func OvlFunc_common1_1ecc
 * (after landing, the same recipe against
 *  src/overlays/common/common1_c_a_c_c_a_c_c_b.c)
 *
 * ============================================================================
 * BATCH 314: THE sched2 TIE WAS NOT UNREACHABLE.  IT WAS THE CALLEE'S RETURN
 * TYPE, AND IT COST ONE WORD OF SOURCE.
 * ============================================================================
 *
 * The residue was one adjacent transposition in the __UploadSpriteGFX argument
 * block:
 *     rom   mov r1,#0x80 / lsl r0,#16 / mov r2,r11 / lsl r1,#2  / asr r0,#16
 *     ours  mov r1,#0x80 / lsl r0,#16 / mov r2,r11 / asr r0,#16 / lsl r1,#2
 *
 * The park had the PASS right and the TERM wrong.  Its claim was that
 * rank_for_schedule's reachable tie-breakers are exhausted -- priority 68 = 68,
 * forward depend_count 3 = 3 -- so INSN_LUID decides, and LUID is fixed by
 * calls.c iterating arguments 0..n-1, which no spelling reorders.  The
 * PRIORITY and LUID halves of that are correct and were re-derived.  THE
 * DEPENDENT COUNT IS NOT FIXED, AND IT IS WHERE THE FUNCTION OPENS.
 *
 * -fsched-verbose=6, basic block 2, as found:
 *     insn  code  dep  prio          INSN_DEPEND
 *      263   112    2    69   : 387 264               lsl r0,#16
 *      264   113    2    68   : 387 276 273           asr r0,#16     <- 3
 *      376   173    3    69   : 387 377               mov r1,#0x80
 *      377   112    2    68   : 387 278 273           lsl r1,#2      <- 3
 *      272   173    5    68   : 388 387 287 283 273   mov r2,fp      <- 5
 * 272 wins the third slot on dependent count, which is already proof that the
 * term bites here.  264 and 377 then tie at 3 and fall to LUID, and
 * `-fno-schedule-insns2` confirms the pre-sched2 stream is
 * lsl r0 / asr r0 / mov r1 / lsl r1 / mov r2, so 264 precedes 377 and wins.
 *
 * READ 264's THIRD DEPENDENT.  It is insn 276 -- the `ldr r0, =<task>` that
 * loads __StartTask's first argument, AFTER the __UploadSpriteGFX call.  That
 * is an OUTPUT dependence on r0 reaching BACK ACROSS A CALL, and it exists
 * because __UploadSpriteGFX was declared `void`: a void call has no `set r0`,
 * so it never becomes r0's last setter and never intercepts the chain.  377's
 * third dependent, insn 278, is the symmetric `mov r1, #0xc76` for the same
 * call, and nothing can take that away from it.
 *
 * SO THE LEVER IS TO SHORTEN 264's LIST, NOT TO LENGTHEN 377's:
 *
 *     extern int __UploadSpriteGFX(int slot, int size, void *src);
 *
 * One word -- `void` to `int`.  The call becomes a call_value whose `set r0`
 * IS r0's last setter, insn 276's output dependence retargets onto the call,
 * 264 drops to 2 dependents, 377 keeps 3 and WINS THE DEPENDENT-COUNT TEST
 * BEFORE LUID IS EVER CONSULTED.  The return value is unused, so not one
 * instruction, byte or relocation changes anywhere else: the figure goes
 * straight from 3 differing encodings to 0.
 *
 * THE GENERAL RULE, worth carrying to every sched2 LUID park: AN ARGUMENT-
 * REGISTER SETTER'S DEPENDENT COUNT INCLUDES EVERY LATER WRITE OF THAT HARD
 * REGISTER IN THE SAME BASIC BLOCK, AND A VOID CALL DOES NOT SHIELD THEM --
 * ONLY A VALUE-RETURNING CALL DOES, AND THEN ONLY FOR r0.  Insn 272 above is
 * the same mechanism in the other direction: `mov r2,fp` counts all three
 * later calls (273, 283, 287) as dependents, because a call's CLOBBER of r2
 * never becomes r2's last setter either.  Declaring a callee's return type is
 * therefore a REAL CODEGEN LEVER on r0 chains, costing nothing when the value
 * is unused.
 *
 * ============================================================================
 * WHAT THE PARK HAD RIGHT, AND ITS LONG INERT LIST
 * ============================================================================
 * The whole batch-295 sweep stays valid and stays INERT, and all of it was
 * aimed at the LUID term, which is why none of it moved: 17 spellings at
 * exactly 3 (short-typed first parameter either way, `(int)(short)slot`,
 * `size = 0x200` coupled with the w->fd8 argument, the size as a struct or
 * union member, the size as the argument's own assignment expression, the sign
 * extension split across statements, the whole slot expression folded into
 * arg0); plus the earlier set (w->fd8 vs a named `int slot`, `short` vs `int`
 * prototypes, `w->fd8 = slot = ...` chained, naming the size in four
 * positions).  DROPPING the (short) cast is 31 of 100 -- the extension is
 * genuinely wanted.  A `short slot` local is 35 of 100.
 *
 * AND THE SPLIT-PLUS-BARRIER LEVER REMAINS DESTRUCTIVE, measured:
 * `size = 0x80; __asm__ volatile ("" : "+r"(size)); size <<= 2;` gives 26 at
 * the join, 87 at the top of the function, 24 immediately before the call, 27
 * without the split, 26 with the shift moved into the argument.  The barrier
 * forces the value into its own register and costs an extra `adds r1, r5, #0`,
 * because the ROM builds the size IN the argument register
 * (`movs r1,#0x80 / lsls r1,r1,#2`) and only load_register_parameters can do
 * that.  A pre-call statement can never BE the argument load.  Keeping this
 * function PIN-FREE was therefore never a constraint to work around -- the pin
 * was always worse.
 *
 * STRUCK: the park's "-fno-schedule-insns is inert" note rules nothing out.
 * sched1 does not run in this build; that flag controls a pass that never runs.
 *
 * ============================================================================
 * LANDING PREREQUISITES
 * ============================================================================
 *  * PIN-FREE.  tools/shimcount.py exits 0 with no findings.  The
 *    `__asm__("_TBL_L5")` on the extern is a symbol NAME, not an .equ and not a
 *    register pin.  NO fakematch.txt ROW, no Makefile row (plain -O2).
 *
 *  * TEXT/TEXT SPLIT, NO DATA.  tools/datacheck.py on the reference is SILENT
 *    (exit 0) -- re-run in batch 314, not inherited.
 *    tools/split_s.py --dry-run asm/overlays/common/common1_c_a_c_c_a_c_c.s
 *    OvlFunc_common1_1ecc:
 *        would write ..._a.s  (2 functions, 699 lines)  <- 1928 + 1b08 stay asm
 *        would write ..._b.s  (1 function, 104 lines)   <- this function
 *        would REMOVE ..._a_c_c.s, would rewrite all three overlay.ld files
 *    Landed file: src/overlays/common/common1_c_a_c_c_a_c_c_b.c
 *
 *  * EXACTLY ONE NEW EXPORT in the retained _a.s:
 *        .global OvlFunc_common1_1b08
 *    This function reads it (`ldr r0, =OvlFunc_common1_1b08`) and the file
 *    carries ZERO .global lines today.  OvlFunc_common1_1928 is read by nobody
 *    in the file and needs no export.
 *
 *  * THREE LINKER ALIAS LINES, one per overlay.ld that names common1 --
 *    overlays/rom_7db0c8/overlay.ld, overlays/rom_7ddb88/overlay.ld,
 *    overlays/rom_7e0928/overlay.ld -- beside the _TBL_L10..L13/_TBL_L16 block
 *    already there (rom_7db0c8:122-128):
 *        _TBL_L5 = .L5;
 *    plus the one stem line each that split_s.py rewrites.  This is the same
 *    remedy the landed sibling src/overlays/common/common1_c_a_c_c_a_c_b.c used
 *    for .L16, and it is needed because `.L5` is a LOW-numbered gcc label:
 *    spelling the extern `__asm__(".L5")` collides with gcc's own .L5 in this
 *    object and objcmp then shows R_ARM_ABS32 `.text` with a captured pool word
 *    `.word 0xd8` (the offset of gcc's own .L5) instead of `.L5`.
 *
 *  * THE ALIAS IS UNAMBIGUOUS -- CHECKED IN BATCH 314, BECAUSE IT IS THE ONE
 *    THING THAT COULD HAVE SUNK THIS.  26 to 44 of the objects linked into each
 *    of the three overlays contain a `.L5:` definition, which looks fatal for a
 *    linker-script alias.  It is not: those are assembler local labels and as
 *    discards them.  EXACTLY ONE FILE IN THE WHOLE TREE carries `.global .L5`,
 *    asm/overlays/common/common1_c_c_b_c.s, and it is linked into all three
 *    overlays; arm-none-eabi-nm on overlays/rom_7db0c8/overlay.elf shows a
 *    single global `.L5` at 0200bf14 (beside `.L16` and `_TBL_L16` at
 *    0200c6fc, the working precedent).  So `_TBL_L5 = .L5;` binds one symbol.
 *
 *  * CORRECTION TO THE PARK: it named the .L5 definition site as
 *    asm/overlays/common/common1_c_a_c_c_a_c_b.s:52.  That file does have a
 *    `.L5:` at line 52, but it is a LOCAL label that never reaches the symbol
 *    table -- that file's only .global is OvlFunc_common1_1814.  The .L5 this
 *    function actually reaches is the globalised one in
 *    asm/overlays/common/common1_c_c_b_c.s.  The remedy is unaffected; the
 *    attribution was wrong and would have sent someone to edit the wrong file.
 *
 *  * NO SHARED-DATA EDIT IS NEEDED, which settles the open owner decision for
 *    THIS function: nothing has to be renamed.  The alias adds a name, it does
 *    not take one away, so `.L5` keeps working for the still-asm
 *    OvlFunc_common1_1b08 in the retained _a.s (which reads it at _a.s-relative
 *    line 325) and for every other object.
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

extern unsigned char L5[] __asm__("_TBL_L5");
extern void OvlFunc_common1_1b08(void);
extern void *__galloc_ewram(int id, int size);
extern void *__Func_8004970(int size);
extern struct Actor *__MapActor_GetActor(int id);
extern int __GetFlag(int flag);
extern void __DecompressLZ(void *src, void *dst);
extern int __AllocSpriteSlot(void);
extern int __UploadSpriteGFX(int slot, int size, void *src);
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
