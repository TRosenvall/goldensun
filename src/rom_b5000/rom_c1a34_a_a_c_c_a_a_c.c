/* Func_80c23c0, Func_80c23e8 and GetEnemyAttackAnimParam -- 0x080c23c0,
 * 0x080c23e8 and 0x080c2410, from asm/rom_b5000/rom_c1a34_a_a_c_c_a_a_c.s.
 *
 * Three accessors over one 8-byte-per-entry sprite table of 0xAC entries, each
 * reading a different subfield of byte 2 of the entry and each returning a
 * fallback for an out-of-range index. Byte 2 splits as 1 + 4 + 3 bits, which is
 * what the ROM's shift pairs say: `lsl #31` alone for bit 0, `lsl #27 / lsr #28`
 * for the middle nibble, and `lsr #5` alone for the top three bits. Declaring
 * the record with those three bitfields reproduces all three extractions with no
 * shifting written by hand, and also fixes the table walk: with the array
 * indexed as a struct the base lands in r3 and the scaled index in r2, which is
 * the ROM's assignment and the opposite of what pointer arithmetic on a
 * `unsigned char *` gives.
 *
 * THE LEVER IS THE STATEMENT-LEVEL BRANCH, and all three needed it. Every
 * expression form of the fallback -- `v ? v : 1`, `if (v == 0) return 1;`,
 * `v <<= 31; if (v) r = 1;`, `return (b & 1) != 0;` -- lets gcc prove the result
 * is a single bit or a single value and collapse it, and our output comes out
 * two to five instructions SHORTER than the ROM. Each function needed a
 * different one of the two statement shapes:
 *
 *   - Func_80c23c0 wants the *initialise then override* form,
 *     `r = 0; if (field) r = 1;`. This is the batch-53/55 lever: the mask read
 *     `(b & 1)` written in the condition of an `if` statement survives, where
 *     `(b & 1) != 0` in a return is rewritten to `lsl #31 / lsr #31`. It also
 *     produces the ROM's `mov r1, #0 ... mov r1, #1 / mov r0, r1`: `r` is
 *     written in two blocks, so it is a global-allocator quantity, gets r1, and
 *     the copy into the return register is real.
 *
 *   - Func_80c23e8 and GetEnemyAttackAnimParam want the *explicit else* form,
 *     `if (v != 0) r = v; else r = FALLBACK;`. Initialise-then-override is
 *     wrong for these two -- it puts the constant first and emits `mov r0, #0`
 *     before the compare -- and `r = v; if (v == 0) r = FALLBACK;` is wrong
 *     too, because gcc coalesces `r` with `v` and drops the ROM's `mov r0, r3`.
 *     Only the two-armed `if`/`else` keeps `v` live in its own register across
 *     the compare, which is what that copy is.
 *
 * GetEnemyAttackAnimParam is the one that had been called unreachable: its
 * fallback is 0 and its `else` arm is therefore a no-op, so every spelling that
 * lets gcc see it as one -- including `if (v == 0) v = 0;` and
 * `if (v == 0) return 0; return v;` -- is deleted outright and lands four
 * instructions short. The `else r = 0;` arm of a two-armed `if` is not deleted,
 * because the two arms are separate assignments to a variable that spans
 * blocks and nothing merges them before allocation.
 */

struct SpriteTableEntry {
    unsigned char pad0[2];
    unsigned char flag : 1;
    unsigned char mid : 4;
    unsigned char hi : 3;
    unsigned char pad3[5];
};

extern struct SpriteTableEntry Lc7420[] __asm__(".Lc7420");

int Func_80c23c0(int i)
{
    int r;

    if ((unsigned int)i > 0xab)
        return 0;
    r = 0;
    if (Lc7420[i].flag)
        r = 1;
    return r;
}

int Func_80c23e8(int i)
{
    int v;
    int r;

    if ((unsigned int)i > 0xab)
        return 1;
    v = Lc7420[i].mid;
    if (v != 0)
        r = v;
    else
        r = 1;
    return r;
}

int GetEnemyAttackAnimParam(int i)
{
    int v;
    int r;

    if ((unsigned int)i > 0xab)
        return 0;
    v = Lc7420[i].hi;
    if (v != 0)
        r = v;
    else
        r = 0;
    return r;
}
