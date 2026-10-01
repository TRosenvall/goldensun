/* Func_80f0678 -- 0x080f0678, the credits initialiser.  *** EXACT *** (batch 316).
 *
 *   OK Func_80f0678 -- 376 bytes, 165 encodings and 11 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_f0000/rom_f0254_c_c_b.c \
 *     asm/rom_f0000/rom_f0254_c_c_b.s --func Func_80f0678
 * (before the split, against the tracked multi-function reference:
 *     ... tools/objcmp.py <this file> asm/rom_f0000/rom_f0254_c_c.s \
 *       --func Func_80f0678 )
 *
 * ============================ LANDING REQUIREMENTS ========================
 *
 * TEXT/DATA SPLIT.  asm/rom_f0000/rom_f0254_c_c.s holds three functions
 * (Func_80f0614, Func_80f0678, Func_80f07f0) AND a `.rodata` section, so the
 * data must keep its own object.  `tools/split_s.py asm/rom_f0000/rom_f0254_c_c.s
 * Func_80f0678 --dry-run` REFUSES until the export is added:
 *
 *     REFUSING to split ...: 1 local label(s) would cross files.
 *       _a.s references .Lf1220, defined in _c.s
 *       _b.s references .Lf1220, defined in _c.s
 *
 * EXPORT LIST: exactly ONE label, `.global .Lf1220`, added to the residual
 * `.s`.  (Func_80f0614, which stays in assembly as _a, reads it too; one
 * `.global` covers both.)  The file currently exports only `.Lf0a5c`.
 *
 * *** tools/datacheck.py OVER-EXPORTS HERE, AND THE PARK'S DIAGNOSIS OF THE BUG
 * *** IS CORRECT -- RE-VERIFIED IN BATCH 316.  datacheck prints
 *     Func_80f0678  reads .Lf1220
 *     *** SPLIT MUST EXPORT: .global .Lf1220
 * for THIS function (so for once it agrees), but it attributes .Lf11bd and
 * .Lf1770 to Func_80f0678 in other runs because split_requirements ends each
 * function's range at the NEXT `.thumb_func_start`, swallowing the following
 * function's `@` comment block, and LABEL_REF = re.compile(r"\.L\w+") matches
 * inside comments.  Lines 242-243 of this .s are
 *     @     .Lf1770   the glyph bitmaps, 8 bytes each, 8x8 and 1bpp
 *     @     .Lf11bd   the advance width per glyph, one byte each
 * and the real references to both are inside Func_80f07f0 (lines 329, 375,
 * 422).  Two-line fix: end the range at `.func_end` (this file HAS them -- 3)
 * and strip from an unquoted `@` to end-of-line before matching.  Measured
 * again here: with comments stripped, Func_80f0678's body references exactly
 * `.Lf1220` and nothing else from .rodata.
 *
 * SHIMS: ONE, the `__asm__ ("" : "+r" (sp0))` barrier (tools/shimcount.py says
 * 1 `"+r"` barrier, classed as a fakematch) -- A fakematch.txt ROW IS REQUIRED.
 * Plus the SIX register pins inside include/dma.h's DMA3_SET, used twice here;
 * those are the tree's existing helper, not this file's, but a shim count read
 * only from this file understates what the object contains.
 *
 * ============ HOW THE LAST 6 FELL: TWO EDITS, EACH INERT OR WORSE ==========
 *
 * The park stood at 6 of 165 and closed itself "UNREACHABLE-BY-CONSTRUCTION AT
 * THE SOURCE LEVEL".  All six were three adjacent transpositions, all one
 * decision -- the ROM schedules the control word's giv chain ahead of the
 * offset's, in the inner preheader AND at the increment:
 *
 *     [76/77]  ref  movs r0,#0xc0 ; adds r3,r7,r6   | ours the reverse
 *     [80/81]  ref  lsls r0,#13   ; lsls r1,r3,#3   | ours the reverse
 *     [91/92]  ref  add  r0,r8    ; adds r1,#4      | ours the reverse
 *
 * THE DIAGNOSIS WAS RIGHT ABOUT THE PASS AND WRONG ABOUT REACHABILITY.
 * Measured from the dependence table (-fsched-verbose=6, block 7):
 *
 *     ;;  insn code bb dep prio cost  ...  : <INSN_DEPEND>
 *     ;;   566    5  0   0    2    1  ... core : 570      <- adds r3,r7,r6
 *     ;;   698  173  0   0    2    1  ... core : 699      <- movs r0,#0xc0
 *     ;;	Ready list (t = 0):  602 224 698 566  -> picks 566
 *
 * PRIORITY TIES AT 2, DEPENDENT COUNT TIES AT 1, and this is the block's FIRST
 * decision so the CLASS rung is skipped (last_scheduled_insn == 0,
 * haifa-sched.c:5963).  So rank_for_schedule falls to INSN_LUID, and LUID is the
 * order loop.c's strength_reduce emitted the two giv initialisations in -- which
 * is REVERSE SCAN ORDER, because record_giv PREPENDS to bl->giv.  The park's
 * mechanism, confirmed.
 *
 * THE PARK THEN CONCLUDED that reaching the ROM's order needs the offset
 * expression scanned first, that every spelling which names it reclassifies
 * 0x180000 from an inner-loop giv base into an outer-loop invariant, and that
 * the two properties are "COUPLED".  THEY ARE NOT.  THE FIX IS TO NAME BOTH:
 *
 *     u32 n = j * 0x18 + k * 4;                       <- offset, scanned FIRST
 *     u32 c = (0xc0 << 13) + k * (0x80 << 14);        <- control giv, named
 *     u32 *q = p;
 *     *q++ = (0x10 + j * 8) | c | 0x40004000;
 *     *q = n;
 *
 * EACH HALF ALONE IS A DEAD END AND THE PARK HAD MEASURED BOTH HALVES:
 *
 *     name the offset only (`u32 n` first, control word inline)   105 of 165, +4 bytes
 *     name the control giv only (`u32 c`, offset inline and last)   6 of 165  INERT
 *     NAME BOTH                                                     *** 0 ***
 *
 * This is the brief's "two edits, each individually inert or a clear
 * regression, jointly the whole residue" shape, and it is why one-at-a-time
 * testing could never find it.  Naming the offset gets it scanned first (which
 * is what flips the giv list) but lets gcc lift 0x180000 out of the nest;
 * naming the control giv as its OWN pseudo is what keeps 0x180000 an
 * inner-loop giv base while being scanned second.  The park had the second
 * edit in its own MEASURED-INERT list ("`((0xc0 << 13) + k * (0x80 << 14))`
 * hoisted into its own named local") and the first in its REJECTED list, and
 * never crossed them.
 *
 * *** AND THE PARK'S "BIGGEST FINDING IN THE FUNCTION" IS NOW SUPERSEDED. ***
 * Lever 3 was a shared `u32 m = j * 2` used in BOTH terms, to raise `2*j`'s
 * n_refs so global.c's allocno_compare would not push it into r14; it was worth
 * 19 of 165 and three non-sharing spellings of the same algebra all measured 23.
 * With the two givs named, `m` IS NO LONGER NEEDED: `j * 0x18` and `0x10 + j*8`
 * written with no shared subexpression at all are byte-identical.  So the
 * allocation that lever 3 was buying is a CONSEQUENCE of the giv list order,
 * not an independent fact.  `j * 24` and `(j*2 + j) * 8` are also exact, and
 * `k << 2` / `k << 21` for the multiplies are exact.  The function is three
 * locals shorter than the park.
 *
 * ================== THE OTHER LOAD-BEARING CONSTRUCTS ==================
 * Each figure is that construct reverted ALONE from this file; every one keeps
 * size and count exact, so they are all true distances.
 *
 * 1. `unsigned int` LOOP COUNTERS -- 24 of 165 as `int`.  Every loop test in the
 *    ROM is `bls`, i.e. UNSIGNED, including `cmp r6,#7` where a signed counter
 *    gives `ble`.
 * 2. BLOCK-SCOPE `u32 *q` (and `u32 t` where used) INSIDE EACH LOOP BODY -- the
 *    ROM wants a FRESH pseudo per loop; one shared pair at function scope puts
 *    the value and the pointer in each other's registers.
 * 3. A BLOCK-SCOPE `int pr` FOR THE FIRST StartTask PRIORITY -- 2 of 165
 *    without it.  `StartTask(Func_80f0538, 0x90 << 3)`: the function pointer is
 *    a pool load, so precompute_register_parameters (calls.c:850) copies it to a
 *    pseudo first and sched2 then puts `ldr r0,=Func_80f0538` BEFORE the
 *    priority's `lsl r1,#3`, where the ROM has it after.  A local at the TOP of
 *    the function does NOT work; a local in a BLOCK around the call does.  The
 *    SECOND StartTask needs nothing.
 * 4. THE LOCAL POINTER `sp0` AND ITS `"+r"` BARRIER -- 2 of 165 with the barrier
 *    dropped.  The ROM materialises `&v` into a register once (`mov r4,sp`) and
 *    stores through it, then copies it to DMA3_SET's pinned r0.  `u32 *sp0 = &v;`
 *    alone does not survive: cse rewrites the pseudo to `sp`, because
 *    `str r3,[sp,#0]` is a legal Thumb encoding.  The barrier is the only thing
 *    found that keeps it.  REJECTED, worse: `u32 v[1]` with `sp0 = v`; and
 *    include/dma.h's DMA3_CLEAR + DMA3_FILL instead of two DMA3_SET calls.
 * 5. `i * 0x18` WRITTEN INLINE IN THE LAST LOOP, not as an accumulator.
 *
 * ================= A FINDING FOR include/dma.h's OPEN DECISION =============
 * Batch 294 left "include/dma.h's helpers as macros rather than static inline"
 * as an owner decision.  THIS FUNCTION IS A SECOND, INDEPENDENT ARGUMENT FOR IT.
 * Each of DMA3_CLEAR and DMA3_FILL declares its OWN `u32 value` local; using
 * them for this function's two transfers gives `sub sp,#0x8` -- gcc-2.96 gives
 * the two inlined locals SEPARATE STACK SLOTS -- against the ROM's `sub sp,#0x4`.
 * So a function doing two DMA3 transfers through one scratch word cannot be
 * written with those two helpers at all; it needs DMA3_SET, which takes the
 * source as a parameter.  As MACROS sharing one caller-declared word, both
 * forms would be available.
 */
#include "dma.h"

extern void *Func_8004970(int size);
extern void StartTask(void *task, int priority);
extern void Func_80f0538(void);
extern void Func_80f0614(void);
extern int Func_80f07f0(char *s, int row, int align);

extern void *ewram_2004c0c;
extern unsigned short ewram_2004c00;
extern unsigned short ewram_2004c04;
extern unsigned short ewram_2004c08;
extern char *Lf1220[] __asm__(".Lf1220");

void Func_80f0678(void)
{
    u32 v;
    u32 *p;
    u32 *sp0;
    unsigned int i;
    unsigned int j;
    unsigned int k;

    ewram_2004c0c = Func_8004970(0x80 << 3);
    sp0 = &v;
    __asm__ ("" : "+r" (sp0));
    *sp0 = 0;
    DMA3_SET(sp0, (void *)0x6010000, 0x85001800);
    *sp0 = 0x11111111;
    DMA3_SET(sp0, (void *)0x6016000, 0x85000040);
    p = (u32 *)ewram_2004c0c;
    for (i = 0; i <= 7; i++) {
        u32 t = (i << 21) | 0x80004000;
        u32 *q = p;
        *q++ = t;
        *q = 0xc0 << 2;
        p += 2;
    }
    for (i = 0; i <= 7; i++) {
        u32 t = (i << 21) | 0x80004088;
        u32 *q = p;
        *q++ = t;
        *q = 0xc0 << 2;
        p += 2;
    }
    for (i = 0; i <= 7; i++) {
        u32 t = (i << 21) | 0x40004098;
        u32 *q = p;
        *q++ = t;
        *q = 0xc0 << 2;
        p += 2;
    }
    for (j = 0; j <= 0xf; j++) {
        for (k = 0; k <= 5; k++) {
            u32 n = j * 0x18 + k * 4;
            u32 c = (0xc0 << 13) + k * (0x80 << 14);
            u32 *q = p;
            *q++ = (0x10 + j * 8) | c | 0x40004000;
            *q = n;
            p += 2;
        }
    }
    for (i = 0; i <= 7; i++) {
        u32 t = 0xc000c0;
        u32 *q = p;
        *q++ = t;
        *q = 0xc0 << 2;
        p += 2;
    }
    ewram_2004c00 = 0;
    ewram_2004c08 = 0;
    ewram_2004c04 = 0;
    { int pr = 0x90 << 3; StartTask(Func_80f0538, pr); }
    StartTask(Func_80f0614, 0xc8 << 4);
    for (i = 0; i <= 0x1f; i++)
        Func_80f07f0(Lf1220[0], i * 0x18, 1);
}
