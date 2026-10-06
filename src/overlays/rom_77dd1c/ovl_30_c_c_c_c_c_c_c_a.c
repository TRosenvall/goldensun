/* OvlFunc_882_200c5b8 -- 0x0200c5b8  --  *** BYTE-EXACT.  NOT A PARK. ***
 *
 * MATCHING, 0 differing encodings of 31.  objcmp.py under the production flags:
 *   OK OvlFunc_882_200c5b8 -- 68 bytes, 31 encodings and 3 relocations identical
 *   OK whole file -- 68 bytes, 31 encodings and 3 relocations identical
 * SIZE EXACT (68 against 68).  RELOCATIONS EXACT (3 THM_CALLs to
 * __MapActor_GetActor at the same offsets).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_a.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_a.s --func OvlFunc_882_200c5b8
 *
 * SPLIT SHAPE: NONE NEEDED.  split_s.py --dry-run says it outright --
 * "asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_a.s holds only
 * OvlFunc_882_200c5b8 and no data; convert it directly, no split needed" -- and
 * datacheck.py reports nothing, so `exports` is empty and the linker script does
 * not move.
 *
 * PINS: 0.  RESIDUE: none.
 *
 * NOT ONE OF BRIEF D's THREE TARGETS.  It was found while reading batch 330's
 * own park for it, which is a sibling of OvlFunc_888_2008848 in the same class,
 * and the lever that landed that one lands this one unchanged on the first try.
 *
 * THE LEVER: the mask is a BITFIELD, not an `&`.  Written as an explicit
 * `(0xc & p->flags) | (m & q->flags)`, cse1 unifies the two `(const_int 12)`
 * loads into one pseudo that must live across a call, giving THREE
 * call-crossing values where the ROM has two; with `-fcall-used-r4`
 * (Makefile:131-134) r4 cannot hold one, so the third goes to r8 and costs a
 * prologue pair, an epilogue pair and a `mov` down into a low register at each
 * use.  Declared as a 2-bit bitfield and assigned directly, the mask is built
 * inside store_bit_field instead and is NOT unified -- each block gets its own
 * `mov rN, #0xc`, the `-0xd` stays shared exactly as the ROM has it, and the
 * allocation collapses onto the ROM's.
 *
 * WHAT BATCH 330's PARK HAD RIGHT, AND WHERE IT STOPPED.  Its arithmetic was
 * exact and its dump evidence stands: one basic block, ";; 0 regs to allocate:"
 * in .18.greg so every pseudo is local-alloc's, three call-crossing pseudos
 * named in .17.lreg.  What it closed wrongly was the DIMENSION: it recorded ten
 * spellings of the mask as inert and wrote "spelling the constant is now a
 * closed dimension: ten attempts, zero movement", then named r7's availability
 * as the one unvaried dimension and the only route left.  Spelling was indeed
 * closed -- but the construct that GENERATES the constant was never varied, and
 * that is the one that moves.  The r7 question is now moot for this function.
 *
 * Also retired with it: the park's reading that this function "is a fakematch
 * and the local-alloc argument above is the final word" unless r7 turns out
 * allocatable.  It is neither -- it lands pin-free.
 */
struct Spr {
    unsigned char pad_00[9];
    unsigned char f9_lo : 2;
    unsigned char f9_sel : 2;
    unsigned char f9_hi : 4;
};

extern void *__MapActor_GetActor(int slot);

#define SPRITE_OF(n) (*(struct Spr **)((unsigned char *)__MapActor_GetActor(n) + 0x50))

/* Copies the player's two-bit selector onto the sprites of actors 0x16 and 8. */
void OvlFunc_882_200c5b8(void)
{
    struct Spr *p;

    p = SPRITE_OF(0);
    SPRITE_OF(0x16)->f9_sel = p->f9_sel;
    SPRITE_OF(8)->f9_sel = p->f9_sel;
}
