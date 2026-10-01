/* Func_801d94c  @  0x0801d94c  [rom_15000]   *** LANDS -- BYTE-IDENTICAL ***
 *
 * Source asm: goldensun/asm/rom_15000/rom_1ca1c_c_c_a_a.s
 *   (the park src/non_matching/rom_15000/rom_1d94c.c cites
 *    asm/rom_15000/rom_1ca1c_c_c_a.s, which no longer exists -- the file has
 *    been split once more since that park was written.)
 *
 * FIGURE: 0.  objcmp --func:
 *   OK Func_801d94c -- 52 bytes, 21 encodings and 3 relocations identical
 *
 * Verify with (AFTER the split below is installed; recipe names the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_15000/rom_1ca1c_c_c_a_a_b.c \
 *     asm/rom_15000/rom_1ca1c_c_c_a_a_b.s --whole
 *
 * Verify BEFORE the split (what was actually run to produce the figure above):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b317/B/p2_candidate.c \
 *     asm/rom_15000/rom_1ca1c_c_c_a_a.s --func Func_801d94c
 *
 * SPLIT SHAPE -- REQUIRED. The .s holds three functions.
 *   python3 tools/datacheck.py asm/rom_15000/rom_1ca1c_c_c_a_a.s
 *     -> clean (rc=0, no output): CODE ONLY, no data section to preserve.
 *   python3 tools/split_s.py asm/rom_15000/rom_1ca1c_c_c_a_a.s Func_801d94c --dry-run
 *     [dry-run] would write asm/rom_15000/rom_1ca1c_c_c_a_a_a.s  (2 function(s), 902 lines)
 *     [dry-run] would write asm/rom_15000/rom_1ca1c_c_c_a_a_b.s  (1 function(s), 23 lines)
 *     [dry-run] would REMOVE asm/rom_15000/rom_1ca1c_c_c_a_a.s
 *     [dry-run] would rewrite stage1.ld
 *   INSTALL PATH: src/rom_15000/rom_1ca1c_c_c_a_a_b.c
 *   Verify `make compare` is green AFTER the split and BEFORE this .c is added.
 *
 * PIN COUNT: 0.  No inline asm, no __asm__ register bindings, no asm labels.
 * fakematch.txt row: NOT NEEDED -- this is a true match.
 *
 * REPOINT THE SIBLINGS. Landing this invalidates the `Verify with:` recipe of
 * every other park in the old .s. After the split, repoint to
 * asm/rom_15000/rom_1ca1c_c_c_a_a_a.s:
 *   src/non_matching/rom_15000/801d108.c      (Func_801d108)
 *   src/non_matching/rom_15000/Menu_Settings.c (Menu_Settings)
 *
 * ---------------------------------------------------------------------------
 * WHAT THE PARK CLAIMED, AND WHY ALL OF IT IS WRONG
 *
 * The park claimed "5 instructions in disagreeing regions, of 17" and named TWO
 * blockers, both "the unreachable variety". The measured figure for its own body
 * is 19 of 21 encodings (ours 19), 48 bytes against 52, with RELOCDIFF -- so its
 * number was not a distance at all. Both blockers are gone with ONE change, and
 * the park's single "this part is right" sentence was the actual blocker.
 *
 * THE MECHANISM: A VARIABLE OFFSET IS A LEGAL THUMB ADDRESSING MODE; A CONSTANT
 * ONE IS NOT.
 *
 * The park carried the byte offsets in a reused `unsigned int off`. That makes
 * the halfword address a (plus reg reg), which IS the Thumb-1 register-offset
 * load mode, so gcc folds address and load into one instruction:
 *
 *     ours   ldrh r3, [r5, r2]
 *     rom    add  r3, r5, r2  /  ldrh r3, [r3, #0]
 *
 * With the offsets as CONSTANTS (struct members), 0x574 is far outside the ldrh
 * immediate range, the address has no legal mode, and gcc must compute it into
 * its own register -- which is exactly the ROM's two instructions. The park's
 * diagnosis ("name the intermediate pointer; gcc folds it back because `p` has
 * exactly one use") was reading the wrong cause: the fold is driven by the
 * ADDRESSING MODE of a reg+reg plus, not by a named pointer's use count. Naming
 * the pointer cannot help, because the folded form is legal either way.
 *
 * The park's FIRST blocker -- the pool load hoisted above the dereference,
 *
 *     rom    ldr r3, =iwram / ldr r2, =0x5a4 / ldr r5, [r3] / add r0, r5, r2
 *     ours   ldr r3, =iwram / ldr r5, [r3]   / ldr r3, =0x5a4 / add r0, r5, r3
 *
 * -- which the park argued was unreachable via update_equiv_regs because "this
 * function has no branches at all, so REG_BASIC_BLOCK (regno) < 0 cannot hold"
 * -- ALSO disappears with constant offsets, and never involved
 * update_equiv_regs. With a constant, the pool load is a separate pseudo born at
 * expand time ahead of the add, so its live range overlaps the iwram-address
 * pseudo and it gets its own register (r2). With a variable the two never
 * overlap and r3 is simply reused. One cause, two symptoms.
 *
 * THIRD CLAIM ALSO REFUTED. The park asserted: "The offset variable IS correctly
 * reused -- `off = 0x574` then `off += 0x9c` reproduces the ROM's
 * `ldr r2, =0x574 ... add r2, #0x9c` -- so that part of the shape is right and
 * is not what fails." Wrong twice over. That register reuse is GCC'S OWN cse of
 * related constants (it materialises 0x610 as 0x574 + 0x9c because 0x574 is
 * already live in a register), not a source-level variable -- and the source
 * variable that appeared to produce it was the thing blocking the function.
 *
 * CROSSED CANDIDATES (tools/sweep_variants.py, one container):
 *   BASE (park body, `off` variable)          19  first=1   dsize=-4  RELOCDIFF
 *   literal constants on a u8* base            9  first=9   dsize=0   RELOCDIFF
 *   literal constants with (idx << 2)         17  first=1   dsize=+4  RELOCDIFF
 *   literal constants, ((void**)(b+0x610))[i]  9  first=9   dsize=0   RELOCDIFF
 *   struct members (this body)                 0  first=-1  dsize=0   ok
 * The literal-constant forms take 10 of the 19 off at once and fix the
 * instruction count -- that is what identified the mechanism. The remaining 9
 * were the base being an `unsigned char *` instead of the struct pointer.
 * NOTE: `(idx << 2)` is a REGRESSION against `idx * 4` here (+4 bytes); the
 * shift spelling is not a free substitution.
 *
 * LAYOUT. The three padding/array bounds are written as differences of the ROM's
 * own byte offsets so the three constants 0x574, 0x5a4 and 0x610 are visible and
 * checkable. `tab` is sized 8 only because nothing reads past it; its length is
 * not load-bearing for the match (only the 0x610 start is).
 */
struct W {
    unsigned char pad_000[0x574];
    unsigned short cur;
    unsigned char pad_576[0x5a4 - 0x576];
    unsigned char blk[0x610 - 0x5a4];
    void *tab[8];
};

extern struct W *iwram_3001ea0;
extern void _Func_80b08b8(void *p);
extern void Func_80217a4(void *p);

void Func_801d94c(void)
{
    struct W *w = iwram_3001ea0;

    _Func_80b08b8(w->blk);
    Func_80217a4(w->tab[w->cur]);
}
