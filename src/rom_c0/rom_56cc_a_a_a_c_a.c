/* Func_8005868 @ 0x08005868  --  MATCHING
 *
 * BYTE-IDENTICAL: 68 bytes, 30 encodings and 4 relocations identical.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c0/rom_56cc_a_a_a_c_a.c \
 *     asm/rom_c0/rom_56cc_a_a_a_c_a.s --func Func_8005868
 *
 * Source asm: asm/rom_c0/rom_56cc_a_a_a_c_a.s
 * NO SPLIT NEEDED -- the piece holds this function alone, datacheck.py reports
 * no data section, and stage1.ld:61 already lists the object.
 * PINS: zero.  No shims, no .equ, no flag group.
 *
 * A flash-write-and-verify: call the writer through a function pointer held in
 * ewram_2004c04, and if it reports failure fall back to VerifyFlashSector and
 * return whether that found anything.
 *
 * TWO LEVERS CLOSED THE PARK'S TWO RESIDUES, AND THE PARK HAD BOTH VERDICTS
 * WRONG WHILE HAVING BOTH OBSERVATIONS RIGHT.
 *
 * ONE -- the park wrote `buf = (u8 *)iwram_3001f1c; buf += 0x40;`, a two-address
 * update of ONE pseudo, and explained the ROM's extra `mov r6, r3` as staging
 * through r3 because r3 was wanted again for the second pool load.  That is
 * refuted by the park's own output, which reuses r3 for the second pool load
 * and still loads straight into the callee-saved register.
 *
 * The real mechanism is Thumb-1's 3-bit `add Rd,Rn,#imm` (*thumb_addsi3,
 * config/arm/arm.md:496): it cannot encode 0x40, so `(set q (plus p 64))` with
 * q distinct from p has no alternative and reload inserts the copy.  Writing
 * the offset as an EXPRESSION AT THE USE SITE -- `base + 0x40`, CSE-unified
 * between the two calls -- is what makes q distinct from p.  A `buf` local
 * holding it does not: it also fixes the pool-address pseudo's live range,
 * which is the second half of this (see below).
 *
 * TWO -- the park asked "what makes gcc-2.96 materialise a boolean instead of
 * branching when the other arm is a constant".  The other arm has nothing to do
 * with it.  `return r != 0;` is intercepted by expr.c:7720-7738, whose own
 * comment reads "For foo != 0, load foo, and if it is nonzero load 1 instead",
 * and which emits exactly the park's `cmp r0,#0 / beq / mov r0,#1`.  It fires
 * whenever the code is NE_EXPR against zero and `original_target` is a REG of
 * the compared operand's mode -- a returned `s32` always is.
 *
 * That path is reached because do_store_flag returned 0 first: expr.c:10329-10350
 * wants an scc pattern or abs_optab/ffs_optab for the operand mode, and in Thumb
 * `sne` (arm.md:5581) and `abssi2` (arm.md:2663) are both TARGET_ARM, so
 * genopinit.c:231's `if (HAVE_...)` guard leaves both optabs empty.  SO IN THUMB
 * A COMPARISON CAN NEVER BE MATERIALISED BY THE COMPARISON EXPANDER AT ALL --
 * worth knowing generally, because it means every `x != 0` written for value
 * comes out branchy unless something else converts it.
 *
 * The ROM's `mov r3,r0 / neg r0,r3 / orr r0,r3 / lsr r0,#31` is
 * expmed.c:4500-4508 (neg_optab then ior_optab) plus the normalising shift at
 * :4513-4515, and the only caller that can reach it here is ifcvt.c:530
 * (noce_emit_store_flag) from noce_try_store_flag (ifcvt.c:544-556), which
 * requires the two arms to be the CONSTANTS 0 and 1.  So the source has to hand
 * gcc a block that already stores both -- `ok = 0; if (r != 0) ok = 1;` -- which
 * misses expr.c:7720 entirely, takes the TRUTH_ANDIF fallback at
 * expr.c:7745-7765 (emit_clr_insn / jumpifnot / emit_0_to_1_insn), and lets
 * if_convert (toplev.c:2877, :3238) fold it.  The separate `r` local matters:
 * without it the result is one instruction too MANY.
 *
 * THE LAST SIX ENCODINGS WERE PURE REGISTER ALLOCATION, and the lever is
 * local-alloc.c:1496's QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size /
 * (death - birth) -- the SHORTER live range wins -- crossed with REG_ALLOC_ORDER
 * (config/arm/arm.h:989), which hands out r3 first and r2 second.  The ROM's
 * `ldr r3,[r3]` plus `ldr r2,=ewram_2004c04` says the base pseudo was allocated
 * FIRST (so r3) and the pool-address pseudo's range OVERLAPS it (so r2).  With
 * `fp = ewram_2004c04;` as its own statement the address load and the memory
 * load are adjacent, the address pseudo has the shorter range, it takes r3, base
 * is pushed to r2, and sched2 then cannot hoist the address load past
 * `mov r6,r3` because of the anti-dependence on r3.  Calling the global
 * directly puts both at the call site and the ordering falls out.
 *
 * Three further spellings were measured identical and are NOT needed:
 * `pp = &ewram_2004c04; (*pp)(...)`, the same with `pp[0](...)`, and an
 * array-typed `extern u16 (*ewram_2004c04[])(...)` called as `[0]`.  The direct
 * call is the plain one and is what ships.
 *
 * MEASURED INERT on the final boolean, all leaving the function short:
 * `return !!r`, `return r ? 1 : 0`, and `u32 r; return r > 0` (fold rewrites an
 * unsigned `> 0` to `!= 0`, so it is the same program and the same code).
 */
#include "gba/types.h"

extern u32 iwram_3001f1c;
extern u16 (*ewram_2004c04)(u16 sector, void *buf);
extern s32 VerifyFlashSector(u16 sector, void *buf);

s32 Func_8005868(u16 sector)
{
    u8 *base;
    s32 r;
    s32 ok;

    base = (u8 *)iwram_3001f1c;
    if (ewram_2004c04(sector, base + 0x40) != 0)
        return 1;
    r = VerifyFlashSector(sector, base + 0x40);
    ok = 0;
    if (r != 0)
        ok = 1;
    return ok;
}
