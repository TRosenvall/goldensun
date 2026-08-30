/*
 * SetDjinni  (AddStatusEntry) -- asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_c.s
 *
 * BLOCKER: constant hoisting. 53 lines against 54, 24 differing, first
 * difference at line 30. gcc emits `mov r0, #0x8e / lsl r0, #1` before the
 * `orr`/`str` pair that ends the previous statement; the ROM emits them after,
 * at the top of the block that uses them.
 *
 * PROGRESS RECORD, because each step was a measured improvement and the levers
 * generalise:
 *
 *   50 lines, 37 differing  -- first draft
 *   50 lines, 37 differing  -- naming the index `i = k + 0xf8`
 *   50 lines, 32 differing  -- computing `i` BEFORE the mask `m = 1 << bit`
 *   53 lines, 24 differing  -- `return 0` in an else-branch, not a bare early
 *                              return
 *
 * The last of those is the interesting one. Written as an early
 * `if (...) return 0;` gcc hoists `mov r0, #0` above the compare and jumps
 * straight to the epilogue, collapsing FOUR instructions. Written as
 * `if (cond) { ...body... } else { return 0; }` -- identical semantics -- gcc
 * emits the ROM's separate `mov r0, #0 / b` block. Early-return versus
 * else-return is not cosmetic here; it is worth four instructions.
 *
 * Note the second step, naming the index, did NOT reproduce the ROM's
 * register-offset load on its own. It does now, but only because the
 * subsequent reorder changed the schedule. Contrast GetNumDjinn, where naming
 * the index alone was the whole fix -- there the index was an opaque
 * parameter, here `k = entry * 4` is locally computed and gcc can see through
 * the name and reassociate anyway. That is a real limit on the batch-142
 * naming lever and it is worth carrying forward: naming blocks reassociation
 * only when the compiler cannot see the definition.
 *
 * What is left is one hoisted constant pair and the one instruction it costs.
 */
extern unsigned char *GetUnit(int id);
extern int Func_807a1f8(int id, int entry, int bit);
extern void Func_8079ae8(int id);

int SetDjinni(int id, int entry, int bit)
{
    unsigned char *u;
    int r;
    int m;
    int k;
    int i;

    u = GetUnit(id);
    r = Func_807a1f8(id, entry, bit);
    if (r != 0) {
        k = entry * 4;
        i = k + 0xf8;
        m = 1 << bit;
        if ((*(int *)(u + i) & m) != 0) {
            k = k + (0x84 << 1);
            *(int *)(u + k) |= m;
            k = entry + (0x8e << 1);
            (*(unsigned char *)(u + k))++;
            Func_8079ae8(id);
        } else {
            return 0;
        }
    }
    return r;
}
