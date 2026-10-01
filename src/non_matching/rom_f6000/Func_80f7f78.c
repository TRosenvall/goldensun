/* Func_80f7f78 -- RECON ONLY, NO FIGURES INVENTED.
 *
 * NO objcmp and NO aligncmp number exists and none is claimed; nothing was
 * compiled against it. There is deliberately no `NON-MATCHING, N of M` line,
 * because inventing one would make this park re-measurable against a figure
 * that was never taken.
 *
 * Verify with, once a candidate exists at the installed path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/Func_80f7f78.c \
 *     asm/rom_f6000/rom_f6008_c_c_c.s --func Func_80f7f78
 *
 * SHIMS: not applicable, no candidate. The fact that replaces it: THE
 * REFERENCE CARRIES ZERO _call_via_rX VENEERS -- no indirect call anywhere --
 * so a candidate that produces one has a defect, and the brief's screen that
 * this function needs no asm shim is confirmed.
 *
 * ================================================================
 * THIS IS THE ONE TO PICK UP FIRST OF THE FOUR UNMEASURED TARGETS
 * ================================================================
 *
 *     asm/rom_f6000/rom_f6008_c_c_c.s, lines 60..1000
 *     877 instructions, 80 branches, 48 labels, 156 high-register mentions
 *     frame `sub sp, #0x2c`  (44 bytes)
 *     0 veneers, FIVE calls in total, and the split needs NO new export
 *
 * Three things make it the cheapest of the four, and they are all measured:
 *   - THE SMALLEST DECLARATION LIST. 44 bytes of frame against 0xac for
 *     Func_80acab8 and 0x74 for LuckyWheelsMain. Only nine slot addresses
 *     appear in the whole body -- sp+0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24,
 *     0x28 and the `add sp, #0x2c` epilogue -- so at most eight 4-byte slots
 *     plus the outgoing-argument area. A short declaration list is the whole
 *     battle at this length.
 *   - ONLY FIVE CALLS: Func_80f7db4 once, Func_80f7e60 twice, Func_80f7f30
 *     twice. 877 instructions with five calls means the function is almost
 *     pure computation, so none of the call-boundary levers (lever 1's
 *     pointer-across-a-call, the argument interleave, caller-saves pressure)
 *     is in play. That removes most of the usual search space.
 *   - NO SPLIT EXPORT NEEDED, see below.
 *
 * WHAT IT IS, read off the body rather than guessed. It is a LONGEST-MATCHING-
 * RUN SEARCH over a ring buffer, and the same search block appears FOUR times:
 *   - `ewram_2004c00` holds a POINTER, loaded fresh as `ldr r2, [r5]` at the
 *     head of each region. The prologue writes `gBuffer` into it
 *     (`str r3, [r5]`), so the whole working set is addressed as
 *     `*(T *)(*(char **)ewram_2004c00 + K)`.
 *   - `0x3404` is a word table indexed by a ring position: `lsl r3, #2 / add
 *     r3, =0x3404 / ldr r3, [r1, r3]`. `0x3ff` masks every position, so the
 *     ring is 1024 entries. `-1` is the empty sentinel, tested as
 *     `mov r2, #1 / neg r2, r2 / cmp r3, r2`.
 *   - `0x10f` (271) is the run-length cap and `0x3e` (62) the minimum-distance
 *     window, tested as `sub r3, r5, #1 / cmp r3, #0x3e / bhi` -- an unsigned
 *     range test, so the source is `(unsigned)(d - 1) <= 0x3e`.
 *   - Fields at 0x4430 (best length), 0x442c (best distance), 0x4434
 *     (position), 0x4438, 0x443c, 0x4440, 0x4408, 0x3408 and `0xc0 << 6`
 *     (= 0x3000, the hash-chain heads).
 *   - `0x88 << 1` = 0x110 recurs as both a loop bound and a comparison, which
 *     is `0x10f + 1`, so it is the same cap reached the other way.
 * In short this is an LZ-style match finder. That matters for the approach:
 * THE FOUR NEAR-IDENTICAL SEARCH BLOCKS ARE THE WHOLE RISK. gcc 2.96 does not
 * inline static functions at -O2 without `inline`, so the duplication is in
 * the source, and lever 8 applies in its sharpest form -- cross-jumping
 * compares EMITTED TAILS, so any literal that erases what distinguishes two of
 * the four blocks invites a merge the ROM does not have. Write all four out
 * separately and do NOT factor them, however much they want factoring.
 *
 * The inner run-compare loop is also worth reading before writing it: the
 * reference enters it with `mov r4, #1 / cmp r2, r1 / bne` and then loops
 * `add r4, #1 / cmp r4, #0x10f / bgt`, i.e. the first comparison is PEELED
 * ahead of the loop. That is jump.c:1137 duplicate_loop_exit_test on a `while`
 * or `for`, which a `do`-`while` can never reach -- so the source is a `while`
 * loop, not a `do`-`while`, and lever 6 settles it without a probe.
 *
 * ================================================================
 * SPLIT SHAPE -- CLEAN THREE-WAY, NO EXPORTS
 * ================================================================
 *
 * `python3 tools/datacheck.py asm/rom_f6000/rom_f6008_c_c_c.s`:
 *
 *     data sections : .rodata
 *     functions     : Func_80f7f30, Func_80f7f78
 *     EXPORTS       : .Lf86f8, .Lf870c, .Lf8712, .Lf871a, .Lf8728, .Lf8736
 *                     (already global -- NOT the set a split needs)
 *     Func_80f7f30  reads no data label -> split needs NO new export
 *     Func_80f7f78  reads no data label -> split needs NO new export
 *
 * So the six `.L` labels in this file belong to OTHER functions and are
 * already exported from an earlier split of this stem. NEITHER function here
 * reads a data label, and `python3 tools/split_s.py
 * asm/rom_f6000/rom_f6008_c_c_c.s Func_80f7f78 --dry-run` does NOT refuse:
 *
 *     would write asm/rom_f6000/rom_f6008_c_c_c_a.s  (1 function,   38 lines)
 *     would write asm/rom_f6000/rom_f6008_c_c_c_b.s  (1 function,  960 lines)
 *     would write asm/rom_f6000/rom_f6008_c_c_c_c.s  (1 function,   21 lines)
 *     would REMOVE asm/rom_f6000/rom_f6008_c_c_c.s
 *     would rewrite stage1.ld
 *
 * The target lands as src/rom_f6000/rom_f6008_c_c_c_b.c. The _c piece is only
 * 21 lines and holds the `.rodata`; note that 110 objects in this tree carry a
 * `.rodata` linker line with NO data section, so do not assume the number of
 * linker lines from the absence of data. ALWAYS run split_s.py with --dry-run
 * first: it DELETES a tracked .s and REWRITES a linker script. Verify
 * `make compare` is GREEN after the split and BEFORE writing any .c.
 *
 * ================================================================
 * WHERE TO START
 * ================================================================
 *
 * 1. Settle the eight slots first. The prologue is explicit about three of
 *    them -- `str r0, [sp, #0x28]`, `str r1, [sp, #0x24]`,
 *    `str r0, [sp, #0x20]` with r0 freshly zeroed -- so parameters 1 and 2
 *    take sp+0x28 and sp+0x24 and a zero-initialised local takes sp+0x20.
 *    Sorting the spilled scalars DESCENDING gives declaration order, and here
 *    the two parameters sit at the TOP of that order exactly as the rule
 *    predicts, which is a free confirmation that the rule holds in this bank
 *    before anything is written. r2 goes to r6 and is never spilled.
 * 2. Nearest landed sibling by address: src/rom_f6000/rom_f6008_c_a_e_a_b.c is
 *    landed and already declares `extern unsigned char *iwram_3001ef0;`, which
 *    is this bank's extern style for the iwram pointer block. Read the rest of
 *    src/rom_f6000/ for the `ewram_2004c00`-through-a-pointer idiom before
 *    choosing between `*(T *)(base + K)` and a struct.
 * 3. The band doc's section-5 CONSTANT-SET CHECK is unusually cheap here and
 *    should come FIRST after the first compile: the whole function uses only
 *    two pool SYMBOLS (`ewram_2004c00` and `gBuffer`) and a small set of
 *    numeric literals (0x3404, 0x3408, 0x3ff, 0x10f, 0x3e, 0x4408, 0x442c,
 *    0x4430, 0x4434, 0x4438, 0x443c, 0x4440, 0xc0<<6, 0xd0<<6, 0x88<<1,
 *    0xa8<<2). One `grep | sort | uniq -c` against the generated `.s` settles
 *    whether an 877-instruction reconstruction has the right program.
 * 4. 80 branches over 877 instructions is ~11 instructions per block, the
 *    ORDINARY density and the densest of the five targets in this brief -- so
 *    docs/band-800plus.md section 2 (cse1 cross-call constant commoning) does
 *    NOT apply, and with five calls it cannot apply. The standard lever set
 *    does. This park makes no prediction about WHICH lever, because none was
 *    measured.
 */
