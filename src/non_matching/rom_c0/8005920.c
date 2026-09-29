/* SomethingSaveHeader -- NON-MATCHING, 2 encodings of 156.  A TRUE DISTANCE --
 * same size (344), 156 encodings against 156, every relocation identical, no
 * shims.  The residue is two adjacent independent instructions swapped.
 * UNREACHABLE FROM SOURCE while dma.h's helpers are `static inline`; the
 * tree-wide macro screen the earlier park asked for is DONE and says NO.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/8005920.c \
 *     asm/rom_c0/rom_56cc_a_a_c_a_a.s --func SomethingSaveHeader
 * objcmp: "ENCODINGS differ in 2 place(s) (ref 156, ours 156), first at index 13".
 *
 * THE RESIDUE IS TWO ADJACENT INSTRUCTIONS IN THE WRONG ORDER:
 *     rom   mov r0, sp   /  add r1, r10
 *     ours  add r1, r10  /  mov r0, sp
 * `mov r0, sp` is DMA3_FILL's `_src = &value`, the first statement of the inline
 * body.  `add r1, r10` computes the `dst` argument, `base + 0x40`.
 *
 * THE FULL rank_for_schedule ACCOUNT (batch 295; the earlier park had priority
 * and LUID right and stopped one test short).  From the -dR -fsched-verbose=6
 * dump of basic block 0, insn 22 is the add and insn 28 the mov:
 *   - INSN_PRIORITY 4 == 4.  Structural: prio(22) = 1 + prio(asm) and
 *     prio(28) = 1 + max(prio(asm), prio(str)), and prio(str) == prio(asm)
 *     because the store's dependence on the volatile asm is charged 0.
 *   - INSN_REG_WEIGHT is skipped (`!reload_completed`).
 *   - same bb; the class-vs-last_scheduled_insn test is 3 == 3.
 *   - depend_count 2 == 2, AND THIS IS THE ONE THAT NEARLY PAYS.  insn 28's two
 *     dependents are the `str r3,[r0]` and the asm.  insn 22's two are the asm
 *     and INSN 447 = `mov r9, r1`, the copy that saves `base + 0x40` for the two
 *     later `DMA3_COPY(h, base + 0x40, 0x10)` calls.  If that reuse went away,
 *     the add would have ONE dependent and the mov would win outright -- PROVED
 *     by probe scratch_elev/b295/B/t3_probeDC.c, which puts
 *     `__asm__ volatile ("" : "+r" (base))` after the fill so the later
 *     `base + 0x40` is a different expression: `mov r0, sp` then moves AHEAD of
 *     the dst computation.  But THE ROM HAS `mov r9, r1` TOO, at index 16, so the
 *     ROM's add had the same two dependents and this route is closed.  (That
 *     probe is 130 differing overall -- it is a mechanism control, not a
 *     candidate.)
 * With all five tests tied it falls to INSN_LUID, and integrate.c:743/748 fixes
 * LUID: an inline argument is expanded with EXPAND_SUM and then UNCONDITIONALLY
 * `copy_to_mode_reg`'d before a single insn of the body is copied, so the add is
 * always emitted ahead of `_src = &value`.  A `static inline` helper therefore
 * CANNOT produce a body insn before an argument insn.  The ROM does.
 *
 * NINETEEN SPELLINGS MEASURED EARLIER, none moves it (all at 157 instructions):
 *   DMA3_CLEAR(base + 0x40, 0x1000)                              4 differing
 *   DMA3_CLEAR(&base[0x40], ...) / (void *)(base + 0x40) / ...   4
 *   DMA3_FILL(base + 0x40, 0, 0x1000)                            2
 *   DMA3_FILL with the fill value named (`z = 0;` first)         2  <- this file
 *   DMA3_FILL(&base[0x40], z, 0x1000)                            2
 *   the dst named into a local, assigned before the call         2
 *   the same with the value assigned first                       3
 *   `z = 0` hoisted above `base = iwram_3001f1c`                 4
 *   size spelled 0x400 * 4 / 0x400 << 2 / (0x10 << 2) offset     2 / 2 / 4
 *
 * ADDED IN BATCH 295 (delta to the inert list):
 *  - THE HELPER SWAP.  DMA3_SET/DMA3_COPY with a CALLER-SIDE zero word, so the
 *    `&value` becomes the FIRST argument (pre-copied before dst) instead of a
 *    body insn:  `unsigned int z; unsigned int *zp; zp = &z; *zp = 0;
 *    DMA3_SET(zp, base + 0x40, 0x85000000 | (0x1000 / 4));`  -> 10 differing.
 *    Same with `&z` passed directly -> 10, with DMA3_COPY -> 11.  It fails for a
 *    reason worth recording: the addressof pass turns the caller's `*zp = 0`
 *    back into `str r3, [sp, #0]`, so the ROM's `str r3, [r0]` is lost, AND
 *    `&z` needs no pre-copy insn at all (EXPAND_SUM yields `sp`), so the
 *    `mov r0, sp` is deferred into the body anyway and still lands after the add.
 *    The ROM's `mov r0,sp / str r3,[r0]` pair only comes out of a helper whose
 *    OWN local is addressed -- i.e. dma.h's DMA3_CLEAR/DMA3_FILL shape.
 *  - EVERY FLAG GROUP AND EVERY PAIR: 17 flags alone and in all 136 pairs (see
 *    src/rom_8a000/rom_8ba38_a_c_a_a_b.c (landed in batch 295) for the list).  Best is 2; nothing
 *    reaches 0.  -fno-schedule-insns2 is worse.
 *
 * ALSO REFUTED EARLIER, do not repeat: const-qualifying the parameter
 * (`void *const dst`) to satisfy integrate.c:748's `! TREE_READONLY (formal)`
 * test does NOT skip the pre-copy (probe scratch_elev/b294/A/t2_probe.c).
 *
 * THE dma.h MACRO DECISION IS ANSWERED: NO.  Rewriting DMA3_CLEAR's body as a
 * macro removes THIS site's residue completely, but the tree-wide screen (batch
 * 295, object level -- compile each landed user with the stock header and with a
 * macro header, assemble both, compare encodings + relocations + size, so label
 * renumbering is invisible) says the macro form CHANGES LANDED BYTES:
 *      DMA3_CLEAR  33 landed users, 20 change      (13 unchanged)
 *      DMA3_FILL    4 landed users,  1 changes     ( 3 unchanged)
 *      DMA3_COPY   53 landed users, 33 change      (20 unchanged)
 *      all three    82 landed users, 51 change     (31 unchanged)
 * Worst cases are not marginal: src/rom_c0/rom_56cc_a_c.c goes 164 -> 168 bytes
 * with 74 of 75 encodings differing, src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_c.c
 * 1004 -> 1008 with 421 of 463.  Every one of those files matches its tracked .s
 * today (spot-checked with `objcmp --whole`), so a changed encoding is a broken
 * file.  A macro form of any of the three is a per-site lever, not a header
 * change; if it is ever wanted it has to arrive as a SECOND, differently named
 * macro used only where the ROM's order demands it.  I did not touch include/.
 *
 * TWO REFERENCE-COMMENT CORRECTIONS, both in this .s's own banner:
 *   - it names the function "StartMusicTrack" and describes it as beginning a
 *     music track through Func_5810/5868/5b24/5b64. It is save-media work: it
 *     DMA-clears base+0x40, copies 0xff0 bytes of caller data to base+0x50,
 *     stamps an 8-byte header from .L79b8 with the slot id, a checksum-ish word
 *     and a wrapping counter, and writes it back.
 *   - it says "160 lines"; the function is 147 instructions.
 *
 * FOUR LEVERS GOT IT FROM 47 TO 2, all measured (tryc --align, of 157):
 *  1. A NAMED POINTER TO THE 16-BYTE HEADER BUFFER: 47 -> 28, assigned AFTER the
 *     DMA that fills the buffer, not at its declaration.
 *  2. THE HEADER BUFFER MUST BE DECLARED IN AN INNER BLOCK, so the inlined
 *     helper's `u32 value` takes sp+0 and the buffer sp+4.
 *  3. NAMING THE TWO COMPUTED INDICES of the final three stores: 28 -> 4, and it
 *     is what fixes the instruction COUNT.  TWO separate index locals: reusing
 *     one scores 21.
 *  4. `int one = 1; *(unsigned short *)(h + 0xa) = one;` -- the HImode-literal
 *     lever.  Func_80056cc, nine instructions away in the same file's neighbour,
 *     wants the bare literal for the same kind of store.  Same bank, opposite
 *     answers.
 *
 * ONE PROTOTYPE NOTE for whoever lands this: the ROM sets r0 before BOTH
 * `bl Func_8005b24` and `bl Func_8005810`, so at this call site they take an
 * argument -- but src/rom_c0/rom_56cc_a_a_a_b.c defines `int Func_8005810(void)`
 * and src/rom_c0/rom_56cc_a_a_c_a_b.c declares `Func_8005b24(void)`. That is
 * consistent, not a conflict: pre-prototype C calls them with an argument the
 * callee ignores, and each TU carries its own extern. Declaring them `(void)`
 * here drops the two `mov r0, r8` and loses the match.
 *
 * SHIMS: none in this file.  No `register ... __asm__` declarations and no
 * `__asm__(".equ ...)` lines.  The one `__asm__` is the asm-label on the
 * `.L79b8` declaration, the tree's standard spelling for a dot-label symbol, and
 * `.L79b8` is already `.global` in asm/rom_c0/rom_56cc_c_c_b.s so the reference
 * needs no new export.  (The four `register ... __asm__` pins that reach the
 * object come from include/dma.h, a landed header, not from this file.)
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern const unsigned char _TBL_79b8[] __asm__(".L79b8");
extern unsigned int Func_8005b24(int id);
extern unsigned int Func_8005810(int id);
extern int Func_8005ae0(void);
extern int Func_8005c2c(int id);
extern int Func_8005868(unsigned int n);
extern int Func_8005b64(unsigned int n);

int SomethingSaveHeader(int id, const void *src)
{
    unsigned char *base;
    unsigned int a;
    unsigned int b;
    unsigned int z;
    unsigned int k1;
    unsigned int k2;
    vu32 *dma;

    base = iwram_3001f1c;
    z = 0;
    DMA3_FILL(base + 0x40, z, 0x1000);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    a = Func_8005b24(id);
    b = Func_8005810(id);
    if (b > 0xf)
        return 1;
    DMA3_COPY(src, base + 0x50, 0xff0);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    {
        unsigned char hdr[16];
        unsigned char *h;

        DMA3_COPY(_TBL_79b8, hdr, 8);
        dma = (vu32 *)&REG_DMA3SAD;
        while (dma[2] & 0x80000000)
            ;
        h = hdr;
        h[7] = id;
        *(unsigned short *)(h + 8) = Func_8005ae0();
        *(unsigned short *)(h + 0xa) = Func_8005c2c(id) + 1;
        DMA3_COPY(h, base + 0x40, 0x10);
        dma = (vu32 *)&REG_DMA3SAD;
        while (dma[2] & 0x80000000)
            ;
        if (Func_8005868(b) != 0)
            return 1;
        if (a <= 0xf && Func_8005b64(a) != 0)
            return 1;
        if (*(unsigned short *)(h + 0xa) > 0xfde8) {
            int one = 1;
            *(unsigned short *)(h + 0xa) = one;
            DMA3_COPY(h, base + 0x40, 0x10);
            dma = (vu32 *)&REG_DMA3SAD;
            while (dma[2] & 0x80000000)
                ;
            if (Func_8005868(a) != 0)
                return 1;
            if (Func_8005b64(b) != 0)
                return 1;
            b = a;
        }
        base[b] = 1;
        k1 = b + 0x10;
        base[k1] = id;
        k2 = b * 2 + 0x20;
        *(unsigned short *)(base + k2) = *(unsigned short *)(h + 0xa);
    }
    return 0;
}
