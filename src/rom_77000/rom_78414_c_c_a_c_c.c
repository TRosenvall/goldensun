/* CanRemoveItem (0x08078980) -- MATCHING, batch 332 D.
 *
 * MATCHES: 92 bytes, 43 encodings and 2 relocations identical, whole file.
 * PINS: 0.  No shim, no device, no flag group, no fakematch row.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_78414_c_c_a_c_c.c \
 *     asm/rom_77000/rom_78414_c_c_a_c_c.s --whole
 *
 * INSTALL PATH: src/rom_77000/rom_78414_c_c_a_c_c.c
 * NO SPLIT.  The piece holds this function alone, so it converts whole.
 *
 * ---------------------------------------------------------------------------
 * THE WHOLE RESIDUE WAS ONE COPY, AND THE COPY IS AN IMMEDIATE-ENCODING COST
 *
 * The park's observation was exact:
 *
 *     rom    lsl r5, #0x1 / mov r6, r5 / add r6, #0xd8
 *     park   lsl r6, r5, #0x1 / add r6, #0xd8
 *
 * one instruction apart, with every later encoding one position early -- so
 * its positional claim was measuring that offset and not a distance.  Its
 * verdict was that the copy needs `slot * 2` live in its own register at the
 * same time as the offset, that nothing here needs that, and that therefore
 * "this is allocator choice and no source form selects it".
 *
 * Nothing needs the two values live at once.  The copy is not about liveness
 * at all -- it is forced by the ADD'S IMMEDIATE.  Thumb-1's three-operand
 * `add Rd, Rn, #imm` carries a 3-bit immediate, so there is no
 * `add r6, r5, #0xd8`; the only expansion of "a new value equal to another
 * register plus 0xd8" is a register copy followed by the two-address
 * `add Rd, #imm8`.  So the copy appears exactly when the shift updates its own
 * pseudo in place and the offset is a SEPARATE pseudo formed in ONE statement:
 *
 *     slot *= 2;             two-address update of slot's own pseudo -> lsl r5, #1
 *     off = slot + 0xd8;     a new pseudo + an 8-bit immediate -> mov r6, r5
 *                                                                add r6, #0xd8
 *
 * WHY THE PARK'S PROBE MISSED IT BY ONE STATEMENT.  It tried `slot *= 2;` and
 * then `off = slot;` and then `off += 0xd8;` -- the same three values in the
 * same order -- and measured it byte-identical to the park.  That spelling
 * makes the copy a statement of its own, and a copy whose source is dead is
 * copy-propagated away, after which the shift folds back to the three-operand
 * form.  The copy only survives when it is not a copy in the source: it has to
 * be the side effect of an addition gcc cannot encode.  This is the
 * two-similar-regions trap in its smallest form -- the park varied the SHIFT
 * for three rounds when the statement that decides it is the ADD.
 *
 * ALSO CONFIRMED AND KEPT, from the park: the arithmetic must come AFTER the
 * GetUnit call.  The reference holds the raw slot across the call and only then
 * doubles it, so the call has to precede the doubling in the source; written
 * the other way the prologue itself is wrong from the first instruction.
 *
 * The semantics are unchanged from the park and are worth restating: the item
 * halfword is read at unit + slot*2 + 0xd8, masked with 0x1ff for the id, and
 * the three failure codes are -1 (no item), -4 (info byte 3 has bit 3) and
 * -3 (item bit 9 set AND info bit 1 set).
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
    slot *= 2;
    off = slot + 0xd8;
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
