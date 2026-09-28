/* BaseAnim_FullScreenSlash  [rom_c9000]  --  asm/rom_c9000/rom_ece7c_c_c.s
 *
 * NON-MATCHING: 97 encodings of 220 differ (objcmp).
 * Size 520 against the ROM's 528 (-8); 216 instructions against 220 (-4).
 * THE RELOCATION SEQUENCE IS IDENTICAL -- all 36 symbols in the same order,
 * offsets shifted only by the 8-byte deficit.  97 is therefore NOT a distance:
 * four instructions are missing and every later encoding is renamed by the shift.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/ece7c_FullScreenSlash.c \
 *     asm/rom_c9000/rom_ece7c_c_c.s --func BaseAnim_FullScreenSlash
 *
 * TWO functions in that .s (this one and Anim_UndeadSword) AND A .rodata TAIL.
 * datacheck.py reports: data sections .rodata; functions BaseAnim_FullScreenSlash,
 * Anim_UndeadSword; "converting a function here needs a TEXT/DATA SPLIT".  The
 * .rodata holds .Leef88 (.incrom 0xeef88,0xeef96) and .Leef96
 * (.incrom 0xeef96,0xeefa4) -- 14 bytes each, NEITHER declared .global, and both
 * referenced ONLY by Anim_UndeadSword (lines 503 and 510).  Read out of
 * baserom.gba they are unsigned short[7]:
 *     .Leef88 = { 0, 0x100, 0x500, 0xe00, 0x1700, 0x2000, 0x2900 }
 *     .Leef96 = { 0x10, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30 }
 * so a split must export BOTH labels across the new boundary (batch 290's rule:
 * export every label referenced across the boundary in either direction, not
 * just the ones datacheck lists as already .global -- it lists none here).
 *
 * ================================================================
 * THE WHOLE RESIDUE IS ONE INSN'S POSITION, AND IT IS A CROSS-JUMPING
 * CONSEQUENCE, NOT A SCHEDULING ONE
 * ================================================================
 *
 * The ROM's four blit arms are
 *     arm1  str r6,[sp] / str r6,[sp,#4] / mov r0,r8 / mov r1,r7      / b .Led030
 *     arm2  mov r2,#0xe1 / lsl / add r1,r7,r2 / str / str / mov r0,r8 / b .Led030
 *     arm3  str / str / mov r0,r8 / ldr r1,=gBuffer                   (falls through)
 *     .Led030  mov r2,#0 / mov r3,#0 / ldr r4,[sp,#8] / bl _call_via_r4 / b
 *     arm4  str / str / mov r0,r8 / ldr r1,=ewram_2013840 / <tail inline>
 * i.e. only the FOUR-INSN tail from `mov r2,#0` is shared.  Ours emits
 * `ldr r1,=gBuffer` FIRST in arms 3 and 4, which makes `str / str / mov r0,r8`
 * a common tail of arms 2 and 3, so jump2's find_cross_jump merges THREE more
 * instructions than the ROM's -- the entire 4-instruction deficit (the fourth is
 * the extra `b`).  Everything before instruction 36 and after the loop is
 * byte-identical.
 *
 * WHY THE `ldr` IS EARLY, FROM THE COMPILER'S OWN SOURCE.  On thumb an array
 * address is not a legitimate constant, so expand turns `gBuffer` into
 * `(mem/u:SI (symbol_ref "*.LC3"))` -- confirmed in the .00.rtl dump.  In
 * calls.c:2932 `precompute_register_parameters` runs BEFORE the stack args are
 * stored (calls.c:2942) and copies any reg argument whose
 * `rtx_cost (value, SET) > 2` into a pseudo there.  arm.c's thumb `arm_rtx_costs`
 * gives MEM `10 + (CONSTANT_POOL_ADDRESS_P ? 4 : 0)` = 14, and SYMBOL_REF
 * `COSTS_N_INSNS (3)` = 12.  BOTH exceed 2 unconditionally, and
 * `preserve_subexpressions_p ()` is 1 at -O2 because `flag_expensive_optimizations`
 * is set, so the second half of the guard cannot fail either.  A bare
 * symbol-address argument on thumb is therefore ALWAYS precomputed ahead of the
 * outgoing-stack stores -- there is no spelling of the argument that reaches the
 * ROM's late position.
 *
 * THE ONE ROUTE THAT COULD PUT IT LATE IS local-alloc.c's `update_equiv_regs`,
 * which either substitutes a REG_EQUIV value at a pseudo's single use or "moves
 * the initialization just before the use" (local-alloc.c:957-985).  Both need
 * `reg_equiv_replace[regno]`, which needs REG_N_REFS == 2 **and**
 * REG_BASIC_BLOCK < 0 (set and use in different blocks).  But the note itself is
 * only synthesised for a MEM source when REG_BASIC_BLOCK >= 0 (local-alloc.c:841),
 * and the earlier guard at local-alloc.c:789 throws the equivalence away outright
 * when `CLASS_LIKELY_SPILLED_P (reg_preferred_class (regno)) && src is MEM`.
 * The two conditions are mutually exclusive for a pool MEM, so this is a
 * BY-CONSTRUCTION dead end, not an unbeaten one.
 *
 * MEASURED, all on top of the file below:
 *   - `gbuf = gBuffer; ebuf = ewram_2013840;` before the loop reproduces the
 *     ROM's ARM STRUCTURE EXACTLY (arm2 keeps its own str/str/mov r0 and `b`,
 *     arm3 falls through, arm4 separate) but the two pointers take r9/r10, so
 *     `mov r1,r9` / `mov r1,sl` stand where the ROM has the pool loads and the
 *     prologue/epilogue grow: 219 of 220, 228 instructions.  Same at the top of
 *     the function (n1) and at the top of the loop body (o1, 105/218).
 *   - the same assignments in a DOMINATING nested block (o2, 99/218) materialise
 *     both pool loads at the top of that block instead.
 *   - INERT: `int` return on the DrawFn typedef, on the ClearFn typedef, on both;
 *     `unsigned char *` for the src parameter; `do { } while (0)` before the
 *     arm-3/arm-4 calls; literal `0x78, 0x78` instead of `w` (196/224, worse).
 *
 * ================================================================
 * FOUR LEVERS THAT LANDED EVERYTHING ELSE, ALL MEASURED
 * ================================================================
 *
 * THE MAIN LOOP IS A `goto` LOOP.  Its body hoists NOTHING -- not
 * `ldr r1,=gBuffer`, not `ldr r2,=0x3f3f3f3f`, not `ldr r3,=Func_80008d8`, not
 * the three `0x77a8`/`0x7828`/`0x7824` offsets.  Written as a `do ... while`
 * gcc hoists the `Func_80008d8` pool word into a callee-saved register before the
 * loop; with `again:` / `if (i != 0x15) goto again;` it does not, because loop.c
 * never sees a NOTE_INSN_LOOP_BEG.  136 of 220 -> the ROM's per-iteration form.
 *
 * `blit` IS A TWO-ELEMENT LOCAL ARRAY, AND ONE ELEMENT IS NOT ENOUGH.
 * `DrawFn blit[1]` is PROMOTED TO A REGISTER by gcc-2.96 (r8, `bl _call_via_r8`,
 * frame 0x8); `DrawFn blit[2]` with only `blit[0]` used is memory-resident, lands
 * at sp+8, gives `ldr r4,[sp,#8] / bl _call_via_r4` at both call sites, puts `ctx`
 * back in r8 and fixes the frame to the ROM's 0x10.  136 -> 97, and it also fixed
 * the three `ldr r4, =0x7784 / =0x7828 / =0x77a8` pool registers for free.
 * A plain `DrawFn blit;` local is worse still.  This is the Anim_Vine `DrawFn d[2]`
 * idiom (src/rom_c9000/rom_dd2ac_c_c_b.c) and the counterpart of
 * d9ab8_StatDown.c's "the blit callee must be loaded late".
 *
 * `blit[0] = (DrawFn)gPtrs[0x2e]` WITH `extern void *gPtrs[]` -- NOT
 * `*(DrawFn *)(gPtrs + 0xb8)`.  gPtrs is 0x03001E50 and 0xb8 later is
 * iwram_3001f08, which this file's OTHER function names directly; here the ROM
 * derives it: `ldr r3,=gPtrs / add r3,#0xb8 / ldr r3,[r3]`.  Pointer arithmetic on
 * a char array folds symbol+184 into one pool word (two instructions); an
 * ARRAY INDEX makes it a MEM at symbol+184 whose offset exceeds the thumb `ldr`
 * immediate range, so reload splits it into the ROM's three.  The two spellings of
 * one address in two functions 0x210 bytes apart are both correct.
 *
 * `REG_BLDCNT = 0;` (0x4000050, not REG_BLDALPHA) is the mid-function pool with
 * the `b` over it.  `ldrh rX, label` and `ldr rX, label` ASSEMBLE IDENTICALLY --
 * thumb has no PC-relative ldrh -- so the ROM's `ldr r3, .Lecf30 @ 0` is gcc's
 * `ldrh`, and the narrow HImode fixup range is what puts the pool mid-function.
 *
 * Also load-bearing and confirmed: `pp = tbl; base = *pp++; ctx = *pp;` for
 * `ldmia r3!, {r7}`; a 3-arm variant ladder whose two `LoadVFXFile(id, gBuffer, 1, 1)`
 * tails cross-jump into one; `((State *)context)->ids[0]` at 0x24 for the
 * register-offset `mov r2,#0x24 / ldrsh r1,[r5,r2]` (thumb LDRSH has no immediate
 * form); `i >= 0x10 && i <= 0x13` for the folded `sub r3,#0x10 / cmp #3 / bhi`;
 * and `clear = Func_80008d8; clear(...)` as a block-local inside the `if`.
 *
 * No new symbol is warranted.  No per-file Makefile flag override applies.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*ClearFn)(void *dst, int len, int val);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

extern int *iwram_3001eec[];
extern unsigned char gBuffer[];
extern void *gPtrs[];
extern unsigned char ewram_2013840[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _Func_80b82c4(int a, int b, int c, int d);
extern void WaitFrames(unsigned int n);
extern void BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void _PlaySound(int id);
extern void Func_80008d8(void *dst, int len, int val);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int a, int b);
extern void Func_80cd52c(void);
extern void gfree(int tag);

void BaseAnim_FullScreenSlash(void *context, int variant)
{
    int **pp;
    unsigned char *base;
    void *ctx;
    DrawFn blit[2];
    int i;
    int w;

    pp = iwram_3001eec;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDCNT = 0;
    if (variant == 0) {
        LoadVFXFile(FILE_4f, base, 1, 0);
        LoadVFXFile(FILE_50, gBuffer, 1, 1);
    } else if (variant == 1) {
        LoadVFXFile(FILE_4d, base, 1, 0);
        LoadVFXFile(FILE_4e, gBuffer, 1, 1);
    } else {
        LoadVFXFile(FILE_4b, base, 1, 0);
        LoadVFXFile(FILE_4c, gBuffer, 1, 1);
    }
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    if (variant == 1) {
        _Func_80b82c4(((State *)context)->f8, ((State *)context)->ids[0], 0x10, 0x80 << 12);
    } else {
        _Func_80b82c4(((State *)context)->f8, ((State *)context)->ids[0], 0x10, 0);
    }
    WaitFrames(0x10);
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 0);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 0);
    }
    blit[0] = (DrawFn)gPtrs[0x2e];
    _PlaySound(0xd4);
    i = 0;
    w = 0x78;
again:
    {
        if (i <= 3) {
            blit[0](ctx, base, 0, 0, w, w);
        } else if (i <= 7) {
            blit[0](ctx, base + (0xe1 << 6), 0, 0, w, w);
        } else if (i <= 0xb) {
            blit[0](ctx, gBuffer, 0, 0, w, w);
        } else if (i <= 0xf) {
            blit[0](ctx, ewram_2013840, 0, 0, w, w);
        }
        if (i >= 0x10 && i <= 0x13) {
            ClearFn clear = Func_80008d8;
            clear(ctx, 0x80 << 7, 0x3f3f3f3f);
        }
        if (i == 0x12) {
            _Func_80bd7dc(0x86);
        }
        if (i == 0x14) {
            *(int *)(base + 0x77a8) = 8;
            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 4);
        }
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        i++;
    }
    if (i != 0x15) {
        goto again;
    }
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
}
