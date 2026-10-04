/* Func_801b9a8 -- 0x0801b9a8, split out of
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c.s, which holds THREE functions:
 * Func_801b810 (stays in _a.s), Func_801b9a8 (this, _b) and Func_801b9ec
 * (stays in _c.s; see p4_candidate.c, which matches it too).
 *
 * Split shape -- tools/datacheck.py on the source .s exits 0 (no data
 * section), and tools/split_s.py --dry-run reports:
 *   would write asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_a.s  (1 function, 207 lines)
 *   would write asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_b.s  (1 function, 40 lines)
 *   would write asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_c.s  (1 function, 42 lines)
 *   would REMOVE asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c.s, rewrite stage1.ld
 * No exports needed.
 *
 * MATCHES to ONE encoding, and that one is the `_CONST_1f` POOL WORD, which
 * the link resolves: 32 of 32 instructions, SIZE EXACT, the only difference
 * `ref 0000001f / ours 00000000` at index 31 with an extra
 * `R_ARM_ABS32 _CONST_1f` relocation. That is const.sym's documented PHANTOM
 * relocation class -- the reference is a DISASSEMBLY, so a symbol in the
 * original was baked to its value before the .s was written and the assembled
 * reference cannot carry a relocation either way. `make compare` is the
 * authority. NO PINS, no shim, no fakematch row, no per-file flag, and NO NEW
 * SYMBOL: `_CONST_1f = 0x1f;` is already in const.sym (INCLUDEd by
 * stage1.ld:18) and is marked UNVALIDATED there. THIS VALIDATES IT.
 *
 * Verify with (AFTER the split):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_b.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_b.s --whole
 * Before the split, against the three-function reference:
 *   ... python3 tools/objcmp.py <this file> \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c.s --func Func_801b9a8
 *
 * Walks n links down a list from the field at +0xd2*4, and if the node's type
 * is 1 or 6 loads a UI icon with two stack out-parameters and a stack fifth
 * argument.
 *
 * ================== THE PARK'S HEADER, CORRECTED TWICE ==================
 *
 * The park (src/non_matching/rom_15000/801b9a8.c) read 19 of 32 with the COUNT
 * ONE SHORT, so its figure measured misalignment. Its first line cites
 * `asm/rom_15000/rom_1aeec_a_a_c_a_c_c.s`, which no longer exists -- only the
 * prose drifted; its `Verify with:` recipe already named the right file.
 *
 * ITS OBSERVATIONS ALL REPRODUCED. Its VERDICT -- "guessing a name would be
 * inventing source, so it is left open deliberately" -- is REFUTED, because
 * the name is not a guess and does not have to be invented: it is already in
 * const.sym, and the bytes force it by a mechanism STRONGER than the one
 * const.sym's header rests on.
 *
 * ***** THE NEW STRUCTURAL ARGUMENT: gcc WOULD HAVE WRITTEN `add`. *****
 *
 * const.sym's standing bar is "gcc-2.96 never pools a constant it can build
 * with an eight-bit mov". For 0x1f that is true but it is not the strongest
 * thing the bytes say. Measured here, with a pool-forcing LITERAL used as an
 * instrument:
 *
 *     - 0x101   (a pooled LITERAL)   ldr r3,[pc,#20] / adds r0,r0,r3
 *                                    .word 0xfffffeff      <-- NEGATED
 *     - (int)&_CONST_1f (a SYMBOL)   ldr r3,[pc,#20] / subs r0,r0,r3
 *                                    .word _CONST_1f
 *     ROM                            ldr r3,[pc,#24] / subs r0,r0,r3
 *                                    .word 0x0000001f      <-- POSITIVE
 *
 * gcc-2.96 NEGATES a pooled const_int subtrahend and emits `add`. It can only
 * emit a genuine `sub rd,rn,rm` against a pool word when the operand is
 * UNFOLDABLE -- i.e. a symbol_ref. So the ROM's `sub` against a POSITIVE pool
 * word is structurally impossible from any literal spelling, by an argument
 * that does not depend on the value being small. THE SIGN OF THE POOL WORD IS
 * THE EVIDENCE. This belongs in const.sym as a second criterion-1 test, and it
 * is checkable on every existing entry.
 *
 * THE HALFWORD EXCEPTION WAS CHECKED FIRST, as const.sym requires, and does
 * NOT apply: the subtraction's result is an SImode CALL ARGUMENT. Every HImode
 * spelling is WORSE, measured -- see the table below.
 *
 * ================ AND THE PARK HELD HALF OF A TWO-PART FIX ================
 *
 * The symbol alone reads 9. The park's own rejected negative -- "icon id named
 * in a local computed BEFORE the 0xc read: 34 lines, 20 differ" -- is the other
 * half. Crossed:
 *
 *     park base (literal 0x1f, id inline)            19   31 insns (SHORT)
 *     id-local only, literal 0x1f                    20   short   (park's row)
 *     (int)&_CONST_1f only, id inline                 9   32/32, size exact
 *     (int)&_CONST_1f + id-local computed first       1   32/32, size exact
 *     instrument 0x101 + id-local                    14   32/32  <-- MISLEADING
 *
 * Read the last row. **A pool-forcing literal is a usable instrument for
 * ALIGNMENT and an actively misleading one for SCREENING the second edit**:
 * with the negated `add` the id-local measures 14, worse than 9; with the real
 * `sub` it measures 1. The add/sub difference moves both the sched2 schedule
 * and the allocation.
 *
 * WHY THE ID-LOCAL CLOSES THE REGISTER CAUSE TOO. The park also recorded "the
 * ROM keeps the walked node in r2 and leaves r0 free for the icon id; ours
 * reuses r0 for the node. Allocation, not spelling." That is a CONSEQUENCE,
 * not a separate blocker. Computing the id FIRST keeps `node` live ACROSS the
 * id's definition, so node can no longer share r0 with the id and is pushed to
 * r2 -- the ROM's assignment. One edit, both causes. Indices 5, 9 and 12 close
 * with index 17 onward.
 *
 * ========================= MEASURED INERT / WORSE =========================
 *   *(u16*)(node+0x20) + -0x1f                    19  exactly inert (combine folds)
 *   (unsigned short)(x - 0x1f)                    21  +4 bytes (adds the extend)
 *   `unsigned short h; h = x; h -= 0x1f;`         21  +4 bytes (same)
 *   *(short*)(node+0x20) - 0x1f                   10  WRONG PROGRAM -- `ldrsh`
 *                                                     needs a zero register, so
 *                                                     the count matches by
 *                                                     accident and it sign-extends
 *   unsigned short *node, node[5]/node[6]/node[0x10]  9  exactly inert
 *   `t` inlined into both type comparisons        9   exactly inert
 *   `for (; n != 0; n--)` for the `while`         9   exactly inert
 *   early `return` instead of the `if` arm        9   exactly inert
 */
extern void LoadOldUIIcon(int id, int b, int *x, int *y, int flag);
extern int _CONST_1f;

void Func_801b9a8(char *p, int n)
{
    char *node;
    int a;
    int b;
    int t;
    int id;

    node = *(char **)(p + 0xd2 * 4);
    while (n != 0) {
        n--;
        node = *(char **)(node + 4);
    }
    t = *(unsigned short *)(node + 0xa);
    if (t == 1 || t == 6) {
        id = *(unsigned short *)(node + 0x20) - (int)&_CONST_1f;
        a = *(unsigned short *)(node + 0xc);
        LoadOldUIIcon(id, 0, &a, &b, 1);
    }
}
