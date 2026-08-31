/* Func_80c1f50 -- asm/rom_b5000/rom_c1a34_a_a_a.s
 *
 * BLOCKER: STRENGTH REDUCTION creates an induction pointer the ROM does not
 * have. 29 of 46, and OURS IS THREE LINES LONG -- the three extra lines ARE
 * the pointer.
 *
 * Scans units 0x80..0x85 for one whose byte at +0x12a is 1 and whose byte at
 * +0x128 matches the argument, then writes 0x31 into the first free slot of a
 * short list at the unit base and re-terminates it.
 *
 * The outer scan, both guards, the two-store fast path and the epilogue all
 * reproduce. The inner free-slot search does not:
 *
 *     rom    add r0, #1 / cmp r0, #0xd / bgt / ldrb r1, [r2, r0] / cmp / bne
 *     ours   mov r3, r1 (cursor init, hoisted)
 *            add r0, #1 / cmp r0, #0xd / bgt / add r3, #1 / ldrb r2, [r3] / ...
 *
 * The ROM keeps ONE variable -- the index -- and addresses register-offset off
 * the unit base. gcc keeps the index for the bound test AND builds a parallel
 * cursor for the load, which costs the init plus the increment.
 *
 * ONE EDIT DID LAND and is kept: the ROM births `i = 0` before the 0x31
 * constant, so assigning the counter before the constant and leaving the `for`
 * init empty moved the first divergence from instruction 2 to 8 (31 -> 29).
 * That is the assignment-position lever from docs/elevation.md, working again.
 *
 * FOUR INNER-LOOP SHAPES, and the pointer survives all of them:
 *   temp `w = u[j]` tested in the while                 49 lines, 29 differ
 *   no temp, `while (u[j] != 0)` directly               49 lines, 29 differ
 *   an offset local declared INSIDE the loop body       49 lines, 29 differ
 *   `while` with the increment hoisted before the loop  60 lines, 51 differ
 *
 * The first three are byte-identical. The third is the notable one: declaring
 * the offset inside the loop body is exactly what fixed the preheader
 * placement on Sprite_DeleteLayer in this same round, and here it does
 * nothing. Those are different passes -- that lever moves where
 * move_movables/strength_reduce INSERT a value, and this is strength_reduce
 * deciding to CREATE one. The lever does not reach the create decision.
 *
 * The fourth is a clean negative: restructuring into a `while` with a peeled
 * first increment costs eleven lines.
 */
extern unsigned char *_GetUnit(int id);

void Func_80c1f50(int who)
{
    unsigned char *u;
    int i;
    int j;
    int v;
    int w;
    int val;

    i = 0;
    val = 0x31;
    for (; i <= 5; i++) {
        u = _GetUnit(i + 0x80);
        v = u[0x95 * 2];
        if (v == 1 && u[0x95 * 2 - 2] == who) {
            j = 0;
            if (u[0] == 0) {
                u[0] = val;
                u[v] = 0;
            } else {
                do {
                    j++;
                    if (j > 0xd)
                        return;
                    w = u[j];
                } while (w != 0);
                u[j] = val;
                u[j + 1] = w;
            }
            return;
        }
    }
}
