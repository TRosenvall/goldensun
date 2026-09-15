// fakematch
/* ModifyHP @ 0x080783a4, ModifyPP @ 0x080783dc
 * (the whole of asm/rom_77000/rom_77320_c_c_c.s's .text -- both functions)
 *
 * A TRUE TWIN PAIR: identical structure, differing only in the field offsets
 * (0x38/0x34 against 0x3a/0x36).  They screened at the SAME 9 of 26 from the
 * same first draft and closed on the same one-line change -- so the pair cost
 * barely more than one function.
 *
 * The clamp is written to match the ROM's emission order: v takes max, then the
 * two tests fall through to it.
 *
 * THE RESIDUE WAS A PURE REGISTER-ROLE SWAP, and the dumps name the cause.
 * Every one of the 26 instructions was correct except that the ROM keeps the
 * running value in r3 and the maximum in r2, and we had them the other way --
 * which also turns the ROM's destructive `add r3, r5` into `add r2, r2, r5`.
 *
 *   .17.lreg:  Register 35 used 6 times across 9 insns; set 2 times
 *              Register 36 used 3 times across 4 insns IN BLOCK 0; set 1 time
 *   .18.greg:  35 in 2   36 in 3        ;; Register 36 in 3.   <- local_alloc
 *
 * `max` is block-local and dies once, so LOCAL_ALLOC takes it -- and local_alloc
 * runs FIRST and hands out REG_ALLOC_ORDER's head, r3.  `cur` spans blocks, so
 * it falls through to global_alloc and gets whatever is left.  That is the
 * batch-258 pass-ownership finding seen from the other side: there the problem
 * was a quantity DEMOTED into global_alloc, here it is a quantity PROMOTED into
 * local_alloc and taking the register the other one needed.
 *
 *   When two locals exchange registers, check whether one of them is
 *   block-local.  If it is, priority never runs -- local_alloc has already
 *   spent the register before global_alloc sees either of them.
 *
 * NO HONEST FORM REACHES IT, because every restructuring that changes which
 * variable is block-local also changes the LENGTH:
 *
 *     if/else-if chain (shipped shape)                  9 of 26, length exact
 *     v = max; if (cur <= max) { v = 0; ... }           52 bytes against 56
 *     goto-based fallthrough                            52 bytes against 56
 *     nested if inside the <= max arm                   52 bytes against 56
 *     v = cur; then the two tests                       7 of 26
 *     cur loaded and added in one expression           13 of 26
 *
 * The three short forms are gcc collapsing the clamp into fewer branches; they
 * are not the ROM's shape at all.
 *
 * FAKEMATCH: ONE pin per function.  Either member works here --
 * `cur` pinned to r3 and `max` pinned to r2 are both exact -- which is worth
 * recording against the batch-258 note that "pin either member" is too loose.
 * That note stands for a swap decided by global_alloc PRIORITY, where pinning
 * the low-priority member only half-fixes it.  Here the swap is decided by pass
 * OWNERSHIP, and naming either side settles it, because the pin removes that
 * variable from the contest entirely.  `cur` is pinned rather than `max` because
 * it states the ROM's accumulator directly.
 */
extern unsigned char *GetUnit(int id);
extern void UpdateStatBarPercent(int id);

int ModifyHP(int id, int delta)
{
    unsigned char *u;
    int max, v;
    register int cur __asm__("r3");

    u = GetUnit(id);
    cur = *(short *)(u + 0x38);
    max = *(short *)(u + 0x34);
    cur += delta;
    if (cur > max)
        v = max;
    else if (cur < 0)
        v = 0;
    else
        v = cur;
    *(short *)(u + 0x38) = v;
    UpdateStatBarPercent(id);
    return *(short *)(u + 0x38);
}

int ModifyPP(int id, int delta)
{
    unsigned char *u;
    int max, v;
    register int cur __asm__("r3");

    u = GetUnit(id);
    cur = *(short *)(u + 0x3a);
    max = *(short *)(u + 0x36);
    cur += delta;
    if (cur > max)
        v = max;
    else if (cur < 0)
        v = 0;
    else
        v = cur;
    *(short *)(u + 0x3a) = v;
    UpdateStatBarPercent(id);
    return *(short *)(u + 0x3a);
}
