/* OvlFunc n/a -- `free` (FreeScratch), asm/rom_c0/rom_2dd8.s, 0x08002df0, 6 instructions.
 *
 * FIGURE MEASURED THIS BATCH (no figure was inherited; no park named `free` as
 * a subject -- it had been matched to src/non_matching/tiny_reg_order.c BY
 * SUBSTRING ACCIDENT on the word "free" in that file's prose).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b317/A/p1_candidate.c asm/rom_c0/rom_2dd8.s --func free
 *
 *   production flags : 5 of 6 differ, 7 encodings against 7, SIZE EXACT (16/16),
 *                      RELOCATIONS EXACT (one R_ARM_ABS32 gPtrs at offset 0xc).
 *   -ffixed-r3       : EXACT instruction-stream match (tryc "OK free (6 lines)").
 *
 * RESULT: UNMATCHABLE, on a NEW and INDEPENDENT argument. The batch-265 park
 * argument (one shared literal pool word) is REFUTED for `free` and SURVIVES
 * only for gfree -- see below -- so this replaces it rather than inheriting it.
 *
 * ---------------------------------------------------------------------------
 * 1. THE BODY IS REACHED EXACTLY, ORDER AND OPERAND SHAPE INCLUDED.
 *
 * The C below measures 6 instructions against the ROM's 6, in the ROM's order,
 * with the ROM's operand shape, differing ONLY in which hard register each
 * value occupies:
 *
 *     rom   ldr r4,=gPtrs  mov r1,#4  lsr r2,r0,#22  and r2,r1  str r0,[r2,r4]
 *     ours  ldr r1,=gPtrs  mov r2,#4  lsr r3,r0,#22  and r3,r2  str r0,[r3,r1]
 *
 * Two levers, CROSSED, were needed to get there and neither is sufficient alone:
 *   - a NAMED MASK LOCAL (`mask = 4`) fixes the INSTRUCTION ORDER. Without it
 *     gcc emits the `mov #4` and the pool load in the other order (v2/f1, v2/f5).
 *   - the PLUS FLIP IN INTEGER SPACE (`*(void **)(off + base)`, base an
 *     `unsigned int`) fixes the STORE'S OPERAND ORDER, putting the scaled index
 *     in Rn and the array address in Rm as the ROM has it. Spelled as pointer
 *     arithmetic (`(char *)gPtrs + off`) fold renormalises it and the operands
 *     come out the other way (v3/g2 vs v3/g1), exactly as the brief predicts.
 *
 * 2. THE WHOLE REGISTER RESIDUE IS ONE CONDITION: r3 MUST BE UNAVAILABLE.
 *
 * Read off the dumps, not guessed. `.17.lreg` for this body:
 *
 *     Register 32 (p)     used 3 times across  6 insns   -> r0
 *     Register 33 (base)  used 2 times across 10 insns   -> r1
 *     Register 34 (mask)  used 2 times across  6 insns   -> r2
 *     Register 36 (index) used 4 times across  4 insns   -> r3
 *
 * `.18.greg` says ";; 0 regs to allocate" -- the function is branchless, so
 * global_alloc never runs and LOCAL-ALLOC assigns all four. Probed empirically
 * (scratch_elev/b317/A/probe/, functions with 2, 3 and 6 mutually conflicting
 * quantities): local-alloc walks the registers in the order 3, 2, 1, 0, 4, 5, 6
 * and takes quantities SHORTEST-LIVED FIRST, which is the same local-alloc.c:1480
 * ordering the Makefile's FIXEDR7_CFLAGS note cites. So:
 *     index (L=4) -> r3, p -> r0 by copy preference, mask (L=6) -> r2,
 *     base (L=10) -> r1.
 * The ROM instead has index->r2, mask->r1, base->r4. Every one of those three
 * follows from deleting r3 from that walk and changing NOTHING else: index
 * takes r2, mask takes r1, base finds r3/r2/r1 taken and r0 held by p and
 * lands on r4. Confirmed: -ffixed-r3 on this exact body is an EXACT match.
 *
 * THE LIVE-RANGE ROUTE IS EMPTY, which is the precondition the Makefile's
 * FIXEDR7_CFLAGS caution demands before reaching for a -ffixed row. With four
 * quantities and shortest-lived-first over {3,2,1,0,4,...}, the three non-`p`
 * quantities can only occupy r3, r2, r1 -- (r1, r2, r4) is unreachable by ANY
 * permutation of their live ranges, because base can only be pushed past r1 if
 * a FOURTH competitor exists. No source-level spelling supplies one: an extra
 * quantity needs an extra instruction, and a plain source-level copy is deleted
 * by cprop before flow measures it (brief, and re-confirmed here -- 5 of 5
 * declaration-order and statement-order permutations in v4/ were EXACTLY inert,
 * all 5 of 6).
 *
 * 3. AND -ffixed-r3 IS REFUTED AS A BUILD FLAG BY gfree.
 *
 * This is the half that closes it. The batch-265 pool argument proves gfree and
 * free share ONE object: the single pool word at 0x08002dfc is reached from
 * 0x08002dd8 at pc+32, ACROSS free's entire body, and from 0x08002df0 at pc+8.
 * The pair occupies 0x08002dd8..0x08002e00 exactly = 22 (gfree) + 2 (align) +
 * 12 (free) + 4 (the one word). GAS merges the two `ldr rX, =gPtrs` literals
 * into that one slot, which is why the current hand-written .s assembles to the
 * ROM at all.
 *
 * One object means one flag set. And gfree USES r3 -- `lsr r3, r1, #22`,
 * `and r3, r0`, `str r1, [r3, r4]`. So r3 was NOT reserved in that translation
 * unit, and free's allocation is therefore not gcc's output under -ffixed-r3 or
 * under any other flag.
 *
 * 4. CORPUS CHECK, over ENCODING-INDEPENDENT register use, both directions.
 *    (scratch_elev/b317/A/r4_thumb.py; parses generated .s by
 *    `.type NAME,function` + label + `.LfeN`, NOT by `.thumb_func_start`, which
 *    generated files do not contain -- a first pass that used the hand-written
 *    marker silently measured 137 functions instead of 4,807 and would have
 *    reported a vacuous "0 of 4,469".)
 *
 *    GENERATED thumb : 4,807 functions, 654 use r4, *** 0 use r4 without r3 ***
 *    HAND-WRITTEN thumb: 849 functions, 441 use r4,     2 use r4 without r3
 *                        -- ply_fine (asm/rom_f9000/rom_f95e0.s), ALREADY
 *                           ENROLLED in unmatchable.txt as stock hand-asm m4a
 *                        -- free
 *    HAND-WRITTEN arm  :  51 functions,  40 use r4,     3 use r4 without r3
 *                        -- Func_800a958/960/968, asm/rom_9000/rom_92b8.s, the
 *                           ARM-mode `add pc, r2, lsl #2` / `strlsb` text
 *                           routines, hand-written on their face.
 *
 *    654 generated functions reach r4 and every single one of them also uses
 *    r3, which is the local-alloc walk order made visible at corpus scale. free
 *    sits in the hand-written column with an already-accepted member of the
 *    same class for a positive control.
 *
 * 5. NO SPLIT IS NEEDED, AND THE SPLIT WOULD NOT HAVE WORKED ANYWAY.
 *    Recorded because it was the assignment, and because it is the reason the
 *    pair cannot be taken apart even one function at a time:
 *      datacheck.py asm/rom_c0/rom_2dd8.s -> no data section, 2 functions.
 *      A split gives gfree its own 24-byte object (22 + 2 align). Its own
 *      `ldr r4, =gPtrs` would then pool at offset 24, encoding 4c05; the ROM
 *      has 4c08 (pc+32), a word that after the split lives in free's object.
 *      gfree is byte-recoverable after a split ONLY by hand-writing a bare
 *      `ldr r4, [pc, #32]` with no relocation and relying on free's TU landing
 *      immediately after -- i.e. by making gfree's correctness depend on its
 *      neighbour's layout. That is not worth doing for a function that is
 *      already enrolled in unmatchable.txt, and it is moot now that free is
 *      unmatchable on its own evidence.
 *
 * ---------------------------------------------------------------------------
 * ASK: enroll `free` in unmatchable.txt. Suggested row body --
 *   free  thumb-regalloc  FreeScratch at 0x08002df0, 6 instructions. Body is
 *   reached exactly in order and operand shape (6 of 6 instructions, size and
 *   relocations exact) and differs only in hard-register assignment, which
 *   reduces to one condition: r3 unavailable. local-alloc is the only allocator
 *   that runs (branchless, greg reports "0 regs to allocate") and it walks
 *   3,2,1,0,4,... shortest-lived-first, so the ROM's (index r2, mask r1, base
 *   r4) needs a fourth competitor no source-level spelling can supply -- an
 *   extra quantity costs an extra instruction and a plain copy dies in cprop.
 *   -ffixed-r3 matches this body EXACTLY, and is refuted as a build flag
 *   because gfree -- proved by the shared pool word at 0x08002dfc to be in the
 *   SAME object -- uses r3 three times. Corpus: 0 of 4,807 generated thumb
 *   functions use r4 without r3 (654 use r4); the only hand-written thumb
 *   companions are free and ply_fine, already enrolled.
 *
 * The C below is the best body and should be kept as documentation: it is a
 * correct reading of FreeScratch and it is 6 of 6 instructions with exact size
 * and relocations.
 */
extern void *gPtrs[];

/* FreeScratch -- rewinds whichever arena the pointer belongs to back to it.
 * Picks the arena by (ptr >> 22) & 4, so an EWRAM 0x2xxxxxx pointer selects
 * [gPtrs + 0] and an IWRAM 0x3xxxxxx pointer selects [gPtrs + 4]. Everything
 * allocated after the block is silently reclaimed, so callers must free in
 * reverse allocation order. */
void free(void *p)
{
    unsigned int base;
    unsigned int mask;
    unsigned int off;

    base = (unsigned int)gPtrs;
    mask = 4;
    off = ((unsigned int)p >> 22) & mask;
    *(void **)(off + base) = p;
}
