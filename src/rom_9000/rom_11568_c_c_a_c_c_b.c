/* Func_8011a84 -- 0x08011a84  (asm/rom_9000/rom_11568_c_c_a_c_c.s)
 *
 * MATCHING.  0 of 40 encodings, 92 bytes, 3 relocations identical.  NO PINS in
 * the function body.  (MEASURED, batch 323 brief J.  Park was 7 of 40.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_11568_c_c_a_c_c_b.c \
 *     asm/rom_9000/rom_11568_c_c_a_c_c_b.s --func Func_8011a84
 *
 * Pre-split, against the whole reference this was verified as
 *   objcmp.py <this file> asm/rom_9000/rom_11568_c_c_a_c_c.s --func Func_8011a84
 *     -> OK Func_8011a84 -- 92 bytes, 40 encodings and 3 relocations identical
 * and --whole against the extracted single-function .s
 *   -> OK whole file -- 92 bytes, 40 encodings and 3 relocations identical
 *
 * SPLIT SHAPE:
 *   datacheck.py asm/rom_9000/rom_11568_c_c_a_c_c.s  -- CLEAN (rc 0, no data
 *     section, no exports needed)
 *   split_s.py --dry-run asm/rom_9000/rom_11568_c_c_a_c_c.s Func_8011a84 ->
 *     rom_11568_c_c_a_c_c_a.s  (Func_80119cc, 102 lines)   -- stays as asm
 *     rom_11568_c_c_a_c_c_b.s  (Func_8011a84,  43 lines)   -- replaced by this
 *   Func_80119cc remains a park (src/non_matching/rom_9000/80119cc.c).
 *
 * PINS: 0 in Func_8011a84.  The three `register ... __asm__()` declarations are
 * inside the file-local `static inline DMA3_CLEAR_OFS`, which is the same shape
 * as include/dma.h's DMA3_CLEAR / DMA3_FILL / DMA3_FILL_OFS -- legal "l" input
 * constraints plus a "memory" clobber, no illegal clobber list.  The 82 landed
 * users of those helpers carry NO fakematch row, and a grep of fakematch.txt
 * confirms none of them is listed.  The two landed files that DO define a local
 * DMA helper and ARE fakematch rows
 * (src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_c.c and
 *  src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_c.c) define `DMA3_SET_R2CLOB`,
 * whose row is bought by its ILLEGAL r2 clobber, not by the register
 * declarations.  So this entry is fakematch: false.  Flagging it anyway because
 * it is the coordinator's call, not mine.
 *
 * WHAT THE PARK GOT WRONG.  Its diagnosis was "prologue instruction
 * scheduling ... all seven are the SAME instructions in a different order", and
 * it recorded `a named int *q = &buf; used for the store and the DMA source` as
 * INERT at 7.  Both halves are refuted:
 *
 *   - The residue was never purely order.  One of the seven was the store's
 *     BASE REGISTER.  The ROM does `mov r0, sp` and then `str r6, [r0]`
 *     (0x6006); any `int buf; ... &buf` gives RTL `(mem (reg 13 sp))` and
 *     therefore `str r6, [sp]` (0x9600) -- a different encoding that no amount
 *     of statement reordering can reach, because sp is a hard register and
 *     cse/combine fold a pseudo holding `&buf` straight back to it.
 *   - Which is exactly why `int *q = &buf` measured inert: the naming changed
 *     nothing about the mem.  Only a HARD-REGISTER variable keeps the address
 *     in a register, which is precisely what dma.h's DMA3_CLEAR already does
 *     (`register u32 *_src __asm__("r0") = (&value); *_src = 0;`).
 *
 * THE LADDER, every rung measured with tools/sweep_variants.py (ref 40
 * encodings throughout, so these are distances and not misalignment):
 *
 *   installed park: DMA3_SET + a caller-owned `int buf`           7
 *   DMA3_SET -> DMA3_CLEAR(t, 12), `t` assigned before `go = 0`   3
 *   DMA3_SET -> DMA3_CLEAR(t, 12), `go = 0` before `t`            2
 *   the same with `t = (char *)(iwram_3001e70 + 0xd8)`            2
 *   the same with `DMA3_CLEAR(t, 0xc)` and a goto tail            2
 *   DMA3_CLEAR_OFS(base, 0xd8, 12) returning the dst              0   <-- this
 *   DMA3_CLEAR's body hand-inlined in the function (DEVICE)        0
 *   DMA3_CLEAR called before `t` exists, `t` recomputed after     34  (RELOCDIFF;
 *       the asm's "memory" clobber reloads iwram_3001e70)
 *   DMA3_FILL(t, go, 12)                                           3
 *
 * WHY THE LAST TWO ENCODINGS NEEDED THE _OFS SHAPE.  At 2 the only differences
 * are indices 7 and 8, `mov r0, sp` and `add r4, #0xd8` swapped.  Read out of
 * -da -fsched-verbose=6 (`*.23.sched2`), these are insn 26 and insn 19, both
 * priority 5, both with three dependents, in the same basic block, with the
 * same class relative to the last-scheduled insn -- so haifa-sched.c's
 * rank_for_schedule falls all the way through to its final line,
 * `return INSN_LUID (tmp) - INSN_LUID (tmp2)`, and the LOWER LUID wins.  The
 * `add` is the caller's `t = base + 0xd8`; `mov r0, sp` is the first insn of
 * the inlined DMA body.  A call's arguments are evaluated BEFORE the inline
 * body is spliced in, so while `t` is computed at the call site the add can
 * never have the higher LUID -- the axis is closed to statement order.
 *
 * Passing the OFFSET as its own argument moves the add INSIDE the inline body,
 * after `_src = &value`, which flips the LUID pair and the sched2 tie with it.
 * That is the identical mechanism include/dma.h's DMA3_FILL_OFS was added for
 * ("the `add` of the base moves into the body after `mov r0,sp`"); the only
 * thing added here is RETURNING the computed destination, so the caller's `t`
 * is the same pseudo and no second add appears.
 *
 * FOLLOW-UP FOR THE OWNER, not decided here: DMA3_CLEAR_OFS belongs in
 * include/dma.h beside DMA3_FILL_OFS under the same standing instruction
 * ("promote it if a second function needs it").  It is file-local here so this
 * candidate installs as one file with no tree-wide header change.
 */
#include "dma.h"

extern int iwram_3001e70;
extern void Func_80119cc(void);
extern void StartTask(void *f, int pri);

void Func_8011a84(unsigned short *p)
{
    char *t;
    int go;

    go = 0;
    t = DMA3_CLEAR_OFS((void *)iwram_3001e70, 0xd8, 12);
    if (*p != 0xffff) {
        *(unsigned short **)t = p;
        *(unsigned short **)(t + 4) = p;
        *(unsigned short *)(t + 8) = go;
        *(unsigned short *)(t + 0xa) = go;
        go = 1;
    }
    if (go != 0)
        StartTask(Func_80119cc, 0xc8 << 4);
}
