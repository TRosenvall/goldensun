/* CanRemoveItem -- 0x08078980 -- NON-MATCHING.
 * Blocker class: TWO-OPERAND vs THREE-OPERAND SHIFT, and the one-instruction
 * cascade it causes.  45 lines against the ROM's 46, 43 differing -- but the
 * 43 is almost entirely the one-slot shift, not 43 independent problems.
 *
 *     rom    lsl r5, #0x1 / mov r6, r5      (two instructions)
 *     ours   lsl r6, r5, #0x1               (one)
 *
 * The ROM shifts the saved slot IN PLACE and then copies it; gcc uses the
 * three-operand form and needs no copy, so every instruction after it is one
 * position early.
 *
 * SOLVED: the arithmetic must come AFTER the GetUnit call.  The ROM holds the
 * raw slot in r5 across the call and only then doubles it, so `u =
 * GetUnit(who);` has to precede `off = slot * 2;` in the source. Written the
 * other way the prologue itself is wrong (44 differing from line 1); this way
 * `mov r5, r1 / bl GetUnit` is exact and the divergence starts at line 3.
 *
 * Tried for the shift, no change (43 differing, byte-identical output):
 *   - `slot *= 2;` then `off = slot;`, the compound-assignment form that DOES
 *     produce two-operand adds elsewhere in this tree. It does not extend to
 *     a shift whose result goes to a different variable: gcc still folds the
 *     copy into a three-operand lsl.
 *
 * For the copy to survive, `slot * 2` would have to be live in its own
 * register at the same time as `off`, and nothing here needs that -- the ROM
 * does not need it either, since it overwrites r5 with the 0x1ff mask a few
 * instructions later. This is allocator choice, the register-pressure category
 * HANDOFF.md describes, and no source form selects it.
 *
 * The semantics are believed right and are worth keeping: the item halfword is
 * read at unit + slot*2 + 0xd8, masked with 0x1ff for the id, and the three
 * failure codes are -1 (no item), -4 (info byte 3 has bit 3), and -3 (item bit
 * 9 set AND info bit 1 set).
 */
extern char *GetUnit(int who);
extern unsigned char *GetItemInfo(int id);

int CanRemoveItem(int who, int slot)
{
    char *u;
    int off;
    int id;
    unsigned char *info;
    int flags;

    u = GetUnit(who);
    off = slot * 2;
    off += 0xd8;
    id = *(unsigned short *)(u + off) & 0x1ff;
    info = GetItemInfo(id);
    if (id == 0)
        return -1;
    flags = info[3];
    if (flags & 8)
        return -4;
    if ((*(unsigned short *)(u + off) & (0x80 << 2)) && (flags & 2))
        return -3;
    return 0;
}
