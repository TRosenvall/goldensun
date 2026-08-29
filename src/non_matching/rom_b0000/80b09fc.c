/* Func_80b09fc (InitListRecord) -- 0x080b09fc,
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s
 *
 * 16 vs 14 lines, 12 differing.  Candidate at scratch/Lb09fc.c.
 * Every store is in the ROM's order and to the right offset; what differs is
 * two things, both about the constant 0.
 *
 *   - The ROM loads the zero FROM THE POOL: `ldr r6, =0x0`, where ours emits
 *     `mov r5, #0x0`.  gcc-2.96 does sometimes pool a zero -- Func_80a3ddc
 *     reproduces `ldr r2, =0` from a plain `0` -- but nothing here provokes it.
 *   - Because the ROM has a pool it also has `b L0 / L0:` branching over it,
 *     which is the two-line deficit.
 *
 * So the whole difference is downstream of whether the TU emits an inline pool,
 * and that is decided by the zero's spelling, which I could not reach.  The
 * register roles (r5 for the list pointer, r4 for the loaded halfword) are
 * swapped with ours as a consequence.
 */

/* ---- MERGED from src/non_matching/rom_b0000/rom_b09fc.c ----
 * A second park for the same function under a different naming convention;
 * its analysis is kept verbatim and the duplicate file removed.
 *
 Func_80b09fc  [rom_b0000]
 * Source asm: goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s
 * (path updated: the .s was split or renamed after this was parked)
 *
 * Parked: logic faithful, does NOT byte-match standalone. Registered in
 * unmatchable.txt (class: tu-pool midpool).
 * TODO(residual): two scheduler slots; the pooled-zero load (ldr r6,=0; the
 *   anonymous direct `= 0` HImode/QImode store form DOES pool it, see the
 *   2026-06-11 wave-4 diag) sits one slot late, and the ROM dumps its literal
 *   pool MID-FUNCTION with a skip-branch before the epilogue (b.n; .word 0;
 *   pop) with no loop to anchor the dump. That midpool placement is an
 *   original-TU pool-pressure artifact: a standalone TU always end-pools a
 *   function this small, and the pc-relative offsets then differ. Sibling
 *   Func_80b0a20 (same source file) has the identical b.n-over-pool tail and
 *   will hit the same wall.
 *
 * LEAD, from the batch 25 park audit: this function loads 0 FROM THE POOL --
 * `ldr r6, =0x0` -- where `mov r6, #0` would do. gcc never pools a constant it
 * can build with an eight-bit mov, so that operand was a SYMBOL whose value is
 * zero. Same tell as area.sym. Nothing in the tree defines a zero-valued
 * symbol yet, so this needs a name before it can match.
 */
struct S {
    unsigned int p;
    unsigned short a;
    unsigned short b;
    unsigned short c;
    unsigned short d;
    unsigned char e;
    unsigned char f;
};

/* InitListRecord -- exported
 * r0 = list record, r1 = visible rows, r2 = cursor, r3 = flag.
 * Initialises the controller from the list at [r0]: copies the total count from
 * [list]+6 and the row height from [list]+8, stores the caller's visible count,
 * cursor and flag, and clears +0x0C.
 */
void Func_80b09fc(struct S *arg0, unsigned short arg1, unsigned short arg2, unsigned char arg3)
{
    unsigned short *src;

    src = (unsigned short *)arg0->p;
    arg0->a = src[3];
    arg0->b = src[4];
    arg0->c = arg1;
    arg0->d = arg2;
    arg0->f = arg3;
    arg0->e = 0;
}
 */
