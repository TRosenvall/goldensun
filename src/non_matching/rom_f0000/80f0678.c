/* Func_80f0678  --  0x080f0678, 146 instructions.  NON-MATCHING, PARKED AT
 * 6 ENCODINGS OF 165, and this IS a true distance: SIZE EXACT (376 = 376),
 * INSTRUCTION COUNT EXACT (165 = 165), every relocation identical.
 *
 * BOTH MEASURES, as the brief asks:
 *   objcmp   XX ENCODINGS differ in 6 place(s) (ref 165, ours 165)
 *   tryc --align   6 instruction(s) in disagreeing regions, of 153
 * (tryc says 154 lines against 153 because the `__asm__` barrier makes gcc
 * re-emit a `.code 16` directive that tryc counts as a line; it is zero bytes
 * and objcmp's count is 165 = 165.  The two tools agree in sign here.)
 *
 * NOT a flag-conditional figure: measured under the Makefile's own
 * GCC296_CFLAGS with no flag added.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py scratch_elev/b295/A/t1_park.c \
 *     asm/rom_f0000/rom_f0254_c_c.s --func Func_80f0678
 *
 * ============================== SPLIT SHAPE ==============================
 *
 * THREE functions in asm/rom_f0000/rom_f0254_c_c.s (Func_80f0614,
 * Func_80f0678, Func_80f07f0) AND a `.rodata` section, so this needs a
 * TEXT/DATA SPLIT: the data keeps its own object.
 *
 * EXPORT LIST: exactly ONE label -- `.global .Lf1220`, which is NOT currently
 * global (the file only exports `.Lf0a5c`).
 *
 * *** THIS DISAGREES WITH tools/datacheck.py AND datacheck IS WRONG. ***
 * It prints
 *     Func_80f0678  reads .Lf11bd, .Lf1220, .Lf1770
 *     *** SPLIT MUST EXPORT: .global .Lf11bd .global .Lf1220 .global .Lf1770
 * but Func_80f0678's body references ONLY `.Lf1220` (one `ldr r7, =.Lf1220`).
 * `.Lf11bd` and `.Lf1770` are the glyph tables read by Func_80f07f0.
 *
 * THE BUG, in tools/datacheck.py's split_requirements:
 *     stop = starts[k + 1][0] if k + 1 < len(starts) else dstart
 *     body = "".join(lines[i:stop])
 *     reads = sorted({r for r in LABEL_REF.findall(body) if r in data_labels})
 * The per-function range runs from one `.thumb_func_start` to the NEXT one, so
 * it SWALLOWS THE FOLLOWING FUNCTION'S LEADING `@` COMMENT BLOCK -- and
 * `LABEL_REF = re.compile(r"\.L\w+")` matches inside comments.  Func_80f07f0's
 * doc comment contains the lines
 *     @     .Lf1770   the glyph bitmaps, 8 bytes each, 8x8 and 1bpp
 *     @     .Lf11bd   the advance width per glyph, one byte each
 * and those two names are what datacheck attributed to Func_80f0678.
 *
 * The error is CONSERVATIVE (it over-exports, so it cannot cause the failed
 * link the tool exists to prevent) but it is still wrong, and the brief told
 * four agents to take its output as "the exact export list".  Two-line fix:
 * end the range at `.func_end` when one is present, and strip everything from
 * an unquoted `@` to end-of-line before matching.  This is the FIRST measured
 * counterexample to the per-function attribution added for batch 294.
 *
 * ================================ SHIMS =================================
 *   `register ... __asm__` declarations : 0 in this file
 *   `__asm__(".equ ...")` lines         : 0
 *   other `__asm__`                     : 1 -- `__asm__ ("" : "+r" (sp0))`
 * Plus the SIX register pins inside include/dma.h's DMA3_SET, which this file
 * uses twice.  Those are not mine and are the tree's existing helper, but a
 * shim count that reads only this file understates what the object contains.
 *
 * ========================= WHAT IT IS =========================
 *
 * The credits initialiser.  Allocates a 0x400-byte display list, clears
 * 0x6000 bytes of VRAM and fills 0x100 bytes with 0x11111111 by DMA3, then
 * fills the list with {control, offset} word pairs through four loops (8, 8, 8,
 * then a 16x6 nest, then 8), clears three halfword flags, starts two tasks,
 * and pre-renders 32 credit lines.
 *
 * ================== LOAD-BEARING CONSTRUCTS, WITH SINGLE DROPS =============
 *
 * Each figure is that construct reverted ALONE from this candidate; every one
 * of them keeps size and count exact, so they are all true distances.
 *
 * 1. BLOCK-SCOPE `u32 t` AND `u32 *q` INSIDE EACH LOOP BODY -- 26 of 165 when
 *    hoisted to one shared pair at function scope.  This is the same lever that
 *    landed OvlFunc_924_200cfcc in this brief: the ROM wants a FRESH PAIR of
 *    pseudos per loop, and one shared pair puts the value and the pointer in
 *    each other's registers (`lsl r3, r6, #0x15 / mov r2, r5` against the ROM's
 *    `lsl r2, r6, #0x15 / mov r3, r5`).  Two functions in one brief, so this is
 *    now a rule and not an anecdote: WHERE THE SAME ROLE RECURS IN SEVERAL
 *    BLOCKS, GIVE EACH BLOCK ITS OWN LOCAL.
 *
 * 2. `unsigned int` LOOP COUNTERS -- 26 of 165 as `int`.  Every loop test in the
 *    ROM is `bls`, i.e. UNSIGNED, including `cmp r6, #7` where a signed counter
 *    would give `ble`.  Four loops, four wrong branches, and the shifted
 *    encodings around them.
 *
 * 3. THE SHARED `u32 m = j * 2` IN THE NESTED LOOP -- 19 of 165 without it.
 *    THIS IS THE BIGGEST FINDING IN THE FUNCTION and it is an allocation lever
 *    dressed as an algebra one.  The ROM's inner loop needs THREE outer-loop
 *    accumulators -- `j` (r6), `2*j` (r7) and `0x10 + 8*j` (r14) -- and only two
 *    can have LOW registers, because `add r3, r7, r6` is a three-operand Thumb
 *    add and r14 is only ever `mov`ed.  Written the obvious way
 *    (`0x10 + j * 8` for the control word and `j * 0x18` for the offset) gcc
 *    synthesises 0x18 as `((j<<1) + j) << 3`, creating the same three
 *    quantities, but global.c's allocno_compare
 *        pri = floor_log2 (n_refs) * n_refs / live_length
 *    ranks the `0x10 + 8*j` giv ABOVE the `2*j` biv, allocates it r7, and pushes
 *    `2*j` into r14 -- which then needs `mov r2, r14 / add r3, r2, r6` where the
 *    ROM has `add r3, r7, r6`.  Naming `m = j * 2` and using it in BOTH terms
 *    (`0x10 + m * 4` and `(m + j) * 8`) raises `2*j`'s n_refs, flips the
 *    priority, and puts the two accumulators where the ROM has them.
 *    So: TO MOVE A giv OUT OF A HIGH REGISTER, GIVE A COMPETING QUANTITY MORE
 *    REFERENCES BY SHARING A SUBEXPRESSION.  Three spellings of the same algebra
 *    without the sharing all measured 23 (`(j*2+j)*8`, `(j<<3)`, `(j+2)*8`), so
 *    it is not the algebra -- it is the reference count.
 *
 * 4. A BLOCK-SCOPE `int pr` FOR THE FIRST StartTask PRIORITY -- 8 of 165
 *    without it.  `StartTask(Func_80f0538, 0x90 << 3)`: the function pointer is
 *    a pool load, so precompute_register_parameters (calls.c:850) copies it to a
 *    pseudo first and sched2 then puts `ldr r0, =Func_80f0538` BEFORE the
 *    priority's `lsl r1, #3`, where the ROM has it after.  A local at the TOP of
 *    the function does NOT work here (still 8); a local in a BLOCK immediately
 *    around the call does.  THIS REFINES THE STANDING RULE: "name the expensive
 *    argument at the top of the function" is right when cse needs room to
 *    common it, and wrong when the goal is only to lower one cheap argument's
 *    LUID -- then the naming must be adjacent.  The SECOND StartTask needs
 *    nothing and is byte-exact either way.
 *
 * 5. `i * 0x18` WRITTEN INLINE IN THE LAST LOOP, not as a `row` accumulator --
 *    8 of 165 with an explicit accumulator.  With a source variable the ROM's
 *    `mov r6, #0 / mov r5, #0` comes out as `mov r5, #0 / mov r6, #0`, because a
 *    source initialisation is expanded before the loop counter's while a
 *    strength-reduced giv's initialisation is emitted by loop.c AFTER it.
 *
 * 6. THE LOCAL POINTER `sp0` AND ITS `"+r"` BARRIER -- 8 of 165 with the barrier
 *    dropped, 10 of 165 with the pointer dropped as well.  The ROM materialises
 *    `&v` into a register once (`mov r4, sp`) and stores through it
 *    (`str r3, [r4]`), then copies it to DMA3_SET's pinned r0 (`mov r0, r4`).
 *    `u32 *sp0 = &v;` alone does not survive: cse rewrites the pseudo to `sp`
 *    because `str r3, [sp, #0]` is a legal Thumb encoding, and the ROM's extra
 *    register disappears.  The barrier is the only thing found that keeps it.
 *    MEASURED AND REJECTED, all worse: `u32 v[1]` with `sp0 = v` (identical to
 *    no pointer at all), and using include/dma.h's DMA3_CLEAR + DMA3_FILL
 *    instead of two DMA3_SET calls -- that one is interesting and is recorded
 *    as a finding below.
 *
 * ================== THE NAMED BLOCKER FOR THE LAST 6 ==================
 *
 * All six remaining encodings are ONE-POSITION sched2 shifts inside the 16x6
 * nest, and they are all the SAME decision: the ROM schedules the control
 * word's giv chain (`mov r0, #0xc0 / lsl r0, #13`, base 0x180000, step
 * 0x200000) AHEAD of the offset's (`add r3, r7, r6 / lsl r1, r3, #3`, step 4),
 * both in the inner-loop preheader and at the increment point
 * (`add r0, r8 / add r1, #4` against our `add r1, #4 / add r0, r8`).
 *
 * THE PASS IS loop.c's strength_reduce, NOT sched2, and the mechanism is that
 * `record_giv` PREPENDS to `bl->giv`, so the giv list is in REVERSE SCAN ORDER
 * and the initialisations come out in reverse scan order too.  Read off the
 * `.08.loop` dump: insns 566/568/570/572 build the offset giv's base and insn
 * 579 sets `reg 121 = 0x180000` LAST, immediately before the inner
 * NOTE_INSN_LOOP_BEG.  To get the ROM's order the OFFSET expression must be
 * SCANNED FIRST -- i.e. appear earlier in the loop body than the control word.
 *
 * AND THAT IS UNREACHABLE BY CONSTRUCTION AT THIS SIZE, because the two
 * properties are COUPLED.  C has no way to reorder the two expressions without
 * naming the first one, and every spelling that names it
 * (`u32 n = (m+j)*8 + k*4;` before the control word, with or without a named
 * control word, and with the stores in either arrangement) RECLASSIFIES
 * 0x180000 from an inner-loop giv base into an OUTER-loop invariant: gcc then
 * hoists it out of the nest entirely and rematerialises 0x200000 inside,
 * costing TWO INSTRUCTIONS (167 against 165, size 380 against 376) and 105
 * differing encodings.  Measured on four separate spellings, all identical at
 * 167/380/105.
 *
 * So the choice is 6 encodings with the right length, or the right giv order
 * with two instructions too many.  UNREACHABLE-BY-CONSTRUCTION AT THE SOURCE
 * LEVEL; the handle, if one exists, is a change that makes 0x180000 stay a giv
 * base while being scanned second -- nothing tried does that.
 *
 * MEASURED INERT on the remaining 6, every one a single change from this
 * candidate, all still exactly 6 of 165:
 *   - `(k << 21)` for `k * (0x80 << 14)`, and `(k << 2)` for `k * 4`, together
 *     and separately.  The twin-file rule that "the SHIFT, not the algebra"
 *     decides strength reduction does NOT extend to giv ORDER.
 *   - `4 * k + (m + j) * 8` and `((m + j) * 2 + k) * 4` for the offset.
 *   - `((0xc0 << 13) + k * (0x80 << 14))` hoisted into its own named local,
 *     at inner-loop scope or before `m`.
 *   - declaring `u32 *q` separately from its assignment.
 *   - naming the control word `t` or writing it inline at the store.
 *
 * ================= A FINDING FOR include/dma.h's OPEN DECISION =============
 *
 * Batch 294 left "include/dma.h's helpers as macros rather than static inline"
 * as an owner decision.  THIS FUNCTION IS A SECOND, INDEPENDENT ARGUMENT FOR IT,
 * and by a different mechanism than the integrate.c:743/748 one already
 * recorded.  Each of DMA3_CLEAR and DMA3_FILL declares its OWN `u32 value`
 * local.  Using them for this function's two transfers gives `sub sp, #0x8` --
 * gcc-2.96 gives the two inlined locals SEPARATE STACK SLOTS -- against the
 * ROM's `sub sp, #0x4`.  So a function that does two DMA3 transfers through one
 * scratch word CANNOT be written with those two helpers at all, whatever else
 * is right; it needs DMA3_SET, which takes the source as a parameter.  As
 * MACROS sharing one caller-declared word, both forms would be available.
 * Measured: the DMA3_CLEAR + DMA3_FILL spelling is 80 aligned instructions and
 * the wrong frame; the two-DMA3_SET spelling below is byte-exact through the
 * whole DMA region.
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
            u32 m = j * 2;
            u32 t = (0x10 + m * 4) | ((0xc0 << 13) + k * (0x80 << 14)) | 0x40004000;
            u32 *q = p;
            *q++ = t;
            *q = (m + j) * 8 + k * 4;
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
