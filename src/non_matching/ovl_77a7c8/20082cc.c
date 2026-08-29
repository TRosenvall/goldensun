/* OvlFunc_881_20082cc  [ovl_77a7c8]
 *
 * Source asm: goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c.s
 *
 * NOT SPLIT. The .s still holds its other functions.
 *
 * Copies a halfword out of the block at iwram_3001e70 into a sprite field and
 * clears a byte next to it.
 *
 * THE PARK WAS WRONG TWICE AND IS NOW RIGHT, going from 13 differing positions
 * to 10 and from 10 instructions to the ROM's 13. It had been filed as
 * "reg-alloc/scheduling divergence; logic correct. Permuter seed."
 *
 *  1. IT TOOK THE ADDRESS INSTEAD OF DEREFERENCING. The C read
 *     `(char *)&iwram_3001e70 + (0x8d << 1)`; the ROM does
 *     `ldr r3, =iwram_3001e70 / ldr r2, [r3]` -- the symbol holds a POINTER and
 *     the offset applies to what it points at. Wrong by one level of
 *     indirection, which is a behavioural bug, not a scheduling one.
 *
 *  2. THE HALFWORD WENT INTO A `unsigned short` LOCAL, which emits `ldrsh` plus
 *     a zero-extend where the ROM has a plain `ldrh`. Reading it into an `int`
 *     fixes that -- the rule is already in docs/elevation.md and was not
 *     applied here.
 *
 * FOUND BY tools/audit_parks.py, which now flags parks where a differing
 * operand is a VALUE or a NAME rather than a register. This one showed
 * `=iwram_3001e70+282` against the ROM's `=0x3001e70`, which is exactly what a
 * missing dereference looks like.
 *
 * WHAT REMAINS: a POOLED ZERO. The ROM loads 0 from the literal pool --
 *
 *     ldr r1, .L2e8   @ 0      ... strb r1, [r3]
 *
 * -- where `mov r1, #0` would encode. gcc never pools a constant it can build
 * with an eight-bit mov, so that operand was a SYMBOL whose value is zero.
 *
 * SECOND MEMBER OF THAT CLASS. src/non_matching/rom_b0000/rom_b09fc.c has the
 * same tell (`ldr r6, =0x0`). Nothing in the tree defines a zero-valued symbol,
 * and unlike the area ids there is no second signal saying what it would be --
 * area.sym was adopted on the strength of 121 of 122 ids appearing in exactly
 * one overlay each, and there is no comparable structure here. So this needs
 * either a name from someone who knows the codebase or another instance that
 * disambiguates it.
 *
 * The rest of the diff is genuine register allocation and is downstream of the
 * pooled zero -- gcc spends a register on `mov r2, #0` that the ROM spends on
 * the pool load.
 */
extern unsigned int iwram_3001e70;

unsigned int OvlFunc_881_20082cc(unsigned char *p)
{
    unsigned char *base;
    unsigned int off;
    int v;
    unsigned char *q;

    base = (unsigned char *)iwram_3001e70;
    off = 0x8d;
    off <<= 1;
    base += off;
    v = *(unsigned short *)base;
    q = *(unsigned char **)(p + 0x50);
    *(unsigned short *)(q + 0x1e) = v;
    *(q + 0x26) = 0;
    return 1;
}
