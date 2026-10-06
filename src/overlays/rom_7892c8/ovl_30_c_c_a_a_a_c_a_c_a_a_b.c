/* OvlFunc_888_2008848 -- 0x02008848  --  *** BYTE-EXACT.  NOT A PARK. ***
 *
 * MATCHING, 0 differing encodings of 32.  objcmp.py under the production flags:
 *   OK OvlFunc_888_2008848 -- 68 bytes, 32 encodings and 2 relocations identical
 * and with --whole against the post-split reference:
 *   OK whole file -- 68 bytes, 32 encodings and 2 relocations identical
 * SIZE EXACT (68 against 68).  RELOCATIONS EXACT (2 THM_CALLs to
 * __MapActor_GetActor at the same offsets).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a_b.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a_b.s \
 *     --func OvlFunc_888_2008848
 * (that is the path AFTER the split below; before the split, point the recipe at
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a.s, which is how it was
 * measured in scratch.)
 *
 * SPLIT SHAPE.  datacheck.py on
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a.s reports NOTHING -- no
 * data sections and no local labels needing promotion, so NO `.global` pre-pass
 * and `exports` is empty.  split_s.py --dry-run:
 *   would write ..._a_a_a.s  (1 function, 106 lines)
 *   would write ..._a_a_b.s  (1 function, 37 lines)   <-- this function
 *   would write ..._a_a_c.s  (1 function, 1553 lines)
 *   would REMOVE ..._a_a.s and rewrite overlays/rom_7892c8/overlay.ld
 *
 * PINS: 0.  No inline asm, no `register ... __asm__`, no per-file flag group.
 * RESIDUE: none.
 *
 * THE LEVER, AND IT GENERALISES -- THE MASK IS A BITFIELD, NOT AN `&`.
 *
 * The ROM merges a two-bit field at bit 2 of one sprite byte into another.
 * Written in C as an explicit mask -- `d[9] = (d[9] & -0xd) | (s[9] & 0xc)` --
 * gcc puts BOTH constants in registers that live across the second call, because
 * cse1 unifies the two `(const_int 12)` loads: `.00.rtl` has two
 * `(set (reg) (const_int 12))` insns and `.03.cse` has one, with the second
 * block's `and` rewritten to use the surviving pseudo.  That makes THREE
 * call-crossing values where the ROM has two, and since the tree builds with
 * `-fcall-used-r4` (Makefile:131-134) r4 cannot hold a value across a call, so
 * the third goes to r8 -- a `mov`+`push` prologue pair, a `pop`+`mov` epilogue
 * pair, and a `mov r3, r8` before each use.  Six instructions.
 *
 * Declaring the field as a BITFIELD and assigning it directly makes gcc build
 * the mask inside store_bit_field instead, and THAT constant is NOT unified:
 * the second block gets its own `mov rN, #0xc`, the third value never crosses a
 * call, and the allocation collapses onto the ROM's.  The `-0xd` stays shared,
 * exactly as the ROM has it (`mov r5, #0xd / neg r5, r5` once).
 *
 * HOW IT WAS FOUND, and the corpus number that found it.  Over the 4,483
 * gcc-generated `.s` files in asm/, scanning for a `mov rX, #imm` that
 * immediately feeds a two-address `and`/`orr`/`eor`/`bic` and occurs TWICE with
 * the same immediate inside one straight-line region (no label and no branch
 * between; a `bl` does not end a cse extended basic block) gives EXACTLY ONE
 * hit: OvlFunc_947_2009aa8 in asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_b.s,
 * with the SAME constant 0xc.  Its source
 * (src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_b.c) spells the field as a
 * 2-bit bitfield `f9_sel`, and the asm it emits is this function's reference
 * block for block.  One corpus hit, and it was the recipe.
 *
 * The second edit that mattered: NOT NAMING the actor returned by the call.
 * With `b = __MapActor_GetActor(0);` gcc keeps a pseudo for `b` and reload emits
 * `mov r3, r0` before each `ldr` -- two instructions, which is the whole
 * remaining gap once the bitfield is in place.  Used inline, the call's return
 * register IS the base register.
 *
 * The park this retires (src/non_matching/ovl_7892c8/2008848.c) had declared the
 * class closed: "Not reachable", with five negatives all of which varied the
 * SPELLING of the mask (per-site `int` locals, the -0xd created at first use,
 * separate pointer locals, --no-rerun-cse, -fno-gcse).  Spelling was the wrong
 * dimension; the CONSTRUCT that generates the mask was the right one.  Its
 * header's figure also exceeded its own function -- the eighteen-park class
 * batch 330 brief C diagnosed.  Done exactly, the reference is 29 sixteen-bit
 * encodings + 2 `bl` + 0 literal pool words + 1 alignment pad = 31 instructions
 * in 68 bytes, and the park body was 35 + 2 + 0 + 1 = 37.  The gap was SIX
 * INSTRUCTIONS of length, with pool and padding identical on both sides.
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9_lo : 2;
    unsigned char f9_sel : 2;
    unsigned char f9_hi : 4;
    unsigned char pada[11];
    unsigned char f15_lo : 2;
    unsigned char f15_sel : 2;
    unsigned char f15_hi : 4;
};

struct Actor {
    unsigned char pad00[0x50];
    struct Sub *f50;
};

extern struct Actor *__MapActor_GetActor(int slot);

/* Copies the player's two-bit selector into the entity's own byte and into the
 * second copy of it eleven bytes later.  Returns 0; no caller checks.
 */
int OvlFunc_888_2008848(struct Actor *a)
{
    a->f50->f9_sel = __MapActor_GetActor(0)->f50->f9_sel;
    a->f50->f15_sel = __MapActor_GetActor(0)->f50->f9_sel;
    return 0;
}
