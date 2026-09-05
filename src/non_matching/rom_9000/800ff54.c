/* UpdateScreenEdge_H  --  0x0800ff54   PARKED
 *   [asm/rom_9000/rom_f9cc_c.s, 2nd of 4 + a .rodata section]
 *
 * BLOCKER CLASS: THE REGISTER-ROLE SWAP.
 *
 * 48 differing of 79, and the size is EXACT -- 172 bytes against 172, pool
 * words in the ROM's order, all three relocations at the ROM's offsets. objcmp
 * prints no SIZE line and no RELOCATIONS line. Started at 92 lines / 89
 * differing; ended at 78 / 48.
 *
 * `i`, `yy`, `row`, `col`, `mr` and the whole r1-r4 scratch quartet are the
 * ROM's. `xb`, `dst`, `mc` and `xa` are ROTATED among r8/r9/r12/r14, and every
 * one of the 48 differing encodings is downstream of that one rotation.
 *
 * THE SCAN'S SHAPE WAS A SYMPTOM, NOT THE CAUSE. It flagged the `mov r0,#0`
 * interleaved into the mov/lsl builds; that is real, but the diagnosis is one
 * line further on -- THE ROM RELOADS ALL THREE SYMBOL POOLS INSIDE THE LOOP, so
 * the source's loop was not a while/for/do.
 *
 * WHY NO SPELLING REACHES IT, read from loop.c in the build image rather than
 * inferred: a thumb SYMBOL_REF goes through force_const_mem, so each
 * `ldr rN,=sym` is a load from a constant MEM, and TWO hoisters take it --
 * move_movables (threshold 2*(1+n_non_fixed_regs) = 30, savings 1, lifetime 1,
 * so ANY loop of 30 insns or fewer moves one) and then load_mems, whose only
 * escapes are a BLKmode store, a call, alloca, or multiple exit targets. None
 * applies here. A REAL LOOP THEREFORE ALWAYS HOISTS, and the goto rewrite is
 * what turns both passes off. Corroborated inside this same .s:
 * UpdateScreenEdge_V names its two masks before its loop, and Func_8010230
 * rebuilds a mov/lsl pair inside its outer loop -- the recorded goto tell.
 *
 * WHAT IS ALREADY RIGHT AND SHOULD NOT BE RE-DERIVED: the goto loop; `mr`/`mc`
 * as named locals (with hoisting off, anything in a register before the loop
 * was written there, and the counter's `mov r0,#0` then lands BETWEEN their
 * split builds -- which is the interleave the scan saw); `volatile` on the three
 * accesses; PARAMETER-AS-COUNTER, worth 73 -> 56 and four registers snapping
 * into place, because the parameter dies after the destination is computed and
 * reusing it gives the counter global_alloc's hard-reg preference for r0, which
 * frees r4 for the scratch quartet; and naming the second EWRAM base ahead of
 * the first store, which creates the ROM's fourth simultaneously-live scratch.
 *
 * SEARCH ALREADY DONE, DO NOT REPEAT: a hill climb over all 9! preheader
 * statement orders with several restarts, plus split and counter-position
 * families, bottoms out at 48. The theory-driven "longest-lived first" order is
 * WORSE at 55. Seven flag groups measured; -Os matches the LINE COUNT by
 * coincidence only (it skips move_movables so the masks are not hoisted, while
 * load_mems still hoists all three pools).
 *
 * NEW WHILE HERE -- A THIRD volatile SIGNATURE: an address MATERIALISED where
 * register-offset addressing was available. The doc records two (a global
 * reloaded; a stack halfword written then read back) and separately records the
 * offset-clobber lever for explicit addresses. This case has NOTHING TO CLOBBER
 * -- the address is dead immediately after -- and a probe shows all four
 * non-volatile spellings fold to `ldr r0,[r0,r3]`. Only volatile stops combine,
 * because gcc-2.96's combine will not fold a register-plus-register sum into a
 * volatile MEM.
 *
 * NEXT: the rotation itself. Nothing in the preheader ordering reaches it, so it
 * wants either a lever that changes global_alloc's preference for one of the
 * four rotated values, or evidence about the original declaration order that
 * the search above cannot supply.
 *
 * Signature is UNCONSTRAINED: no prototype in include/ or src/, and no solved
 * twin. Types read from the body -- the first parameter is shifted left 11 and
 * never compared, so unsigned; the other two use the signed halving idiom.
 *
 * Landing if ever closed: split_s.py, remapping .rodata as well as .text; two
 * stage1.ld lines name the object. No Makefile rule matches this stem.
 */
/* UpdateScreenEdge_H  (asm/rom_9000/rom_f9cc_c.s, 0x0800ff54, 78 instructions)
 *
 * NOT A MATCH.  objcmp, run against BOTH the extracted ref and the original
 * asm/rom_9000/rom_f9cc_c.s (identical verdicts, so no Makefile pattern rule is
 * biting):
 *
 *     XX ENCODINGS differ in 48 place(s) (ref 79, ours 79)
 *        first at index 5: ref 4b24  ours 02c3
 *
 * There is no SIZE line and no RELOCATIONS line: the object is 172 bytes
 * against the ROM's 172, the pool words are in the ROM's order, and all three
 * relocations (gBuffer, ewram_2020000, ewram_2020004) land at the ROM's
 * offsets.  The whole residue is register ROLES.
 *
 * BLOCKER CLASS: the register-role swap ("The register-role swap is now the
 * dominant wall", docs/elevation.md).  Four values are rotated against the
 * ROM and every one of the 48 differing encodings is downstream of that:
 *
 *     value                  ROM   ours
 *     loop counter           r0    r0    ok
 *     yy   (y & 0x1e) << 5   r5    r5    ok
 *     row  map row offset    r6    r6    ok
 *     col  map column        r7    r7    ok
 *     mr   0x3f80            r10   r10   ok
 *     xb   x & 1             r12   r14
 *     dst  0x6002800 + ...   r14   r8
 *     mc   0x3c0             r8    r9
 *     xa   x & 0x1e          r9    r12
 *     loop scratch quartet   r1-r4 r1-r4 ok
 *
 * The loop body is instruction-for-instruction the ROM's; the four names above
 * are simply in each other's slots, which shifts every `mov rLOW, rHIGH`
 * shuttle in the body and the two-instruction `dst` build in the prologue.
 *
 * ------------------------------------------------------------------ levers --
 *
 * 1.  THE LOOP IS A `goto` LOOP, and that is a DIAGNOSIS, not a guess.
 *
 *     The scan flagged this function for "the zero interleaved into a shifted
 *     build" -- `mov r0,#0` (the counter) sitting inside the split builds of
 *     0x3f80 and 0x3c0.  That is the silhouette, and it is NOT the cause.  The
 *     cause is visible one line further on: the ROM reloads all three symbol
 *     addresses from the literal pool INSIDE the loop
 *
 *         .Lffa0: ... ldr r4,=gBuffer ... ldr r3,=ewram_2020000
 *                 ... ldr r4,=ewram_2020004 ...
 *
 *     while gcc hoists them into callee-saved registers and then spills.
 *
 *     Read from the compiler, not inferred (gcc-2.96 loop.c in the build
 *     image).  A thumb SYMBOL_REF is materialised by force_const_mem, so every
 *     `ldr rN,=sym` is a `(set (reg) (mem/u/f (symbol_ref "*.LCn")))`.  Such a
 *     mem is hoisted by TWO independent mechanisms:
 *
 *       * move_movables (loop.c:1803) moves it when
 *             threshold * savings * m->lifetime  >=  insn_count
 *         and for a thumb `for` loop with no calls the numbers are
 *         threshold = 2 * (1 + n_non_fixed_regs) = 30, savings = 1,
 *         lifetime = 1 -- so ANY loop of 30 real insns or fewer moves one,
 *         and `threshold -= 3` after each move;
 *
 *       * whatever move_movables declines, load_mems (loop.c:9395) then takes
 *         ("Hoisted regno NN r/o from (mem/u/f:SI (symbol_ref ...))"), and its
 *         only escapes are unknown_address_altered (a BLKmode store),
 *         loop_info->has_call, calls_alloca, and has_multiple_exit_targets
 *         (prescan_loop, loop.c:2510).  None of them exists here.
 *
 *     So a `while`/`for`/`do` loop in this shape ALWAYS hoists the pool loads;
 *     it is not reachable by any spelling and no flag in the group touches it
 *     (measured below).  Both hoisters are driven off NOTE_INSN_LOOP_BEG/END,
 *     which stmt.c emits only for while/for/do, so a backward `goto` turns
 *     both off at once -- docs/elevation.md, "`goto` loops disable loop
 *     optimisation ENTIRELY".
 *
 *     Corroboration inside the same .s: UpdateScreenEdge_V builds its two
 *     masks (0x7f, 0x1e) into r8/r14 before its loop and reloads all three
 *     pools inside it, and Func_8010230 REBUILDS `mov r3,#0xfe / lsl r3,#6`
 *     and `mov r3,#0xf0 / lsl r3,#2` inside its OUTER loop -- the textbook
 *     "constant rebuilt in the loop" goto tell.  All four loops in this TU are
 *     goto loops.
 *
 * 2.  `mr` / `mc` ARE NAMED LOCALS.  With hoisting off, anything sitting in a
 *     register before the loop had to be put there by the source (the
 *     corollary in the same section).  `mr = 0x7f << 7; mc = 0x1e << 5;`
 *     reproduces the ROM's `mov r4,#0xfe / lsl r4,#6 / mov r10,r4` and
 *     `mov r2,#0xf0 / lsl r2,#2 / mov r8,r2` preheader pair -- and with the
 *     counter's `mov r0,#0` landing between them, which is the interleave the
 *     scan was pointing at.  It falls out of the goto rewrite; it is NOT the
 *     argument-interleave lever (there are no calls here at all).
 *
 * 3.  `volatile` ON THE THREE ACCESSES.  The ROM materialises every address:
 *
 *         add r3, r4 / ldr r1,[r3]        not  ldr r1,[r3,r4]
 *         add r2, r1, r3 / ldrh r2,[r2]   not  ldrh r2,[r1,r3]
 *
 *     Measured on a three-line probe, every non-volatile spelling folds the
 *     add into the mem -- `t += (unsigned int)gBuffer; *(unsigned int *)t`,
 *     `*(unsigned int *)((unsigned int)gBuffer + (t<<2))`, `gBuffer[t]` and a
 *     two-statement int-local chain all give `ldr r0,[r0,r3]`.  Marking the
 *     access `volatile` is the only thing that stops combine folding the
 *     address, and it gives the ROM's `add` + `[rN]` exactly.  This reads as a
 *     real property of the source: the destination is BG VRAM at
 *     0x6002800 + block*0x800 and the two sources are EWRAM tile tables.
 *
 *     NEW (grepped by concept against "volatile is a reading", "register-offset
 *     load's operand order", "explicit address computation", "Finish the OFFSET
 *     before the base" and "Which operand is the POINTER"): the doc lists two
 *     volatile signatures -- a global RELOADED, and a stack halfword written
 *     then read back.  A THIRD one is an ADDRESS MATERIALISED where
 *     register-offset addressing was available.  gcc-2.96's combine will not
 *     fold `(plus (reg)(reg))` into a volatile MEM, so `add rD, rB / ldr rX,
 *     [rD]` on an address that is dead immediately afterwards -- where the
 *     recorded offset-clobber lever has nothing to clobber -- is a volatile
 *     tell, not an allocator artefact.
 *
 * 4.  THE PARAMETER IS THE LOOP COUNTER.  `block` arrives in r0, dies after
 *     `dst = 0x6002800 + (block << 11)`, and the ROM's counter is r0.  Reusing
 *     it as the counter (docs: "A parameter and a loop counter can be the SAME
 *     variable") gives the counter global_alloc's hard-register PREFERENCE for
 *     r0, which in turn frees r4 for the loop's scratch quartet: our scratch
 *     went from r0-r3 to the ROM's r1-r4, and `row`, `col`, `yy` and `mr` all
 *     snapped to the ROM's registers in the same step.  73 differing -> 56.
 *     `block` has to be `unsigned int` for `cmp r0,#0xa / bls`; `x` and `y`
 *     stay `int` (both are divided by 2 with the signed lsr#31/add/asr idiom).
 *
 * 5.  THE FOURTH LIVE SCRATCH.  The ROM issues `ldr r4,=ewram_2020004` BEFORE
 *     the first `strh`, so four values (m, d, v, base) are live at once.  With
 *     the base loaded after the store only three are, r4 is left over for a
 *     long-lived value, and the whole allocation rotates.  Naming the second
 *     base in its own statement ahead of the store -- `e = (unsigned
 *     int)ewram_2020004;` -- reproduces the ROM's instruction ORDER at that
 *     point and is what takes the push list to the ROM's three high registers.
 *     It is load-bearing, not scaffolding: without it, 71 lines and 2 pushed
 *     high registers against the ROM's 78 and 3 (75 differing).
 *
 * ------------------------------------------------------- measured as WORSE --
 *
 * Screened with tools/tryc.py --align against the extracted ref.  "differ" is
 * the disagreeing-region count; the ROM is 78 lines.
 *
 *   spelling / flag                                        lines  differ
 *   plain `for` loop, array subscripts                       92     89
 *   plain `for`, (m<<20)>>18 spelling                        91     88
 *   plain `for`, int-local address chains                    89     86
 *   plain `for` + volatile on all three accesses             87     84
 *   goto loop, masks left inline                             58     77
 *   goto loop + named masks (no volatile)                    68     76
 *   goto loop + named masks + volatile                       71     75
 *   + parameter-as-counter, no `e`                           71     75
 *   + `e` before the store, i in its own variable            78     56
 *   + parameter-as-counter (THIS FILE)                       78     48
 *
 *   -O1 (natural for-loop)                                   77     76
 *   -Os                                                      78     73   (the
 *        equal line count is a coincidence: -Os skips move_movables so the
 *        masks are NOT hoisted, while load_mems still hoists all three pools)
 *   --no-sched2 / --no-rerun-cse / -fno-gcse /
 *     -fno-strict-aliasing / -fno-strength-reduce /
 *     -fno-schedule-insns   (each on the best candidate)     71     75  (inert)
 *   -ffixed-r7                                               77     76
 *
 *   Inert on the final candidate (all 78 / 48):
 *     `d = xa + yy` vs `d = yy + xa`;
 *     `s = (unsigned int)ewram_2020000 + m` vs `m + (unsigned int)...`;
 *     `m = (unsigned int)gBuffer + m` vs `m += (unsigned int)gBuffer`;
 *     a separate address local for the gBuffer read (`a` instead of reusing
 *     `m`) -- gcc coalesces them;
 *     `xb = 1; xb &= x;` (the constant-as-destination copy-then-modify): it
 *     DOES reproduce the ROM's `mov r2,#1 / mov r12,r2 / mov r3,r12 / and
 *     r3,r1 / mov r12,r3` shuttle, but only once xb already has a high
 *     register, so it buys nothing on its own.
 *
 *   Searched, not just poked: a hill climb over all 9! orderings of the
 *   preheader statements (single-element moves, several restarts, plus a
 *   split-`xa` copy-then-modify family and an `i`-position sweep) bottoms out
 *   at 48.  The best order is the one below; the theory-driven order that
 *   would put `xa` at the longest live range and `dst` at the shortest
 *   (xa, mr, mc, col, row, yy, dst, xb, i) is worse, at 55.
 *
 * ------------------------------------------------------------- what is left --
 *
 * The four rotated values are exactly the ones that land in r8/r9/r12/r14 --
 * the slots after the low registers run out.  Local alloc now takes the ROM's
 * r1-r4 and the five low-register globals are all correct, so the remaining
 * question is global.c's ordering of the last four allocnos, which
 * docs/elevation.md records as unreachable from C ("A recurring residue class:
 * the parameter pointer one register too low" -- the cause is in find_reg's
 * conflict/preference pass, not in allocno_compare).
 *
 * ------------------------------------------------------------------ landing --
 *
 * asm/rom_9000/rom_f9cc_c.s holds FOUR functions --
 *   .thumb_func_start UpdateScreenEdge_V   @ 0x0800fec8
 *   .thumb_func_start UpdateScreenEdge_H   @ 0x0800ff54   <- this one
 *   .thumb_func_start UpdateFieldScreen    @ 0x08010000
 *   .thumb_func_start Func_8010230         @ 0x08010230
 * -- plus a `.section .rodata` holding `.global .L13784` / `.incrom 0x13784,
 * 0x1404c`.  A whole-file .c replacement is therefore NOT available; landing
 * this needs tools/split_s.py, and the split must remap the .rodata section as
 * well as .text.
 *
 * stage1.ld names the object on two lines (matched on the full path, quoted by
 * content):
 *     "\t\tasm/rom_9000/rom_f9cc_c.o(.text)"    -- immediately after
 *        "asm/rom_9000/rom_f9cc_b.o(.text)" and before
 *        "asm/rom_9000/rom_10424_a.o(.text)"
 *     "\t\tasm/rom_9000/rom_f9cc_c.o(.rodata)"  -- immediately after
 *        "asm/rom_9000/rom_13624.o(.rodata)" and before the "} > rom" that
 *        closes the section
 *
 * FLAG GROUP: none.  `grep -n rom_f9cc Makefile` is empty, so the generic
 * `%.o: %.c` rule at Makefile:135 applies and the TU is built with plain
 * GCC296_CFLAGS (-O2).  objcmp gives the same verdict against the extracted
 * ref and against the original asm/ path, confirming no rule is biting.
 *
 * SIGNATURE: no header constrains it.  `grep -rn UpdateScreenEdge include/ src/`
 * is empty, and tools/solved_twins.py has no solved twin for it (the obvious
 * sibling, UpdateScreenEdge_V, is in the same unelevated .s).  The three
 * parameters are read from the body: r0 is shifted left 11 and never compared
 * (unsigned int, and reused as the counter), r1 and r2 are both halved with
 * the signed-division idiom (int).
 */

extern unsigned int gBuffer[];
extern unsigned short ewram_2020000[];
extern unsigned short ewram_2020004[];

void UpdateScreenEdge_H(unsigned int block, int x, int y)
{
    unsigned int dst;
    unsigned int row;
    unsigned int col;
    unsigned int yy;
    unsigned int xa;
    unsigned int xb;
    unsigned int m;
    unsigned int s;
    unsigned int e;
    unsigned int d;
    unsigned int mr;
    unsigned int mc;

    dst = 0x6002800 + (block << 11);
    row = ((y / 2) & 0x7f) << 7;
    mc = 0x1e << 5;
    yy = (y & 0x1e) << 5;
    xa = x & 0x1e;
    xb = x & 1;
    col = (x / 2) & 0x7f;
    mr = 0x7f << 7;
    block = 0;
again:
    m = row + col;
    m <<= 2;
    m += (unsigned int)gBuffer;
    m = *(volatile unsigned int *)m;
    m = (m << 20) >> 18;
    m += xb;
    m <<= 1;
    s = m + (unsigned int)ewram_2020000;
    d = yy + xa;
    d += xb;
    d <<= 1;
    d += dst;
    e = (unsigned int)ewram_2020004;
    *(volatile unsigned short *)d = *(volatile unsigned short *)s;
    s = m + e;
    d += 0x40;
    *(volatile unsigned short *)d = *(volatile unsigned short *)s;
    row = (row + 0x80) & mr;
    yy = (yy + 0x40) & mc;
    block++;
    if (block <= 10) goto again;
}
