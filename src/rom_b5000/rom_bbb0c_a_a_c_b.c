/* Func_80bd7a4 (ResetDmaAndDispatch) -- MATCHING, 0 of 25 encodings, PIN-FREE.
 *
 * *** PREREQUISITE: `DMA3_SET_RW` MUST BE PROMOTED TO include/dma.h FIRST. ***
 * This file deliberately does NOT carry a local copy.  docs/elevation.md,
 * "Do not land a function carrying a LOCAL copy of a dma.h-shaped helper",
 * and src/non_matching/rom_b5000/80c02a4.c:37 ("promote `DMA3_SET_RW` to
 * dma.h when it lands") both prescribe promote-first.  The text to add to
 * include/dma.h is VERBATIM 80c02a4.c:185-197 -- no edit, no re-spelling:
 *
 *     static inline void DMA3_SET_RW(const void *src, void *dst, u32 cnt) {
 *         register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
 *         register const void *_src  __asm__("r0") = src;
 *         register void *_dst  __asm__("r1") = dst;
 *         register u32 _cnt  __asm__("r2") = cnt;
 *         __asm__ volatile (
 *             "stmia\tr3!, {r0, r1, r2}\n\t"
 *             "sub\tr3, #0xc"
 *             : "+r" (_cnt)
 *             : "r" (_base), "r" (_src), "r" (_dst)
 *             : "memory", "r0"
 *         );
 *     }
 *
 * `Func_80bd7a4` is the SECOND independent function to need the form and the
 * first to land on it -- the same evidence standard `DMA3_COPY16_RW` was
 * promoted on in batch 299.  Measured: `"+r" (_cnt)` and `"+l" (_cnt)` BOTH
 * reach 0, so there is no reason to deviate from the existing text.
 *
 * PINS: 0.  (tools/shimcount.py reports 0 for this file.  The four
 * `register ... __asm__` declarations live in include/dma.h, exactly as they
 * do for the 80-odd landed DMA3 users, none of which carries a fakematch row.)
 *
 * SPLIT: asm/rom_b5000/rom_bbb0c_a_a_c.s holds THREE functions and this is the
 * last.  tools/split_s.py --dry-run:
 *     would write asm/rom_b5000/rom_bbb0c_a_a_c_a.s  (2 function(s), 511 lines)
 *     would write asm/rom_b5000/rom_bbb0c_a_a_c_b.s  (1 function(s), 30 lines)
 *     would REMOVE asm/rom_b5000/rom_bbb0c_a_a_c.s
 *     would rewrite stage1.ld
 * tools/datacheck.py on the source .s is SILENT (exit 0): no data requirement.
 * The only .word list in the file is Func_80bd424's 7-entry jump table at
 * lines 114-120, which stays in the _a half, and stage1.ld names the object
 * ONCE (line 1654, `.text`) -- no `.rodata` line to re-home.  exports: [].
 *
 * REPOINT: the two siblings that stay in the _a half BOTH have parks whose
 * recipes name the pre-split path -- src/non_matching/rom_b5000/80bd3e4.c and
 * src/non_matching/rom_b5000/80bd424.c.  Run tools/repoint_parks.py after the
 * split or both recipes silently stop working.
 *
 * Verify with (INSTALLED path, after the split and the dma.h promotion):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_bbb0c_a_a_c_b.c \
 *     asm/rom_b5000/rom_bbb0c_a_a_c_b.s --func Func_80bd7a4
 *
 * Verify TODAY, before either (models the promotion with -include; the body is
 * byte-for-byte the one above):
 *   docker run --rm --security-opt seccomp=unconfined \
 *     -e OBJCMP_EXTRA="-include scratch_elev/b323/C/dma_rw_promote.h" \
 *     -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b323/C/p1_candidate.c \
 *     asm/rom_b5000/rom_bbb0c_a_a_c.s --func Func_80bd7a4
 *   -> OK Func_80bd7a4 -- 56 bytes, 25 encodings and 2 relocations identical
 *
 * THE RESIDUE WAS TWO CAUSES, one masking the other.  The park read 18 of 25
 * at ref 25 / ours 21 with `_call_via_r0` against `_call_via_r3`.
 *
 * (A) THE DMA COUNT.  The ROM re-issues `mov r2,#0x84 / lsl r2,#24` before
 *     EVERY transfer; stock DMA3_SET promises the count survives, so gcc sets
 *     r2 once and is four instructions short.  `"+r" (_cnt)` as an inline-asm
 *     OUTPUT withdraws that promise.  18 -> 1, count 21 -> 25, size 48 -> 56.
 *     (r0 needed nothing: dma.h's DMA3_SET already clobbers it, which is why
 *     `mov r0,#0` was already rebuilt per transfer.)
 *
 * (B) THE INTERWORKING VENEER.  The park called `fp()` through a named local
 *     and said "that part is right".  It is not: gcc's interworking expander
 *     calls through whatever hard register the pointer lands in -- r3, the one
 *     the helper has just freed -- so it emits `bl _call_via_r3`.  The ROM
 *     wants r0.  The fix is a DECLARATION, and it is a landed idiom already in
 *     the tree: src/rom_8a000/rom_8ba38_c_b.c (`InitMap`) declares
 *     `extern void _call_via_r0(void (*)(void));` and calls it, which makes the
 *     pointer argument 0 and puts it in r0 by the ABI.  Its generated
 *     asm/rom_8a000/rom_8ba38_c_b.s carries exactly
 *     `ldr r3,.L3 / ldr r0,[r3,#4] / bl _call_via_r0`.  NOT a device: the
 *     symbol is real and .global in src/lib/call_via.s.  1 -> 0.
 *
 *     The veneer NAME is a direct readout of the allocation, which makes it a
 *     cheap diagnostic rather than only a failure: assigning `fp` BEFORE the
 *     three transfers makes it live across three `volatile` asms, it takes a
 *     callee-saved register, and the call becomes `bl _call_via_r4`.
 */
#include "dma.h"

extern void (*iwram_30000c4)(void);

void Func_80bd7a4(void)
{
    extern void _call_via_r0(void (*)(void));

    DMA3_SET_RW((void *)0, (void *)0, 0x84000000);
    DMA3_SET_RW((void *)0, (void *)0, 0x84000000);
    DMA3_SET_RW((void *)0, (void *)0, 0x84000000);
    _call_via_r0(iwram_30000c4);
}
