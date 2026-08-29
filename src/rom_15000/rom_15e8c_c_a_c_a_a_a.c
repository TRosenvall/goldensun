/* Cluster Func_80173ac..Func_80173ac extracted from
 * goldensun/asm/rom_15000/rom_15e8c_c_a_c_a_a.s.
 *
 * Total .text for this TU = 46 bytes (= 0x2e).
 * Placed in the run in goldensun/stage1.ld.
 *
 * Seeds five halfwords of the text/window block: the colour at +0xeae, a width
 * at +0xea8, a count at +0x12b0, a flag cleared at +0xeac and one set at +0xeaa.
 *
 * THE OFFSETS HAVE TO LIVE IN THE TYPE, NOT IN THE EXPRESSION. Written as
 * `*(short *)(p + 0xeae) = 0xf;` this fails twice over:
 *
 *   the constant is POOLED AS A HALFWORD -- `ldrh r3, .L0` -- because the store
 *   is to a `short`, where the ROM has `mov r3, #0xf`. Assigning through an int
 *   local fixes that much (docs/elevation.md, batch 71's narrow-constant note);
 *
 *   and then gcc DERIVES each offset from the last, `sub r0, #0x6` to get from
 *   0xeae to 0xea8, where the ROM loads each one from the pool. That is the
 *   constant-derivation peephole, and no arrangement of int locals stops it.
 *
 * Declared as struct members at their real offsets, both problems disappear at
 * once: each member's address is generated independently, so there is nothing
 * to derive from, and the stored constants come out as immediates. Exact on
 * the first screen.
 *
 * That is the same lever as the gState reads in batch 72, doing a second job.
 * Putting an offset in the TYPE rather than in the arithmetic changes what gcc
 * has to work with, not just how it is spelled.
 */

typedef struct {
    unsigned char pad[0xea8];
    short f_ea8;
    short f_eaa;
    short f_eac;
    short f_eae;
    unsigned char pad2[0x12b0 - 0xeb0];
    short f_12b0;
} Blk;

extern Blk *iwram_3001e8c;

void Func_80173ac(void)
{
    Blk *p;

    p = iwram_3001e8c;
    p->f_eae = 0xf;
    p->f_ea8 = 0xa;
    p->f_12b0 = 9;
    p->f_eac = 0;
    p->f_eaa = 1;
}
