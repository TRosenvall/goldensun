/* Func_80a8b10 -- asm/rom_a1000/rom_a8604_a_a_c_a.s
 *
 * BLOCKER: ONE MISSING INSTRUCTION -- `mov r12, r5`. 58 of 66, ONE LINE SHORT,
 * and every one of the 58 is the cascade from that single omission.
 *
 * Collects four unit-status flags into a five-byte output buffer and returns
 * how many were set. Prologue, the backwards clear loop, both signed-char
 * reads, all four guarded stores and the return are otherwise exact.
 *
 * THE RESIDUE, in full:
 *
 *     rom    add r3, r5, #0x4 / mov r12, r5 / L0: ... cmp r3, r12 / bge L0
 *     ours   add r3, r5, #0x4 /               L0: ... cmp r3, r5  / bge L0
 *
 * The ROM copies the output base into r12 and compares the descending cursor
 * against the COPY; gcc compares against r5 directly, which is correct and one
 * instruction cheaper. r5 stays live in both versions -- it is used by all four
 * stores afterwards -- so the copy buys the ROM nothing that we can see.
 *
 * MEASURED:
 *   baseline                                      65 lines, 61 differ
 *   zero named in a local born before the cursor,
 *     and the limit named as `lim = out`          65 lines, 58 differ
 *
 * The first two lines of that second edit ARE correct and are kept: naming the
 * zero fixed `mov r1, #0x0 / mov r2, r0` ordering at the head, and moved the
 * first divergence from instruction 5 to instruction 8. The limit naming did
 * NOT survive -- gcc coalesces `lim` with `out` because they are provably the
 * same value, so no `mov` is emitted.
 *
 * WHY IT RESISTS. This is the two-names tell (docs/elevation.md: where the ROM
 * copies a register before using it, the original had two live names) hitting
 * its boundary. That tell has closed three functions in recent batches -- but
 * in each of those the two names held DIFFERENT values at some point
 * (a count and a loop counter, a division result and an offset). Here the copy
 * is of a value that never changes, so there is nothing to name: any local
 * initialised from `out` is the same rtx and gets coalesced away.
 *
 * So the tell needs a qualifier, now recorded in the doc: a copy is reachable
 * from source only when the two names DIVERGE. A pure duplicate of a live
 * value is an allocator artifact and no spelling produces it.
 *
 * NOT TRIED: forcing the copy with a register-pinned local, which would be a
 * fakematch (see ovl_7fb4a8/20087b0.c for that judgement).
 */
extern char *_GetUnit(int id);

int Func_80a8b10(char *out, int flag, int unit)
{
    char *u;
    char *p;
    int count;
    int v;
    int zero;
    char *lim;

    u = _GetUnit(unit);
    zero = 0;
    p = out + 4;
    lim = out;
    do {
        *p = zero;
        p--;
    } while ((int)p >= (int)lim);
    count = 0;
    if (*(short *)(u + 0x38) == 0 && flag == 1) {
        out[0] = flag;
        count = 1;
    }
    v = *(signed char *)(u + 0x131);
    if (v != 0) {
        if (v == 1)
            out[1] = v;
        else
            out[2] = 1;
        count++;
    }
    v = *(signed char *)(u + 0x98 * 2);
    if (v != 0) {
        out[3] = 1;
        count++;
    }
    if (*(unsigned char *)(u + 0xa0 * 2) != 0) {
        out[4] = 1;
        count++;
    }
    return count;
}
