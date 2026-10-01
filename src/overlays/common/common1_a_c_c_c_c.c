/* OvlFunc_common1_16cc  --  asm/overlays/common/common1_a_c_c_c_c.s   [LANDS]
 *
 * BYTE-IDENTICAL.  44 bytes, 21 encodings and 1 relocation identical,
 * --func and --whole both green.
 *
 * INSTALL AT: src/overlays/common/common1_a_c_c_c_c.c
 *   - delete asm/overlays/common/common1_a_c_c_c_c.s (the next build writes a
 *     generated .s to the same path; that generated file belongs in the commit)
 *   - NO linker-script change: the three overlay.ld files that name this stem
 *     (overlays/rom_7db0c8, rom_7ddb88, rom_7e0928) all point at
 *     asm/overlays/common/common1_a_c_c_c_c.o(.text), which is where the build
 *     puts the object for either source form -- same as the already-landed
 *     sibling src/overlays/common/common1_a_c_c_c_b.c.
 *   - NO fakematch.txt row: this is natural C, no inline-asm barrier, no pin.
 *   - delete the park src/non_matching/ovl_common/common1_16cc.c
 *
 * SPLIT SHAPE: none needed.
 *   python3 tools/datacheck.py asm/overlays/common/common1_a_c_c_c_c.s
 *     -> (silent; no data section)
 *   python3 tools/split_s.py asm/overlays/common/common1_a_c_c_c_c.s \
 *        OvlFunc_common1_16cc --dry-run
 *     -> "holds only OvlFunc_common1_16cc and no data; convert it directly,
 *         no split needed"
 *   So this conversion touches NOTHING any other common-overlay object sees,
 *   and it is a pure source-spelling landing with NO per-file flag: it cannot
 *   help or hurt a file-mate.  The ovl_common split question does not arise here.
 *
 * PINS: zero.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/common/common1_a_c_c_c_c.c \
 *     asm/overlays/common/common1_a_c_c_c_c.s --func OvlFunc_common1_16cc
 *   (and the same with --whole)
 *
 * WHAT IT DOES
 * Formats an unsigned int as eight hex digits into buf[0..7], back to front,
 * NUL-terminating at buf[8].  The digit characters come from the 16-byte table
 * exported as `.L6` from asm/overlays/common/common1_c_c_b_c.s.
 *
 * ------------------------------------------------------------------
 * THE PARK'S FIGURE WAS WRONG, AND ITS PROGRAM WAS WRONG
 *
 * The park claimed "21 lines against 21, SIX differing, and all six are r4 and
 * r5 exchanged throughout".  Measured: FIVE of 21 -- and, far worse,
 *
 *     XX RELOCATIONS differ
 *        ref  [['00000028', 'R_ARM_ABS32', '.L6']]
 *        ours [['00000028', 'R_ARM_ABS32', '.text']]
 *
 * The park spells the loop `do { ... } while (i >= 0)`, and for that shape gcc
 * names its own loop label `.L6`.  The table is also named `.L6`.  gas resolves
 * a `.L`-prefixed reference against the LOCAL definition, so the park's pool
 * word `.word .L6` points at its own loop top, not at the digit table.  The
 * park is not five instructions from the ROM; it is a DIFFERENT PROGRAM that
 * indexes its own code.  `make compare` could never have passed it, and the
 * "six r4/r5 lines" were being counted on that.
 *
 * TWO INDEPENDENT EDITS, CROSSED.  Each is necessary; neither is sufficient.
 *
 * (1) `while (i >= 0) { ... }` instead of `do { ... } while (i >= 0)`.
 *     Byte-neutral in the instruction stream (gcc rotates the loop itself), but
 *     it moves gcc's internal label numbering off 6: the `while` shape emits
 *     .L5/.L8/.L9 where the `do` shape emits .L6/.L7/.L8.  With no local `.L6`
 *     the pool word becomes an UNDEFINED reference and the relocation is the
 *     ROM's.  Measured alone: 4 of 21, relocation OK.
 *
 * (2) The mask written INLINE as `v & 0xf`, with no named local for it.
 *     This is the whole r4/r5 story, and it is a LOOP-HOIST ORDER question, not
 *     a statement-order one -- which is why the park's two attempts at moving
 *     the mask assignment earlier both got worse.
 *
 *     Allocation is decided by allocno_compare's floor_log2(R)*R/L.  Off
 *     .17.lreg for the park's shape:
 *         Register 37 (the table, symbol_ref .L6) used 3 times across 18 insns
 *         Register 35 (the mask, const_int 15)    used 3 times across 20 insns
 *     Equal R, so the SHORTER live range wins, and .18.greg allocates
 *     "32 33 34 37 35" -- the table first, so the table takes r4 (r3 is the
 *     scratch) and the mask is pushed to r5.  The ROM is the other way round,
 *     so in the ROM the MASK had the shorter range.
 *
 *     With a named `m = 0xf` outside the loop, the mask's `mov` sits at insn 23
 *     while loop.c hoists the table load with emit_insn_before(loop_start) to
 *     insn 87 -- i.e. the hoisted table is born LATER than the hand-placed mask,
 *     and no reordering of source statements can beat a hoist, because the hoist
 *     always lands immediately before the loop.
 *
 *     Write the mask inline and it becomes a loop invariant too, so BOTH are
 *     hoisted and the hoist order decides.  loop.c emits in body-scan order, and
 *     expanding `L6[v & 0xf]` evaluates the ARRAY BASE before the INDEX, so the
 *     table is scanned first and hoisted first.  The mask is now born after the
 *     table, has the shorter live range, wins the priority compare, is allocated
 *     first and takes r4.  Exactly the ROM.
 *
 * THE CROSS (figures are ndiff of 21, reloc state in brackets):
 *     do-while + named mask   (the park) 5  [RELOCDIFF]
 *     while    + named mask              4  [ok]
 *     do-while + inline mask             1  [RELOCDIFF]   <- instruction-exact,
 *                                           only the pool word differs, and it
 *                                           differs because .L6 was captured
 *     while    + inline mask             0  [ok]          <- LANDS
 * The do-while+inline row is the clean proof that the two edits are orthogonal
 * and that the park's residue was two unrelated faults wearing one figure.
 *
 * ALSO BYTE-IDENTICAL (equivalent, kept out of the way): the same `while` loop
 * with a named mask local AND the table copied into a local pointer declared as
 * the first statement -- lengthening the table's live range instead of
 * shortening the mask's reaches the same priority order.  The inline form below
 * is preferred: fewer locals, and it is what the ROM's codegen implies.
 *
 * SETTLED AND KEPT FROM THE PARK: the buffer pointer is advanced IN PLACE
 * (`buf += 8; *buf = 0;`), not indexed.  Written `p = buf + 8; *p = 0;` gcc
 * folds it to `strb r3, [r0, #8]` and the ROM's `add r0, #8` disappears.
 */

extern unsigned char L6[] __asm__(".L6");

void OvlFunc_common1_16cc(char *buf, unsigned int v)
{
    int i;

    buf += 8;
    *buf = 0;
    i = 7;
    buf--;
    while (i >= 0) {
        *buf = L6[v & 0xf];
        i--;
        v >>= 4;
        buf--;
    }
}
