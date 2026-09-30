/* NON-MATCHING, 805 of 839 encodings differ  (objcmp, production flags).
 *
 * SIZE and COUNT are BOTH INEXACT, so the 805 figure SATURATES and is not a
 * true distance: ref 1796 bytes / 839 encodings, ours 1712 / 797 -- 84 bytes
 * and 42 instructions short.  Rank by size-and-count, not by 805.
 *   aligncmp: aligned-equal 281 of 839 = 33.5%, residue 726 in 135 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f2000/80f3078.c \
 *     asm/rom_f2000/rom_f2028_c_c_a_a_a_c_c.s --func Func_80f3078
 *
 * SPLIT SHAPE: NONE REQUIRED.  Func_80f3078 is the ONLY function in
 * asm/rom_f2000/rom_f2028_c_c_a_a_a_c_c.s (lines 13..895, no data section), so
 * the reference already isolates it and no split_s.py run is owed.  The three
 * rodata tables it indexes -- .Lf39ee, .Lf3a2e, .Lf3a6e -- already carry
 * `.global` in asm/rom_f2000/rom_f2028_c_c_c_c.s, so NO new exports are needed
 * either.  divsi3_RAM is a real main-ROM symbol (nm: 03000380 T divsi3_RAM).
 *
 * SHIMCOUNT: 6 register pins, and a fakematch.txt row IS DUE AT LANDING.
 * All six are inline-asm OPERAND BINDINGS for sequences gcc-2.96 cannot choose,
 * not allocation pins:
 *   - Dma3() -- 4 pins (r3/r0/r1/r2).  This is the include/dma.h class verbatim;
 *     at landing it should be PROMOTED into include/dma.h beside DMA3_COPY16,
 *     from which it differs only in taking the flag word as a parameter and
 *     dividing the size by 2 rather than 4.  Both DMA3 sites in this function
 *     need that: the ROM computes ((n-1)*3*2)/2 for the 0x80000000 transfer and
 *     (n*3)/2 for the 0x84000000 one, i.e. `flags | (size / 2)` at both.
 *   - CallVia() -- 2 pins (r0/r1), the `.call_via rN` helper.  This is the
 *     src/rom_8a000/rom_97384_c_c_a_b.c precedent, in its DOC-RECOMMENDED
 *     MULTI-SITE form: the callee is passed as a plain argument constrained
 *     "r" (f) with `bx %1` in the template, because this function has THREE
 *     sites sharing one register.
 * shimcount's "has a fakematch-class shim and NO fakematch.txt row" warning is
 * CORRECT AND EXPECTED while the function is only parked; nothing is owed until
 * the file lands.
 *
 * ===================================================================
 * THE HEADLINE, AND IT IS THE BRIEF'S PIVOTAL QUESTION:
 *   `.call_via rN` REPRODUCES BYTE-EXACTLY IN A MAIN-ROM TU.  CONFIRMED.
 * ===================================================================
 *
 * The retraction in docs/elevation.md ("RETRACTED: `.call_via rN` is a hard
 * wall") was established on Func_8097a10, an overlay-adjacent main-ROM file.
 * This is the first main-ROM confirmation on a function that MIXES the inline
 * veneer with ordinary `bl _call_via_rN` veneers, and the answer is clean.
 *
 * Measured, at the encoding level, from the aligncmp dump of the r8 diagnostic
 * below -- our three sites against the ROM's three:
 *
 *     ours  46fc  mov ip, pc        ref  46fc  mov ip, pc      IDENTICAL
 *     ours  4710  bx r2             ref  4718  bx r3           register only
 *     ours  0000  .short 0x0000     ref  0000  .short 0x0000   IDENTICAL
 *
 * The `.align 2, 0` padding halfword the macro emits is present on BOTH sides
 * at the same place, and the pc-relative pool offset for the Func_8000888 word
 * agrees exactly (`ldr rN, [pc, #128]` on both).  So the two-instruction body,
 * the alignment fill, and the pool word all match.  The ONLY residue at the
 * veneer is which register holds the callee.
 *
 * VERDICT FOR UpdateActors (31 sites): the route is OPEN.  Nothing about the
 * main ROM blocks it and nothing about mixing the two veneer forms in one
 * function blocks it.  See the report for its site table -- one callee
 * (Func_8000888) across FIVE binding registers, two of them HIGH.
 *
 * ===================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ===================================================================
 *
 * 1. THE INDIRECT CALL MUST GO THROUGH A FUNCTION-POINTER LOCAL, AT EVERY SITE.
 *    1700 -> 1712 bytes, 792 -> 797 encodings.  Four sites call divsi3_RAM; a
 *    direct `divsi3_RAM(a, b)` compiles to `bl divsi3_RAM`, where the ROM has
 *    `ldr rN, =divsi3_RAM / bl _call_via_rN`.  The RELOCATION SEQUENCE is what
 *    caught it, not the encodings: the reference carries `_call_via_r6`,
 *    `_call_via_r3` x3 and a `divsi3_RAM` ABS32 POOL WORD, and the first draft
 *    carried four `divsi3_RAM` THM_CALLs and no pool word.  Read the relocation
 *    symbol sequence separately from the offsets -- it names the defect where
 *    the encoding count only saturates.
 *
 * 2. THE THREE-SITE VENEER WANTS THE UNPINNED `bx %1` FORM, NOT THE BOUND ONE.
 *    Binding the callee to r3 inside the helper -- `register int (*_f)(int,int)
 *    __asm__("r3") = Func_8000888;`, the single-site spelling that matched
 *    Func_8097a10 -- measures WORSE here: 1712 -> 1704 bytes, 797 -> 794
 *    encodings, moving AWAY from the ref's 1796/839.  It buys `bx r3` over our
 *    `bx r2` (3 encodings) and pays 3 instructions elsewhere.  This confirms
 *    the table in docs/elevation.md ("one site -> bind the symbol; two or more
 *    sharing one register -> pass it constrained and write `bx %1`") on a
 *    THREE-site function, in the direction the doc predicts.
 *    The ROM's own pattern across the three sites is RELOAD / REUSE / RELOAD:
 *    site 2 has no `ldr r3` because nothing clobbers r3 between sites 1 and 2,
 *    and site 3 reloads because `ldr r3, [sp, #8]` uses r3 as scratch first.
 *    Ours reloads at all three.  That is 1 encoding, and it is DOWNSTREAM of
 *    the blocker below -- with one more register in play the scratch load has
 *    somewhere else to go.
 *
 * 3. THE TABLES MUST NOT BE NAMED.  Inert on size (1712 both ways) but it moves
 *    the residue: 805 -> 804 differing and it is the STRUCTURALLY RIGHT call,
 *    on the spill-slot rule.  The ROM spills the .Lf3a2e pointer to sp+0 -- the
 *    LOWEST slot in the frame -- and the spill-slot rule says low offsets are
 *    temps and outgoing-arg words while declared locals take HIGH offsets in
 *    declaration order.  So that pointer is a COMPILER TEMP from loop-invariant
 *    motion, not a declared local, and `u16 *t = Lf3a2e;` outside the loop is
 *    the wrong spelling even though it produces the same one hoisted load.
 *    Writing `Lf3a2e[z]` at each use and letting LICM hoist it is right.
 *
 * ===================================================================
 * WHAT DID NOT PAY -- MEASURED, so these are DISPROVED, not untested
 * ===================================================================
 *
 * A. SImode `int` TEMPS FOR THE THREE UNPACK STORES: 1712 -> 1676 bytes, WORSE
 *    by 36.  The ROM reaches the 5:5:5 masks with `ldr r2, =0x7c00`,
 *    `ldr r2, =0x3e0`, `ldr r3, =0x1f` -- POOLED WORDS.  Ours, storing the
 *    masked value straight through a u16 pointer, gets `ldrh r2, .L103` -- the
 *    same pool CONTENTS (the pool holds .word 31744 / 992 / 31 / 67109076, and
 *    67109076 is REG_DMA3SAD, so our pool matches the ROM's word for word) read
 *    at HALFWORD width because combine narrowed the AND to HImode.  Forcing
 *    SImode with `int p = a & 0x7c00;` does NOT restore the `ldr`: it makes gcc
 *    SYNTHESISE instead -- `mov r1, #248 / lsl r1, #7` -- because all three
 *    masks are thumb_shiftable_const (0x7c00 = 0xf8<<7, 0x3e0 = 0xf8<<2,
 *    0x1f fits in 8 bits).  So the spelling trades 1 encoding for 2 at three
 *    sites and loses 36 bytes overall.  This is a NEW sub-case of the
 *    "Counter-examples to the narrowed HImode rule" section: there an `int`
 *    local fixed a pooled small constant; here the `int` local is what STOPS
 *    the pool, and the HImode `ldrh` form is the closer of the two.
 *    An `ldr rN, =0x1f` from a CONST_INT 31 is a shape gcc will not emit from
 *    a literal at all, so the ROM's masks are reached some third way and the
 *    THIRD SPELLING IS STILL UNFOUND.  Worth 6 encodings.
 *
 * B. A FUNCTION-SCOPE LOCAL COPY OF dst (`u16 *dst = arg2;`): INERT on size and
 *    count (1712 / 797), 805 -> 815 differing.  Lever 1's inverse -- two
 *    variables where the ROM has one -- does not move this allocation.
 * C. LOCAL COPIES OF BOTH POINTERS: INERT (1712 / 797), 805 -> 820.
 * D. ONE LOOP COUNTER PER LOOP (11 separate `u32 i0..i10` in place of one
 *    function-wide `i`, lever 4's one-variable-per-region applied to the
 *    counter): 1712 -> 1704 bytes, 797 -> 793 encodings, WORSE.  The ROM's `i`
 *    is ONE pseudo in r9 across all eleven loops, and splitting it lowers its
 *    priority out of r9 rather than freeing anything.  Lever 4 does not apply to
 *    a value the ROM keeps in one register for the whole function.
 *
 * E. DECLARING `i` BEFORE `n`: INERT (1712 / 797), 805 -> 805.  `n` is at
 *    sp+0x24, the top of the frame, so it IS the first declared local and the
 *    order already published by the spill-slot rule is the one in this file.
 *
 * ===================================================================
 * THE BLOCKER, ATTRIBUTED
 * ===================================================================
 *
 * ONE allocation decision in global.c accounts for 76 of the 84 missing bytes
 * and 38 of the 42 missing instructions: THE ROM HOLDS dst IN r8, OURS HOLDS IT
 * IN r4 WITH CALLER-SAVES.
 *
 * DIAGNOSTIC, NOT A LANDING ROUTE.  Pinning the parameter -- `register u16 *dst
 * __asm__("r8") = arg2;` -- and changing nothing else:
 *
 *     natural   1712 bytes / 797 encodings   805 differing   33.5% aligned
 *     r8-pinned 1788 bytes / 835 encodings   786 differing   39.7% aligned
 *                  (ref 1796 / 839)
 *
 * 8 bytes and 4 instructions from the reference on one pin.  That figure is
 * quoted ONLY to size the blocker.  It is a PIN USED TO FORCE AN ALLOCATION
 * DECISION, which is the class the brief says has measured worse ten times and
 * is NOT a landing route -- it is categorically different from the two inline-asm
 * helpers above, which are the only way to reach sequences gcc cannot choose.
 * THE SHIPPED CANDIDATE IN THIS FILE CARRIES NO SUCH PIN.
 *
 * THE MECHANISM, and what rules out the alternatives.
 *
 * Every `strh` in this function needs a LOW base register, so a dst held in r8
 * costs `mov rL, r8` before each store.  There are 27 such stores across the
 * nine loops plus the straight-line block; that is the 38 instructions.
 *
 * gcc emits `str r4, [sp] / ldr r4, [sp]` around every call in every arm -- the
 * caller-save pattern -- so find_reg's FIRST pass failed for dst and the
 * `accept_call_clobbered` retry placed it in r4 (r4 is call-clobbered here:
 * `-fcall-used-r4` is in production flags).  The first pass fails only if EVERY
 * call-saved register conflicts with dst somewhere, and dst is live across the
 * whole function, so the question is which allocnos took r5-r11 first.
 *
 * Counting them by arm settles it.  The ROM spends exactly ONE high register per
 * switch arm on a loop-invariant, and it is always r11:
 *     0x10001  divsi3_RAM -> r6 (LOW), dst copy -> r5 (LOW)   no high reg
 *     0x10002  0x1f -> r11 ;  .Lf3a2e -> r2, SPILLED to sp+0
 *     0x10003  0x1f -> r11
 *     0x10004  .Lf39ee -> r11 ;  0x1f NOT hoisted, `mov r1, #0x1f` per iteration
 *     0x10005  .Lf3a6e -> r11 ;  0x1f NOT hoisted
 *     0x10006  0x1f -> r11
 *     0x10007  0x1f -> r11
 *     default  0x7c00/0x3e0/0x1f/dst copy -> r5/r0/r2/r1, all LOW
 * so r8 is left holding nothing but dst, r9 is i and r10 is src.
 *
 * Ours hoists TWO per arm -- the table AND 0x1f -- into r8 and r11 both, which
 * consumes the register the ROM spends on dst.  The pair `mov fp, r3 / mov r8, r1`
 * in our arm-0x10004 preheader is the defect in one line.
 *
 * So the pass to name is LOOP-INVARIANT MOTION (loop.c move_movables), not the
 * allocator: LICM runs FIRST, decides to hoist two invariants where the ROM's
 * build hoisted one, and global.c is then merely correct about the pressure LICM
 * handed it.  The reason the ROM's LICM declined the second hoist is one more
 * live pseudo in that loop body than ours has -- which is the same missing
 * register-pressure unit that leaves our frame ONE WORD SHORT: `sub sp, #0x24`
 * against the ROM's `sub sp, #0x28`, with `n` landing at sp+0x20 instead of
 * sp+0x24.  The ROM's tenth word is the sp+0 caller-save of the arm-0x10002
 * table pointer; ours reuses sp+0 for dst's caller-save instead, so we allocate
 * nine words where the ROM allocates ten.  ONE MISSING LIVE VALUE EXPLAINS ALL
 * THREE SYMPTOMS -- the frame word, the second LICM hoist, and dst in r4.
 *
 * WHAT IS RULED OUT.
 *   - sched1: does not run in this build, so no residue here is attributable to
 *     it and `-fno-schedule-insns` proves nothing.  Not invoked.
 *   - A POOL DEFECT: ruled out.  Our pool holds the ROM's words in the ROM's
 *     order (the three masks then REG_DMA3SAD), and the pc-relative offsets at
 *     the veneer sites agree exactly, so no pool word is missing or extra and
 *     there is no cascade into the later pc-relative offsets.
 *   - A WRONG PROGRAM: the immediates inside the differing hunks were read.
 *     Every clamp bound (8, 0xa, 0x10, 0x18, 0x1a, 0x1c), every divisor
 *     (3, 7, 0xa, 0x60) and both DMA flag words (0x80000000, 0x84000000) agree
 *     with the reference.  The `_call_via_rN` veneer count agrees on both sides
 *     once lever 1 is applied.  The lsr-vs-asr splits agree WITHOUT any cast:
 *     combine narrows ashiftrt to lshiftrt from the AND's nonzero_bits, and it
 *     stops doing so across a `bl` because the call ends the basic block -- which
 *     is exactly why arm 0x10003 gets `lsr r3, r7, #1` (shift before the call)
 *     and arm 0x10005 gets `asr r3, r7, #1` (shift after it) from identically
 *     spelled source.  Nothing was cast to make that happen.
 *   - THE STRUCTURE: the instruction count is 42 short of 839 and the relocation
 *     SEQUENCE matches arm for arm, so the dispatch shape below is right.  In
 *     particular arms 0x10003 and 0x10007 are the SAME SOURCE duplicated -- the
 *     ROM emits both bodies with identical instruction counts and only the
 *     scheduling order differing -- and the `a & 0x800000` loop is a second copy
 *     of the `default:` loop.  Those duplications are in the ROM, not a mistake.
 *
 * NEXT MOVE for whoever picks this up: find the tenth live value.  It is one
 * pseudo, it is live inside the switch arms, and finding it closes the frame
 * word, the LICM hoist and dst's register together.  Do not reach for the pin.
 */
#include "gba/types.h"
#include "gba/io.h"

extern int Func_8000888(int a, int b);
extern int Func_80f3898(int a);
extern int Func_80f38ac(int a);
extern int divsi3_RAM(int a, int b);

extern u16 Lf39ee[] __asm__(".Lf39ee");
extern u16 Lf3a2e[] __asm__(".Lf3a2e");
extern u16 Lf3a6e[] __asm__(".Lf3a6e");

static inline void Dma3(u32 flags, const void *s, void *d, u32 size)
{
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_s __asm__("r0") = s;
    register void *_d __asm__("r1") = d;
    register u32 _c __asm__("r2") = flags | (size / 2);
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_s), "r" (_d), "r" (_c)
        : "memory"
    );
}

static inline int CallVia(int (*f)(int, int), int a, int b)
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

void Func_80f3078(int a, u16 *src, u16 *dst, int mode)
{
    u32 n;
    u32 i;

    n = 0x200;
    if (a == 0x8000)
        a = src[0];

    if (mode == 1) {
        n = 0x100;
    } else if (mode == 2) {
        dst += 0x300;
        n = 0x100;
        src += 0x100;
    }

    if ((u32)a < 0x8000) {
        *dst++ = a & 0x7c00;
        *dst++ = (a & 0x3e0) << 5;
        *dst++ = (a & 0x1f) << 10;
        Dma3(0x80000000, dst - 3, dst, (n - 1) * 6);
    } else if ((u32)a < 0x100000) {
        switch (a) {
        case 0x10001:
            {
                int (*dv)(int, int) = divsi3_RAM;
                for (i = 0; i < n; i++) {
                    int v = *src++;
                    int g = dv(((v << 11) & 0xf800) + ((v << 7) & 0x1f000)
                               + (v & 0x7c00), 7);
                    dst[0] = g;
                    dst[1] = g;
                    dst[2] = g;
                    dst += 3;
                }
            }
            break;
        case 0x10002:
            {
                for (i = 0; i < n; i++) {
                    int (*dv)(int, int) = divsi3_RAM;
                    int v = *src++;
                    int x, y, z;
                    int q = dv((v & 0x1f) + ((v >> 5) & 0x1f)
                               + ((v >> 10) & 0x1f), 0xa);
                    x = q * 4 + 5;
                    y = q * 3 + 5;
                    z = q * 3 + 5;
                    if (x < 8)
                        x = 8;
                    if (y < 8)
                        y = 8;
                    if (z < 8)
                        z = 8;
                    if (x > 0x1c)
                        x = 0x1c;
                    if (y > 0x1c)
                        y = 0x1c;
                    if (z > 0x1c)
                        z = 0x1c;
                    *dst++ = Lf3a2e[z];
                    *dst++ = Lf3a2e[y];
                    *dst++ = Lf3a2e[x];
                }
            }
            break;
        case 0x10003:
            for (i = 0; i < n; i++) {
                int v = *src++;
                int x = v & 0x1f;
                int y = (v >> 5) & 0x1f;
                int z = (v >> 10) & 0x1f;
                x = x - (x >> 1) + 6;
                y = y - y / 3 + 4;
                z = z - 6;
                x = Func_80f3898(x);
                y = Func_80f3898(y);
                z = Func_80f3898(z);
                dst[0] = Lf3a6e[z];
                dst[1] = Lf3a2e[y];
                dst[2] = Lf39ee[x];
                dst += 3;
            }
            break;
        case 0x10004:
            {
                for (i = 0; i < n; i++) {
                    int v = *src++;
                    int x = v & 0x1f;
                    int y = (v >> 5) & 0x1f;
                    int z = (v >> 10) & 0x1f;
                    if (x < 0xa)
                        x = 0xa;
                    if (y < 0x10)
                        y = 0x10;
                    if (z < 0x10)
                        z = 0x10;
                    if (x > 0x1c)
                        x = 0x1c;
                    if (y > 0x18)
                        y = 0x18;
                    if (z > 0x1a)
                        z = 0x1a;
                    x = Func_80f3898(x);
                    y = Func_80f3898(y + 2);
                    z = Func_80f3898(z + 2);
                    *dst++ = Lf39ee[z];
                    *dst++ = Lf39ee[y];
                    *dst++ = Lf39ee[x];
                }
            }
            break;
        case 0x10005:
            {
                for (i = 0; i < n; i++) {
                    int v = *src++;
                    int x = v & 0x1f;
                    int y = (v >> 5) & 0x1f;
                    int z = (v >> 10) & 0x1f;
                    int m = Func_80f3898((x + y + z) / 3);
                    x = Func_80f3898((x >> 1) + m);
                    y = Func_80f3898((y >> 1) + m);
                    z = Func_80f3898((z >> 1) + m);
                    dst[0] = Lf3a6e[z];
                    dst[1] = Lf3a6e[y];
                    dst[2] = Lf3a6e[x];
                    dst += 3;
                }
            }
            break;
        case 0x10006:
            for (i = 0; i < n; i++) {
                int v = *src++;
                int y = (v >> 5) & 0x1f;
                int z = (v >> 10) & 0x1f;
                int x = (v & 0x1f) + (y >> 3) + (z >> 3);
                x = Func_80f3898(x);
                y = y - y / 3;
                z = z - z / 3;
                dst[0] = Lf39ee[z];
                dst[1] = Lf39ee[y];
                dst[2] = Lf3a2e[x];
                dst += 3;
            }
            break;
        case 0x10007:
            for (i = 0; i < n; i++) {
                int v = *src++;
                int x = v & 0x1f;
                int y = (v >> 5) & 0x1f;
                int z = (v >> 10) & 0x1f;
                x = x - (x >> 1) + 6;
                y = y - y / 3 + 4;
                z = z - 6;
                x = Func_80f3898(x);
                y = Func_80f3898(y);
                z = Func_80f3898(z);
                dst[0] = Lf3a6e[z];
                dst[1] = Lf3a2e[y];
                dst[2] = Lf39ee[x];
                dst += 3;
            }
            break;
        default:
            for (i = 0; i < n; i++) {
                int v = *src++;
                dst[0] = v & 0x7c00;
                dst[1] = (v & 0x3e0) << 5;
                dst[2] = (v & 0x1f) << 10;
                dst += 3;
            }
            break;
        }
    } else if ((a & 0x200000) != 0) {
        int r = a;
        int g = a >> 5;
        int b = a >> 10;

        r &= 0x1f;
        g &= 0x1f;
        b &= 0x1f;
        for (i = 0; i < n; i++) {
            int (*dv)(int, int) = divsi3_RAM;
            int v = *src++;
            int q = dv(((v << 11) & 0xf800) + ((v << 7) & 0x1f000)
                       + (v & 0x7c00), 0x60);
            int x = Func_80f38ac(r * q);
            int y = Func_80f38ac(g * q);
            int z = Func_80f38ac(b * q);
            dst[0] = z;
            dst[1] = y;
            dst[2] = x;
            dst += 3;
        }
    } else if ((a & 0x400000) != 0) {
        int r = a;
        int g = a >> 5;
        int b = a >> 10;

        r &= 0x1f;
        g &= 0x1f;
        b &= 0x1f;
        for (i = 0; i < n; i++) {
            int v = *src++;
            int (*f)(int, int) = Func_8000888;
            unsigned x, y, z;
            int (*dv)(int, int) = divsi3_RAM;
            int q = dv(((v & 0x1f) + ((v >> 5) & 0x1f)
                        + ((v >> 10) & 0x1f)) << 4, r + g + b);
            x = CallVia(f, ((r * q) >> 4) << 16, (r << 16) >> 4);
            y = CallVia(f, ((g * q) >> 4) << 16, (g << 16) >> 4);
            z = CallVia(f, ((b * q) >> 4) << 16, (b << 16) >> 4);
            x = Func_80f3898(x >> 16);
            y = Func_80f3898(y >> 16);
            z = Func_80f3898(z >> 16);
            *dst++ = Lf39ee[z];
            *dst++ = Lf39ee[y];
            *dst++ = Lf39ee[x];
        }
    } else if ((a & 0x800000) != 0) {
        for (i = 0; i < n; i++) {
            int v = *src++;
            dst[0] = v & 0x7c00;
            dst[1] = (v & 0x3e0) << 5;
            dst[2] = (v & 0x1f) << 10;
            dst += 3;
        }
    } else {
        if (mode == 2)
            a += 0x600;
        Dma3(0x84000000, (const void *)a, dst, n * 3);
    }
}
