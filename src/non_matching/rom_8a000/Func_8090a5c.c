/* Func_8090a5c  --  0x08090a5c  --  PARK (batch 313, brief B)
 *
 * NON-MATCHING, 815 of 849  (tools/objcmp.py, PRODUCTION FLAGS:
 *   -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
 *   -ffreestanding -fcall-used-r4 -Iinclude -- the tree default for this path,
 *   NO Makefile row needed and none should be written)
 *
 * SIZE   ref 1816 bytes, ours 1796  --  INEXACT by 20 (ours SHORT)
 * COUNT  ref 849 encodings, ours 840  --  INEXACT by 9 (ours SHORT)
 * ALIGNCMP (tools/aligncmp.py, separately): 206 aligned-equal of 849 = 24.3%,
 *        977 differing/ins/del in 102 hunks.
 * POOL   DISTINCT VALUE SET IS EXACT, 11 = 11, after numeric normalisation.
 *        The whole pool residue is MULTIPLICITY (ref 31 words, ours 14).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/Func_8090a5c.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_c.s --func Func_8090a5c
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_8a000/Func_8090a5c.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_c.s Func_8090a5c
 *
 * ===================== SHAPE, SPLIT, FRAME, VENEER =======================
 * SPLIT SHAPE: NONE.  `grep -c thumb_func_start` on the reference is 1, so this
 * is a WHOLE-FILE conversion to src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_c.c
 * when it lands.  tools/datacheck.py is SILENT on the reference.
 * tools/shimcount.py on this candidate: 2 register pins, both inside the
 * `call_via` inline helper.  THAT HELPER'S PRECEDENT IS ALREADY LANDED --
 * src/rom_c9000/rom_e3958_c_c_c_a.c carries the identical helper, reports the
 * same 2 pins, and IS BOOKED in fakematch.txt (line 543, Func_80e3994).  So
 * landing this file needs a fakematch.txt row for Func_8090a5c; shimcount's
 * "has a fakematch-class shim and NO fakematch.txt row" warning on the park is
 * expected and is not a defect.
 *
 * FRAME, ALL FOUR GREPS (and the fourth corrects the brief's table):
 *   g1  `sub sp, #0x28` / `add sp, #0x28`  -- 40 bytes, well under the 508 cap
 *   g2  `(add|sub) sp, rN`                 -- ZERO, so no register-built frame
 *   g3  `mov rX, sp` ZERO and `add rX, sp, #K` ZERO  -- NO STACK AGGREGATES
 *   g4  `str rX, [sp]` 1  AND  `ldr rX, [sp]` 1  -- *** PAIRED ***
 * The brief's table lists `sp0 1` for this function, which reads as outgoing
 * argument space for a 5-or-more-argument call.  IT IS NOT.  The store is
 * PAIRED with a load: the reference spills the .L9e96e table pointer to sp+0
 * across the `divsi3_RAM` call in the 0x10002 arm and reloads it after.  That
 * is a SPILL SLOT.  Pairing stores against loads is the discriminator the
 * frame triad already names, and on this function it flips the reading.
 * Our candidate reproduces the 40-byte frame EXACTLY.
 *
 * VENEER: 3 inline sites, ALL ON r3, counted scoped and DOT-ANCHORED
 * (`^[[:space:]]*\.call_via`).  The same range also carries 4 `bl _call_via_rN`
 * which are gcc's OWN output and must not be added to the veneer count.
 * Three sites is >= 2, so the UNPINNED `bx %1` form is correct here and is what
 * this candidate uses; all three sites reproduce.
 *
 * ================= WHAT THIS FUNCTION IS, STRUCTURALLY ===================
 * A palette post-process.  Arguments are (colour, src, dst, mode); `mode`
 * selects the whole palette (448 entries), the first 224, or the second 224,
 * and `colour` is OVERLOADED three ways:
 *   * below 0x8000  -- a literal 5:5:5 colour; unpack it into one 3-halfword
 *     destination entry and DMA3-replicate that entry over the rest;
 *   * 0x10001..0x10007 -- a 7-way switch (gcc lowers it as `add 0xfffeffff`,
 *     unsigned `<= 6`, jump table at .L90b1c), each arm an unrolled per-entry
 *     recolour: split the 5:5:5 fields, transform, index one of three u16
 *     lookup tables, store three halfwords;
 *   * 0x200000 / 0x400000 / 0x800000 bit flags -- tint, blend, passthrough;
 *   * otherwise `colour` IS A SOURCE ADDRESS and the tail is a 32-bit DMA copy.
 * `.L9e92e`, `.L9e96e` and `.L9e9ae` are UNDEFINED `.L` SYMBOLS, i.e. GLOBAL
 * VARIABLES, not labels -- they are loaded as addresses and indexed as u16
 * arrays.  Declared here with the tree's
 * `extern unsigned short L9e92e[] __asm__(".L9e92e");` convention.
 *
 * ===================== LEVERS MEASURED, WITH FIGURES =====================
 * Ranked on size-and-count first, then on the PER-OPCODE HISTOGRAM (rung 8),
 * then on aligncmp.  Every row below is a real objcmp run at production flags.
 *
 *  # candidate                                 size   count  aligned  hunks
 *  0 first full draft                          +40    +28    --        --
 *  1 non-volatile palette read                 +36    +26    18.5%     64
 *  2 + `int v = *src++` (kills 11 `ldrsh`)       0     +1     --        --
 *  3 + unsigned loop counter and source value    0     +1    23.0%     82
 *  4 + unsigned preamble field shifts            0     +1     --        --
 *  5 + ONE MASK VARIABLE PER REGION             -20    -9    24.3%    102  <- PARKED
 *
 * 1. THE VOLATILE QUALIFIER ON A ONE-SHOT REGISTER READ COSTS A COPY.
 *    `colour = *(vu16 *)(0xa0 << 19);` emits `ldrh r3,[r3] / mov r0,r3`; the
 *    reference has the bare `ldrh r0,[r3]`.  Dropping `volatile` is worth
 *    4 bytes and 2 encodings.  The read is of palette RAM entry 0 and happens
 *    once, so nothing needs the qualifier.
 *
 * 2. THE SINGLE BEST EDIT OF THE BATCH, AND IT IS A TYPE, NOT A SPELLING.
 *    `unsigned short v = *src++;` compiles to `ldrsh` + `lsl #16` + `lsr #16`
 *    -- a SIGN-EXTENDED load immediately re-zero-extended -- and the field
 *    extractions are then built off the shifted copy (`lsr r7,r3,#21` for what
 *    the reference writes as `lsr r7,r4,#5`).  ELEVEN `ldrsh` against the
 *    reference's ZERO.  `int v = *src++;` with `src` already `unsigned short *`
 *    gives the reference's plain `ldrh`.  Worth 36 BYTES AND 25 ENCODINGS in
 *    one token -- size went EXACT and the count to +1.
 *    THE SCREEN IS ONE GREP AND IT SHOULD COME FIRST ON ANY FUNCTION THAT
 *    LOADS HALFWORDS: `grep -c '^[[:space:]]*ldrsh'` on both sides.  A
 *    reference with zero `ldrsh` and a candidate with eleven is a TYPE defect,
 *    not an allocation one, and no allocation lever will touch it.
 *    This sits with, and is the opposite direction of, the existing finding
 *    that Thumb `ldrsh` has no immediate-offset form: there the cost was the
 *    zero-offset register; here it is the sign extension itself.
 *
 * 3. RUNG 8 EARNED ITS KEEP IMMEDIATELY.  At step 2 the count was +1 of 849 --
 *    which reads like one pool word from done.  The histogram said otherwise:
 *        bcc -21 | blt +21 | lsr -21 | asr +21 | mov -20 | str +17 | ldrh +9
 *    FORTY-TWO instructions of pure SIGNEDNESS cancelling to zero inside the
 *    count.  The loop counter and limit must be `unsigned` (the reference's
 *    loop branch is `bcc`, i.e. unsigned `<`) and the source halfword must be
 *    unsigned (the field extractions are `lsr`).  Both ±21 columns went to
 *    ZERO on that one change, at UNCHANGED size and count.  A candidate whose
 *    total does not move while 42 instructions come right is exactly the case
 *    rung 8 was written for.
 *
 * 4. THE REFERENCE SHIFTS `colour` UNSIGNED IN BOTH FLAG PREAMBLES.
 *    `(unsigned int)colour >> 5` / `>> 10` takes asr/lsr from +-7 to +-3.
 *    No size or count movement; kept on the histogram alone, which is the
 *    documented discipline for keeping a change on encoding evidence when the
 *    figures do not move.
 *
 * 5. ONE VARIABLE PER REGION -- AND THE SPILL-SLOT ACCESS TABLE IS WHAT FOUND
 *    IT.  The slot table at step 4 read:
 *        ours  [sp] x8  [sp,#20] x7  [sp,#32] x4  [sp,#36] x3  ... 31 stores
 *        ref   [sp,#0x24] x3  [sp,#0x20] x2  [sp,#0x18] x2  ... 14 stores
 *    Our heaviest slot took 7 stores where the reference's heaviest takes 3.
 *    The culprit was ONE function-level `int u` carrying the 0x1f field mask in
 *    all seven switch arms AND the `sa + sb` accumulator in the blend loop.
 *    Serving eight regions, it earned a stack slot and was stored and reloaded
 *    around every call.  Writing the literal `0x1f` in the arms the reference
 *    rebuilds it in (0x10004..0x10007, where the reference has an in-loop
 *    `mov rX,#0x1f`), keeping a declared variable only in the two arms the
 *    reference HOLDS it in r11 (0x10002, 0x10003), and giving the blend
 *    accumulator its own name, took `str` from +17 to +10 -- EXACTLY the seven
 *    stores that slot carried -- and aligned 23.0% -> 24.3%.
 *    THE RANKING NOTE THAT MATTERS: this step moved size from EXACT to -20 and
 *    the count from +1 to -9, i.e. BOTH HEADLINE FIGURES GOT WORSE, while the
 *    histogram flattened (sum of |per-opcode delta| 56 -> 49) and the
 *    positional figure improved.  It is parked as the best candidate on that
 *    basis.  Anyone re-ranking this park on size-and-count alone will pick
 *    step 4 and be wrong; the spill table and the histogram both say step 5.
 *
 * ============== THE PIN PASS: MEASURED, AND NEGATIVE OR INERT ============
 * The pooled-constant multiset is this function's dominant signal and it is
 * the band's signature exactly.  AFTER NUMERIC NORMALISATION the DISTINCT sets
 * are IDENTICAL (11 = 11); every difference is multiplicity, and every
 * shortfall is on our side:
 *
 *     value          ref  ours
 *     .L9e92e          6     2
 *     divsi3_RAM       4     1
 *     .L9e96e          4     1
 *     .L9e9ae          3     1
 *     0x3e0            3     1
 *     0x1f             3     1
 *     Func_8000888     2     1
 *     0x7c00           3     3   OK
 *     REG_DMA3SAD      2     2   OK
 *     0xfffeffff       1     1   OK
 *     (+ the jump table: ref 1 base word + 7 entries, ours 1 + 7, MATCHED)
 *
 * By the rule recovered from src/non_matching/ovl_7f6e64/20088b4.c -- WHERE THE
 * REFERENCE RELOADS AT EVERY SITE, PIN -- every one of those seven rows is a
 * pin candidate.  ALL THREE PIN SETS WERE MEASURED AND NONE PAYS:
 *
 *   candidate                                      size   count
 *   no pins (step 4 baseline)                        0     +1
 *   ALL pins (9 table-address + 3 call-target)      +20    +8
 *   ONLY the 3 call-target pins (divsi3_RAM)        +20    +8
 *   ONLY the 9 table-address pins                    0     +1
 *
 * TWO SEPARATE RESULTS, AND BOTH ARE BOUNDS WORTH HAVING.
 *
 *   * THE 9 TABLE-ADDRESS PINS ARE SIZE-AND-COUNT IDENTICAL TO NO PINS -- AND
 *     THE OBJECT IS NOT IDENTICAL.  The generated `.s` differs in ~60 places:
 *     register rotation throughout, `ldrh r3,[r2,r3]` -> `ldrh r3,[r3,r2]`
 *     operand-order swaps, and a PERMUTED SPILL-SLOT ASSIGNMENT (sp+4/8/12/16
 *     rotated).  Equal size AND equal count with a materially different stream
 *     is a sharper form of "separated-axis exactness is necessary and still not
 *     sufficient": here the two axes are not merely insufficient, they are
 *     BLIND, and only the `.s` diff sees it.  I nearly recorded this pair as
 *     "byte-identical" off the two figures; it is not.
 *     WHY THE PIN CANNOT REACH IT: the quantity is a LOOP-INVARIANT ADDRESS.
 *     A pin forces the REGISTER, not the REBUILD -- so gcc hoists the pool load
 *     out of the loop into a callee-saved register and satisfies the pin with a
 *     copy at each site.  There is nothing for a pin to rebuild.  The
 *     reference's per-iteration reload comes from holding the base in a
 *     CALL-CLOBBERED register across the `Func_8091294` calls in those arms,
 *     which is an allocation outcome, not a source spelling.
 *
 *   * THE 3 CALL-TARGET PINS ARE STRICTLY NEGATIVE, +20/+7, and they carry
 *     100% of the full pass's regression -- the "ALL" row and the
 *     "call-targets only" row are the SAME figures.  So on this function the
 *     pin pass does not merely fail to pay, it is entirely accounted for by
 *     its call-site half, which is the half the doc says selection pays over.
 *
 * THE GENERAL STATEMENT THIS SUPPORTS is in the batch report: the pin pass's
 * DOMAIN is the CONSTANT-ROOTED class of the `mov rlo,rhigh` partition, and
 * this function's partition has essentially none of it (2 of 59).  The pool
 * multiset said "pin" and the partition said "do not"; THE PARTITION WAS RIGHT.
 * Read the partition before the multiset when they disagree.
 *
 * ===================== WHAT IS LEFT, ATTRIBUTED =========================
 * Corrected histogram at the parked candidate.  The reference's three
 * `.call_via r3` lines are MACROS: each expands to `mov r12,pc` + `bx r3` plus
 * an `.align` fill, so a histogram taken over the raw reference UNDER-COUNTS
 * its `mov` by 3 and its `bx` by 3.  Expanded, the parked candidate is:
 *
 *     mov  -19   str  +10   ldrh  +9   ldr  -5
 *     asr   +3   lsr   -3   add   -2   sub  -1   bx 0   TOTAL -8
 *
 * (An unexpanded histogram reports `bx +3` and `mov -16`; that is the macro
 * accounting, not a residue.  This is a fourth flavour of the `.call_via`
 * counting family the doc already collects, and it bites the HISTOGRAM rather
 * than a site count.)
 *
 * `str +10` with `ldr` from the stack at 45 against the reference's 36 is the
 * whole remaining story: WE STILL SPILL TEN QUANTITIES THE REFERENCE HOLDS.
 * Our slot [sp] takes 8 stores against the reference's 1.  That slot carries
 * the `Func_8091294` return values in the 0x10003/0x10007 arms, where the
 * reference keeps them in r6/r7 across the calls.  The next round on this
 * function is a per-arm live-range question, not a constant question, and the
 * spill-slot access table is the instrument -- not the aligned figure, which
 * averages a slot correction away.
 *
 * `ldrh +9` is the next-largest single column and is NOT yet attributed; it is
 * 9 extra halfword loads against ref 33, and it should be read per-arm before
 * anything else is tried.
 *
 * ===================== NEGATIVES AND INERT RESULTS ======================
 *   * `*dst++` IS WORSE THAN `dst[0]/dst[1]/dst[2]`, +8 bytes / +4 encodings,
 *     IN THE SINGLE-COLOUR PATH -- and the reference's own stream LOOKS like
 *     `*dst++`: three `strh rX,[rY]` at OFFSET ZERO with an `add r8,#2`
 *     between them.  The reason the appearance is misleading is that the
 *     destination pointer lives in a HIGH register (r8), which Thumb-1 cannot
 *     use as a store base, so every store needs its own `mov rlo,r8` ANYWAY
 *     and the pointer bump is the compiler's, not the source's.
 *     RULE: OFFSET-ZERO STORES THROUGH A HIGH-REGISTER POINTER ARE NOT
 *     EVIDENCE OF `*p++` IN THE SOURCE.  Indexed stores are the right spelling
 *     even where the disassembly shows offset zero.
 *   * `n - 1` AS ITS OWN NAMED QUANTITY IS WORSE, +4 bytes / +2 encodings,
 *     even though the reference computes `sub r3,#1` BEFORE the multiply and
 *     our build distributes `(n-1)*6` into `n*6 - 6`.  The distributed form is
 *     cheaper here and the reference's order is not recoverable this way.
 *   * The DMA tail is SHARED by two paths in the reference (one `stmia` site
 *     reached from both the single-colour path and the pointer path, with
 *     different source, flags and unit size).  Writing it as two separate
 *     DMA3_SET calls was not tried and SHOULD BE -- the shared-variable form
 *     used here is what the single `.L91156` entry implies, but it costs three
 *     function-level locals (`dsrc`, `flags`, `size`) and lever 5 has just
 *     shown what a function-level local costs on this function.
 *   * NO FLAG WAS SWEPT.  Flags are per-function and none is cited.  The
 *     obvious candidate is `-fno-rerun-cse-after-loop` -- this function HAS
 *     loops, seven of them, so the "has a loop is not the precondition"
 *     qualification cuts the other way and it is a legitimate one-measurement
 *     probe.  It is the first thing to try next and it was not run.
 */
#include "gba/types.h"
#include "dma.h"

extern int divsi3_RAM(int a, int b);
extern int Func_8000888(int a, int b);
extern int Func_8091294(int a);
extern int Func_80912a8(int a);

extern unsigned short L9e92e[] __asm__(".L9e92e");
extern unsigned short L9e96e[] __asm__(".L9e96e");
extern unsigned short L9e9ae[] __asm__(".L9e9ae");

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

void Func_8090a5c(int colour, unsigned short *src, unsigned short *dst, int mode)
{
    unsigned int n;
    unsigned int i;
    int a, b, c;
    int t;
    unsigned short *d;
    int (*dv)(int, int);
    unsigned short *tab;
    const void *dsrc;
    u32 flags;
    u32 size;
    int sa, sb, sc;
    int u;
    int uu;
    int pa, pb, pc;
    unsigned int ub;

    n = 0xe0 << 1;
    if (colour == (0x80 << 8))
        colour = *(unsigned short *)(0xa0 << 19);
    if (mode == 1) {
        n = 0xe0;
    } else if (mode == 2) {
        dst += 0x540 / 2;
        n = 0xe0;
        src += 0x1c0 / 2;
    }

    if ((unsigned int)colour < (unsigned int)(0x80 << 8)) {
        dst[0] = colour & 0x7c00;
        dst[1] = (colour & 0x3e0) << 5;
        dst[2] = (colour & 0x1f) << 10;
        size = (n - 1) * 6;
        flags = 0x80 << 24;
        dsrc = dst;
        dst += 3;
        DMA3_SET(dsrc, dst, flags | (size / 2));
        return;
    }

    if ((unsigned int)colour < (unsigned int)(0x80 << 13)) {
        switch (colour) {
        case 0x10001:
            dv = divsi3_RAM;
            d = dst;
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                t = ((v << 11) & (0xf8 << 8)) + ((v << 7) & (0xf8 << 9))
                  + (v & (0xf8 << 7));
                t = dv(t, 7);
                d[0] = t;
                d[1] = t;
                d[2] = t;
                d += 3;
            }
            return;
        case 0x10002:
            u = 0x1f;
            tab = L9e96e;
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                a = v & u;
                b = (v >> 5) & u;
                c = (v >> 10) & u;
                t = a + b + c;
                t = divsi3_RAM(t, 0xa);
                a = t * 4 + 5;
                b = t * 3 + 5;
                c = b;
                if (a <= 7) a = 8;
                if (c <= 7) c = 8;
                if (b <= 7) b = 8;
                if (a > 0x1c) a = 0x1c;
                if (c > 0x1c) c = 0x1c;
                if (b > 0x1c) b = 0x1c;
                *dst++ = tab[b];
                *dst++ = tab[c];
                *dst++ = tab[a];
            }
            return;
        case 0x10003:
            u = 0x1f;
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                ub = v & u;
                b = (v >> 5) & u;
                c = (v >> 10) & u;
                if ((ub | b | c) != 0) {
                    a = Func_8091294(ub - (ub >> 1) + 0xa);
                    b = Func_8091294(b - b / 3 + 8);
                    c = Func_8091294(c - 7);
                } else {
                    a = ub;
                }
                *dst++ = L9e9ae[c];
                *dst++ = L9e96e[b];
                *dst++ = L9e92e[a];
            }
            return;
        case 0x10004:
            tab = L9e92e;
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                a = v & 0x1f;
                b = (v >> 5) & 0x1f;
                c = (v >> 10) & 0x1f;
                if (a <= 9) a = 0xa;
                if (b <= 0xf) b = 0x10;
                if (c <= 0xf) c = 0x10;
                if (a > 0x1c) a = 0x1c;
                if (b > 0x18) b = 0x18;
                if (c > 0x1a) c = 0x1a;
                a = Func_8091294(a);
                b = Func_8091294(b + 2);
                c = Func_8091294(c + 2);
                *dst++ = tab[c];
                *dst++ = tab[b];
                *dst++ = tab[a];
            }
            return;
        case 0x10005:
            tab = L9e9ae;
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                a = v & 0x1f;
                b = (v >> 5) & 0x1f;
                c = (v >> 10) & 0x1f;
                t = Func_8091294((a + b + c) / 3);
                a = Func_8091294((a >> 1) + t);
                b = Func_8091294((b >> 1) + t);
                c = Func_8091294((c >> 1) + t);
                dst[0] = tab[c];
                dst[1] = tab[b];
                dst[2] = tab[a];
                dst += 3;
            }
            return;
        case 0x10006:
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                a = v & 0x1f;
                b = (v >> 5) & 0x1f;
                c = (v >> 10) & 0x1f;
                a = Func_8091294(a + (b >> 3) + (c >> 3));
                b = b - b / 3;
                c = c - c / 3;
                dst[0] = L9e92e[c];
                dst[1] = L9e92e[b];
                dst[2] = L9e96e[a];
                dst += 3;
            }
            return;
        case 0x10007:
            for (i = 0; i < n; i++) {
                unsigned int v = *src++;
                ub = v & 0x1f;
                b = (v >> 5) & 0x1f;
                c = (v >> 10) & 0x1f;
                a = Func_8091294(ub - (ub >> 1) + 6);
                b = Func_8091294(b - b / 3 + 4);
                c = Func_8091294(c - 6);
                dst[0] = L9e9ae[c];
                dst[1] = L9e96e[b];
                dst[2] = L9e92e[a];
                dst += 3;
            }
            return;
        }
        for (i = 0; i < n; i++) {
            unsigned int v = *src++;
            dst[0] = v & 0x7c00;
            dst[1] = (v & 0x3e0) << 5;
            dst[2] = (v & 0x1f) << 10;
            dst += 3;
        }
        return;
    }

    if (colour & (0x80 << 14)) {
        sa = colour;
        sb = (unsigned int)colour >> 5;
        sc = (unsigned int)colour >> 10;
        sa &= 0x1f;
        sb &= 0x1f;
        sc &= 0x1f;
        for (i = 0; i < n; i++) {
            unsigned int v = *src++;
            t = ((v << 11) & (0xf8 << 8)) + ((v << 7) & (0xf8 << 9))
              + (v & (0xf8 << 7));
            t = divsi3_RAM(t, 0x60);
            a = sa * t;
            b = sb * t;
            c = sc * t;
            a = Func_80912a8(a);
            b = Func_80912a8(b);
            c = Func_80912a8(c);
            dst[0] = c;
            dst[1] = b;
            dst[2] = a;
            dst += 3;
        }
        return;
    }

    if (colour & (0x80 << 15)) {
        sa = colour;
        sb = (unsigned int)colour >> 5;
        sc = (unsigned int)colour >> 10;
        sa &= 0x1f;
        sb &= 0x1f;
        sc &= 0x1f;
        uu = sa + sb;
        pb = sa << 16;
        pa = sb << 16;
        pc = sc << 16;
        for (i = 0; i < n; i++) {
            unsigned int v = *src++;
            a = v & 0x1f;
            b = (v >> 5) & 0x1f;
            c = (v >> 10) & 0x1f;
            t = divsi3_RAM((a + b + c) << 4, uu + sc);
            a = call_via(Func_8000888, ((sa * t) >> 4) << 16, pb >> 4);
            b = call_via(Func_8000888, ((sb * t) >> 4) << 16, pa >> 4);
            c = call_via(Func_8000888, ((sc * t) >> 4) << 16, pc >> 4);
            a = Func_8091294((unsigned int)a >> 16);
            b = Func_8091294((unsigned int)b >> 16);
            c = Func_8091294((unsigned int)c >> 16);
            *dst++ = L9e92e[c];
            *dst++ = L9e92e[b];
            *dst++ = L9e92e[a];
        }
        return;
    }

    if (colour & (0x80 << 16)) {
        for (i = 0; i < n; i++) {
            unsigned int v = *src++;
            dst[0] = v & 0x7c00;
            dst[1] = (v & 0x3e0) << 5;
            dst[2] = (v & 0x1f) << 10;
            dst += 3;
        }
        return;
    }

    dsrc = (const void *)colour;
    if (mode == 2)
        dsrc = (const char *)dsrc + 0x540;
    size = n * 3;
    flags = 0x84 << 24;
    DMA3_SET(dsrc, dst, flags | (size / 2));
}
