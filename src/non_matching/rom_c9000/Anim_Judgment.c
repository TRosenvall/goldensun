/* Anim_Judgment (asm/rom_c9000/rom_ea0d8.s:108, 0x080ea0d8, 2455 ROM
 * instructions) --
 * NO CANDIDATE.  NO FIGURE IS CLAIMED.
 *
 * STATED PLAINLY: no .c was written, so nothing here has been through objcmp,
 * aligncmp or tryc.  Everything below is a grep or a region read of the
 * reference and re-derivable in one command.  The batch's depth went to
 * Anim_Boreas (measured park, objcmp 1263 of 1416) and Anim_Cybele
 * (structural recon).
 *
 * Install path if a candidate is written: src/non_matching/rom_c9000/Anim_Judgment.c
 * Recipe to use then:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Judgment.c \
 *     asm/rom_c9000/rom_ea0d8.s --func Anim_Judgment
 *
 * ================================================================
 * TWO CORRECTIONS TO THE BRIEF'S TRIAGE, BOTH MEASURED
 * ================================================================
 *
 * 1. "1 function, no split" IS WRONG.  `tools/datacheck.py
 *    asm/rom_c9000/rom_ea0d8.s` reports a `.rodata` section and
 *        *** SPLIT MUST EXPORT: .global .Leef28 .global .Leef30
 *                               .global .Leef3e .global .Leef4a
 *                               .global .Leef50
 *    This is a TEXT/DATA split with FIVE exports.  Because the file holds
 *    exactly one function with all its data AFTER the code, split_s.py takes
 *    its "function goes to _b, the data to _c" path (tools/split_s.py:261) --
 *    a TWO-part split, the cheapest of the four targets, but NOT free and
 *    NOT "no split".  `--dry-run` was NOT run on this stem; run it first.
 *
 * 2. "14 aggregates" IS AN UPPER BOUND AND THE TRUE COUNT IS SIX.  Thumb-1
 *    has no sp-relative sub-word load or store, so every byte or halfword
 *    stack SCALAR also materialises a base with `add rX, sp, #K` and is
 *    indistinguishable from an aggregate at that grep.  Worse here: the
 *    fifteen sites resolve to only SEVEN distinct offsets, and one of those
 *    lies INSIDE another object.  The EIGHT `add rX, sp, #0x8c` sites are ONE
 *    BASE RE-MATERIALISED EIGHT TIMES, in eight different basic blocks.
 *
 * ================================================================
 * THE FRAME -- RESOLVED BLOCK-AWARE, AND IT BALANCES TO THE BYTE
 * ================================================================
 *
 *     sub sp, #0xb8                        184 bytes
 *     (add|sub) sp, rN                      NONE
 *     add rX, sp, #K                        0xa0 (x2), 0x8c (x8), 0x7c
 *     mov rX, sp  then  add rX, #K          0xac, 0x90, 0x74
 *     add rX, sp                            ONE SITE -- the fifth form:
 *                                           `mov r6,#0x6c / add r6, sp`
 *     str rX, [sp] with no matching load    46 (outgoing args) -- the most of
 *                                           the four, and it makes this
 *                                           function's CALLS wide, not its
 *                                           frame deep
 *
 * SIX AGGREGATES, with the arithmetic that fixes every boundary:
 *     sp+0x6c   2 words   (0x6c + 8 = 0x74)
 *     sp+0x74   2 words   (0x74 + 8 = 0x7c)   address spilled to slot 0x18
 *     sp+0x7c   4 words   (0x7c + 0x10 = 0x8c)  written +4 and +0xc
 *     sp+0x8c   5 words   (0x8c + 0x14 = 0xa0)  -- AND sp+0x90 IS INSIDE IT
 *     sp+0xa0   3 words   (0xa0 + 0xc = 0xac)
 *     sp+0xac   3 words   (0xac + 0xc = 0xb8 = the frame)
 * TOTAL 0x6c..0xb8 = 0x4c bytes = 19 words, and 2+2+4+5+3+3 = 19.  Nothing
 * left over.
 *
 * sp+0x8c IS THE CENTRAL OBJECT AND IT IS PASSED BY ADDRESS EVERYWHERE.  Six
 * of its eight materialisations are immediately `mov r0, rX` into a call
 * (`LoadVFXFile`, `_AnimTransitionIn` twice, and three more), one feeds an
 * `stmia r3!, {r0, r1, r2}` three-word block copy, and `sp+0x90` is
 * separately materialised and spilled to slot 0x14 as `&obj[1]`.  THE `stmia`
 * IS THE TELL FOR A STRUCT ASSIGNMENT, and the Boreas and Gaia parks both
 * warn in the other direction: a struct assignment that SHOULD be three
 * `ldr`/`str` pairs coming out as `ldmia`/`stmia` is the separated-axis trap
 * (Anim_Gaia's rung-3 note).  Read the region, do not assume.
 *
 * THE PHANTOM-HOLE CHECK, which the coordinator's correction demands:
 * the `[sp,#imm]` census shows 0x6c:2, 0x74:1, 0x78:1 and 0x8c:10 -- four
 * apparent slots with suspiciously low counts.  EVERY ONE of them is an
 * AGGREGATE WORD, not a scalar: 0x6c, 0x74 and 0x8c are materialised bases
 * above, and 0x78 is the second word of the sp+0x74 object.  So they must be
 * STRUCK FROM THE DECLARATION LIST.  That drops the scalar count from the
 * 28 slots the raw census suggests to TWENTY-FOUR.
 *
 * THE SLOT MAP IS THE DECLARATION LIST -- 24 scalars, 0x0c..0x68 step 4 with
 * no holes.  Reference access counts, the hot end descending (the census
 * regex MUST accept DECIMAL immediates):
 *     0x68  42    iwram_3001ef0[0]     -- `ctx`
 *     0x64  56    iwram_3001ef0[-1]    -- `base`; `base + 0x7828` is derived
 *                                         from it in the prologue
 *     0x60   3    iwram_3001ef0[4]
 *     0x5c   3
 *     0x58  13
 *     0x54  73    THE HOTTEST IN THE FUNCTION -- the frame counter
 *     0x50   7    iwram_3001ef0[1]
 *     0x4c   2    iwram_3001ef0[-0x1c]
 *     then 0x48:4 0x44:7 0x40:3 0x3c:3 0x38:2 0x34:10 0x30:6 0x2c:4 0x28:5
 *          0x24:4 0x20:11 0x1c:5 0x18:2 0x14:4 0x10:4 0xc:4
 *     and 0x8:9 0x4:31 0x0:46 as outgoing arguments.
 *
 * FIVE TABLE ENTRIES ARE READ, the most of any of the four: one pool word at
 * `iwram_3001ef0` reached at -0x70, -4, 0, +4 and +0x10, each spilled to its
 * own slot.  Spell it as ONE pointer with five subscripts, the
 * Boreas/Ramses `void **p = iwram_3001ef0; ... p[-1] ... p[4]` idiom rather
 * than Cybele's and Ragnarok's `ldmia` walk -- the reference's `sub r3, r5,
 * #4` / `ldr r2,[r5,#0x10]` / `sub r3,#0x70` pattern is cse working offsets
 * off a single symbol load, which is what the subscript form produces.
 *
 * ================================================================
 * WHAT ELSE IS KNOWN, AND THE FIRST MOVES
 * ================================================================
 *
 *  - `*(State **)(base + 0x7828) = context;` then `AnimStart(0x80 << 6)` --
 *    the same non-zero argument Anim_Thor passes, and unlike Boreas, Cybele,
 *    Gaia and Ramses, which pass 0.
 *  - `REG_BG2PA = 0x100;` with the address held in r8 across the store
 *    (`mov r8,r2 / mov r4,r8 / strh r3,[r4]`), then `Func_80c9048()` and the
 *    two zeroed palette entries at 0x5000000/0x5000002 sharing ONE pooled
 *    zero -- the same opening Anim_Boreas and Anim_Ramses have.  Take their
 *    spelling; and note the Boreas park's finding that the `ldrh`-vs-`ldr`
 *    print difference on these pooled constants is a mnemonic-text artefact
 *    with IDENTICAL ENCODINGS, so spend nothing on it.
 *  - FOURTEEN `BuildDraw2DFuncEx` and FOURTEEN `gfree` against eight
 *    `LoadVFXFile`: this function builds far more blitters than any sibling,
 *    and five `_call_via_r9` plus four `_call_via_r10` mean at least TWO
 *    blitters live in HIGH registers across calls.  That is Anim_Gaia's open
 *    BLOCKER 2(b) in a function that will need it solved, not avoided --
 *    expect the declaration list, not a per-region local, to decide it (Gaia
 *    measured a per-region blitter local BYTE-IDENTICAL).
 *  - `_AnimTransitionIn` is called FOUR times.  `rom_ea0d8.s` is named in
 *    Anim_Ramses's nine-site pooled-small-int anomaly (`=0x3b`, `=0x3a`,
 *    `=0x3e` and `=0x36` all appear in THIS file), so expect four instances
 *    of that known blocker, 1 instruction and 1 pool word each.  It is
 *    bank-wide and must not be charged to this function.
 *  - COMPARISON CENSUS -- DONE PROPERLY, AND THE BRIEF'S WORST-CASE READING
 *    IS WRONG.  The whole-function mnemonic ratio (bne 49 against ble 27,
 *    bge 27, bgt 15, blt 1, plus bhi 13 / bls 2) makes this look like the
 *    most signed-heavy of the four -- "70 signed".  It counts COMPARISONS,
 *    not loops.  CLASSIFIED BY BACKWARD EDGE -- target label defined earlier
 *    in the stream -- Judgment has 29 back edges: 25 close on `bne`, 4 are
 *    unconditional `b` (loop rotation), and ZERO are signed closures.
 *    GAIA'S `!=` RULE TRANSFERS WHOLE.  Write every loop `i != N`.  All 70
 *    signed compares are clamps, frame-phase tests and the biases of this
 *    function's very heavy signed division (44 `asr`, the most of the four).
 *  - THE CARRIER-TYPE SCREEN MATTERS MORE HERE THAN ANYWHERE IN THE BATCH,
 *    AND IT IS ONE GREP.  The reference has `ldrsh` ONE, `ldrh` 17 and `lsl`
 *    131 -- i.e. essentially NO sign-extending halfword load against the most
 *    shifting of the four.  An `unsigned short v = *src++;` carrier emits
 *    `ldrsh` plus `lsl #16` plus `lsr #16` per site; `int v = *src++;` emits
 *    none.  So a wrong halfword carrier would show up here as a pile of
 *    `ldrsh` against a reference that has one, and a sibling brief measured
 *    that single token at 36 bytes and 25 encodings on one function.  SCREEN
 *    FOR `ldrsh` BEFORE READING ANY OTHER FIGURE: if the candidate has more
 *    than one, the carrier's TYPE is the defect, not the expression.
 *  - A rung-8 histogram on this reference needs the `.call_via` check first:
 *    the macro at include/macros.inc:64 expands to `mov r12,pc` + `bx`, so a
 *    histogram over an UNEXPANDED reference under-counts `mov` and `bx` by
 *    one per macro site.  `rom_d6970.s` was checked and has none; this stem
 *    was NOT checked -- and Judgment has FIVE veneer registers (r3, r4, r5,
 *    r9, r10) across 34 indirect calls, so verify before trusting a `mov`
 *    column.  Getting this wrong reads as `bx +3 / mov -16` where the truth
 *    is `bx 0 / mov -19`.
 *
 * FIRST MOVES, RANKED
 * 1. Resolve the sp+0x8c five-word object and whether sp+0x90 is a second
 *    member or a second argument.  Nothing else can be right until it is.
 * 2. Build the 24-scalar declaration list from the slot map with the four
 *    aggregate words struck out, and check the frame comes out 0xb8 with the
 *    six bases at 0x6c/0x74/0x7c/0x8c/0xa0/0xac BEFORE reading any other
 *    figure -- the Boreas park's lever 1 is the standing proof that the
 *    positional figure moves the wrong way on a correct frame change to these
 *    functions.
 * 3. Take Anim_Boreas's / Anim_Ramses's extern block and struct shapes; this
 *    function shares their prologue idiom.
 * 4. The 46 `str rX, [sp]` mean several 5-, 6- and 7-argument calls.  Get the
 *    argument counts right from the store offsets (0x0:46, 0x4:31, 0x8:9)
 *    before writing the call sites.
 */

/* NO CANDIDATE BODY.  Nothing below this line. */
