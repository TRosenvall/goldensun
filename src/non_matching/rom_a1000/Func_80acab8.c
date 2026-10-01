/* Func_80acab8 -- RECON ONLY, NO FIGURES INVENTED.
 *
 * NO objcmp and NO aligncmp number exists for this function and none is
 * claimed. Nothing was compiled against it. Everything below is measured off
 * the reference or produced by a tool, following the precedent of
 * docs/recon-959_200a7b0-Anim_Ramses.md, so whoever picks it up does not
 * repeat the survey. There is deliberately no `NON-MATCHING, N of M` line,
 * because inventing one would make this park re-measurable against a figure
 * that was never taken.
 *
 * Verify with, once a candidate exists at the installed path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/Func_80acab8.c \
 *     asm/rom_a1000/rom_aa538_c_c_c_c.s --func Func_80acab8
 *
 * SHIMS: not applicable -- there is no candidate, so shimcount.py has nothing
 * to count. The fact that replaces it: THE REFERENCE CARRIES EXACTLY TWO
 * _call_via_r3 VENEERS, both from genuine indirect calls through a function
 * pointer, and a candidate must produce exactly two. The brief's screen that
 * this function has no INLINE veneer needing an asm shim is confirmed here:
 * both veneers are ordinary `bl _call_via_r3` call sites inside the body.
 *
 * ================================================================
 * MEASURED STRUCTURE
 * ================================================================
 *
 *     asm/rom_a1000/rom_aa538_c_c_c_c.s, lines 12..934
 *     868 instructions, 58 branches, 44 labels, 179 high-register mentions
 *     frame `sub sp, #0xac`  (172 bytes)
 *     2 _call_via_r3 veneers
 *
 * THIS IS THE MOST EXPENSIVE OF THE FIVE TARGETS IN THIS BRIEF AND IT SHOULD
 * NOT BE PICKED UP FIRST. The reason is the parameter list, not the length.
 * The eight pushed registers put the first incoming stack argument at
 * sp+0xac+0x20 = sp+0xcc, and the reference reads sp+0xcc, sp+0xd0, sp+0xd4,
 * sp+0xd8 and sp+0xdc -- FIVE stack parameters on top of r0-r3, so the
 * function takes NINE ARGUMENTS. Combined with a 172-byte frame that is the
 * largest declaration list in the batch, and the spill-slot map rule has to
 * carry all of it before the first compile is worth anything.
 *
 * THE FRAME IS FULLY EXPLAINED, AND IT CLOSES EXACTLY. I grepped BOTH stack
 * address idioms rather than leaving it as homework: there is NO `mov rX, sp`
 * anywhere in the body, and only THREE `add rX, sp, #K` sites, all in one
 * instruction group -- 0x44, 0x48 and 0x4c. The call they feed settles the
 * layout:
 *
 *     add  r2, sp, #0x44
 *     add  r5, sp, #0x4c
 *     add  r3, sp, #0x48
 *     str  r2, [sp]
 *     mov  r2, r5
 *     bl   Func_80aae14
 *
 * so Func_80aae14 takes FIVE arguments and the last three are POINTERS TO
 * THREE LOCALS: sp+0x4c in r2, sp+0x48 in r3, sp+0x44 as the stack argument.
 * Immediately afterwards sp+0x4c is walked as a HALFWORD array --
 * `add r7, r3, r5` with the index doubled by `lsl r3, r0, #1`, then
 * `ldrh r3, [r7]` -- so it is an array of shorts, not a scalar, while 0x44 and
 * 0x48 are plain write-only out-parameters.
 *
 * That gives the whole 172 bytes, bottom up:
 *
 *     sp+0x00 .. sp+0x0b   outgoing argument area (3 words)
 *     sp+0x0c .. sp+0x40   FOURTEEN reload spill slots
 *     sp+0x44              addressable int out-param  (expand-time slot)
 *     sp+0x48              addressable int out-param  (expand-time slot)
 *     sp+0x4c .. sp+0xab   96-byte short array        (expand-time slot)
 *     ---------------------------------------------------------------
 *     12 + 56 + 4 + 4 + 96 = 172 = 0xac, exact
 *
 *     sp+0xcc .. sp+0xdc   the five INCOMING stack parameters (not frame)
 *
 * So there is no mystery hole -- the earlier reading of this function as
 * carrying 92 unaddressed bytes was an artifact of grepping only the
 * `sp, #imm` form. The region is one 96-byte aggregate reached exactly once by
 * its address and then walked by pointer, which is precisely the case the
 * brief's free-first-move warns the `sp, #imm` grep cannot see.
 *
 * APPLYING THE DECLARATION-ORDER RULES to that map gives the list directly,
 * and this is the one piece of real work this recon contributes:
 *   - AGGREGATE order is REVERSED, first-declared takes the HIGHEST offset.
 *     So the 96-byte short array is declared FIRST, then the sp+0x48 out-param,
 *     then the sp+0x44 out-param. (Addressable scalars are expand-time slots
 *     like aggregates, not reload spills, which is why they sit ABOVE the
 *     spill region and below the array.)
 *   - SCALARS sorted DESCENDING by offset give declaration order, so after
 *     those three come the fourteen spilled scalars in the order
 *     sp+0x40, 0x3c, 0x38, 0x34, 0x30, 0x2c, 0x28, 0x24, 0x20, 0x1c, 0x18,
 *     0x14, 0x10, 0x0c.
 *   - sp+0x40, 0x3c and 0x38 are written in the PROLOGUE from r1, r2 and r3
 *     (`str r1, [sp, #0x40] / str r2, [sp, #0x3c] / str r3, [sp, #0x38]`), so
 *     parameters 2, 3 and 4 are the first three of those fourteen and the
 *     descending order is confirmed against known quantities rather than
 *     assumed. r0 goes to r9 and is never spilled.
 *   - An unused AGGREGATE still gets a slot; an unused SCALAR does not. All
 *     seventeen here are used, so neither clause bites.
 *
 * WHAT IT DOES. The reference's own header comment names it
 * DrawDjinnCharacterPanel and the call profile confirms a text-layout
 * function, not an animation:
 *
 *     17x _Func_801ea08     13x _Func_801e7c0     12x Func_80ae9f0
 *      7x _SetTextColor      4x _Func_801e8b0      3x _SetDjinni
 *      3x _GiveDjinni        3x _Func_8019000      2x _GetUnit
 *      2x _Func_807a350      2x _call_via_r3
 *      1x each: free, _GetMoveInfo, _Func_801ec6c, _Func_801e9d4,
 *               _Func_801e41c, _CalcStats, Func_80ae958, Func_80aae14,
 *               Func_8004938
 *
 * 42 of the 71 calls go to three text routines, so most of the 868
 * instructions are argument fill for repeated draws. That is good news for
 * reconstruction -- the body is repetitive -- and bad news for the levers,
 * because 42 near-identical argument fills is exactly the shape where
 * cross-jumping (lever 8) and the argument-interleave lever decide everything
 * and a single wrong literal merges two arms that the ROM keeps apart.
 *
 * THE _SetDjinni / _GiveDjinni / _CalcStats TRIO IS A SEMANTIC WARNING, from
 * the reference's own header: `_Func_807a2e4` and `_Func_807a350` are called
 * ON A COPY of the unit record so the panel can preview what a class change
 * WOULD do without committing it. So there is a structure copy in here, and a
 * structure copy is an aggregate, and an aggregate takes a slot -- which is
 * the likeliest occupant of the sp+0x50..0xab hole. Size the unit record
 * before guessing.
 *
 * ONLY THREE POOL SYMBOLS in the whole function -- `Func_8001af8` (twice),
 * `iwram_3001f2c` and `iwram_3001e8c`. Everything else is reached off those or
 * is a numeric literal, so the band doc's section-5 CONSTANT-SET CHECK (diff
 * the distinct pooled values, our `.word` list against the reference's
 * `ldr rX, =` set) is cheap here and should be the FIRST thing read after the
 * first compile, before any hunk.
 *
 * ================================================================
 * SPLIT SHAPE -- TEXT/DATA, AND IT NEEDS ONE NEW EXPORT
 * ================================================================
 *
 * `python3 tools/datacheck.py asm/rom_a1000/rom_aa538_c_c_c_c.s`:
 *
 *     data sections : .rodata
 *     functions     : Func_80acab8
 *     EXPORTS       : .Laf28c  (already global -- NOT the set a split needs)
 *     -> converting a function here needs a TEXT/DATA SPLIT
 *     Func_80acab8 reads .Laf28c, .Laf290
 *     *** SPLIT MUST EXPORT: .global .Laf290
 *
 * `python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_c_c.s Func_80acab8
 * --dry-run` reports the file holds Func_80acab8 plus 2 blobs and 2 labels,
 * ALL of it after the code, so the split is code-to-_b and data-to-_c -- and
 * then REFUSES, naming one crossing label:
 *
 *     rom_aa538_c_c_c_c_b.s references .Laf290,
 *     defined in rom_aa538_c_c_c_c_c.s
 *
 * So ONE line to add:  .global .Laf290
 *
 * `.Laf28c` is ALREADY `.global` from an earlier split of this stem, which is
 * why datacheck lists it under EXPORTS and not under the set the split needs.
 * Do not add it twice.
 *
 * THE ASM-LABEL CAPTURE HAZARD IS SCREENED, NOT ASSUMED. The screen is
 * "contains a hex letter", because gcc's own label counter is DECIMAL.
 * `.Laf290` carries `a` and `f`, so gcc cannot mint a colliding `.LNNN`. The
 * digit length is not what makes it safe; the letters are.
 *
 * Resulting shape, Func_80acab8 being the only function in the file:
 *
 *     src/rom_a1000/rom_aa538_c_c_c_c_b.c   the function
 *     asm/rom_a1000/rom_aa538_c_c_c_c_c.s   the .rodata blobs and both labels
 *
 * and stage1.ld's `asm/rom_a1000/rom_aa538_c_c_c_c.o` lines become two in that
 * order. ALWAYS run split_s.py with --dry-run first: it DELETES a tracked .s
 * and REWRITES a linker script. Add the `.global` and verify `make compare` is
 * GREEN BEFORE running the split, so the two changes stay separable.
 *
 * NOTE for the SIZE line: 110 objects in this tree carry a `.rodata` linker
 * line with NO data section, so "no data" never means "one linker line" --
 * but this file is the other case, a real `.rodata` with real blobs, and a
 * .rodata-from-C false positive on objcmp's SIZE line is the thing to watch
 * if the data ends up in the wrong object.
 *
 * ================================================================
 * WHERE TO START, AND THE ORACLES
 * ================================================================
 *
 * 1. The declaration list above is ready to use; it is the expensive part and
 *    it is done. What is NOT done is the TYPE of the 96-byte array (48 shorts,
 *    or a struct of shorts) and what Func_80aae14 writes into it -- read that
 *    function before writing the declaration, because an aggregate of the
 *    wrong SIZE moves every offset below it.
 * 2. Nearest landed siblings by address, all in the same split chain and so
 *    the strongest available evidence for this bank's idioms:
 *      src/rom_a1000/rom_aa538_c_c_c_a_b.c and rom_aa538_c_c_c_b.c (the
 *      immediately preceding pieces of this very stem),
 *      plus rom_aa538_b.c, rom_aa538_c_b.c, rom_aa538_c_c_a_b.c,
 *      rom_aa538_c_c_a_c_b.c and rom_aa538_c_c_b.c.
 *    Seven landed files in one stem is an unusually rich oracle set; read them
 *    for the unit-record struct, the `_GetUnit` return type and the
 *    `_Func_801ea08` / `_Func_801e7c0` argument conventions before writing a
 *    line. src/rom_a1000/rom_ad274_a.c is also landed and declares
 *    `_CreateSprite`, so this bank's extern style is settled there.
 * 3. The reference's 58 branches over 868 instructions is ~15 instructions per
 *    block, the ORDINARY density -- so docs/band-800plus.md section 2 does NOT
 *    apply and the standard lever set does. This park makes no prediction
 *    about WHICH lever, because none was measured.
 */
