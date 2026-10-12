/* Func_80b90ac -- 0x080b90ac, the whole of asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_a.s.
 *
 * LANDING: 76 bytes, 34 encodings and 3 relocations identical.
 *   PER-FILE FLAG GROUP: GCSE_CFLAGS (-fno-gcse).  REQUIRED -- see below.
 *   PINS: 0 (tools/shimcount.py).
 *
 * Verify with (inside the build container):
 *   python3 tools/objcmp.py src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_a.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_a.s --whole
 *   ... with GCSE_CFLAGS, i.e. OBJCMP_EXTRA=-fno-gcse.
 *
 * PIECE: the .s holds ONE .thumb_func_start, so no split is needed and the
 * whole file converts.  (tools/piece.py: 1 function, 1 parked.)
 *
 * WHAT IT DOES.  Asks Func_80b6c08 for a roster of unit ids into a 28-byte
 * stack buffer, then for each id clears the unit's 0x12b flag byte and
 * recalculates its stats.
 *
 * ========== THE LANDING IS A THREE-WAY CROSSING.  NONE OF THE THREE ==========
 * ==========  IS WORTH ANYTHING ON ITS OWN -- ALL THREE MEASURED.    ==========
 *
 * The park was at 26 of 34 with the stream FOUR BYTES SHORT (29 reference
 * 16-bit instructions against our 28), and it recorded seven source spellings
 * as "MEASURED AND EXACTLY INERT".  Every one of those was measured at default
 * flags, and that is why they were inert.  Measured here:
 *
 *   two pointer names + struct store, NO FLAG   26 of 34, 29/28  (short)
 *   one pointer name  + struct store + flag     26 of 34, 29/28  (short)
 *   two pointer names + flag, char store         3 of 34, 29/29
 *   ALL THREE                                    0 -- byte-identical
 *
 * 1. TWO POINTER NAMES.  The reference splits the buffer address across two
 *    callee-saved registers -- `mov r5, sp` for the Func_80b6c08 argument and
 *    `mov r7, r5` inside the guard for the loop base -- and then reuses r5 for
 *    the count.  That second definition has to exist in the RTL as its own
 *    pseudo, so the source needs a second name (`p = base;`) assigned inside
 *    the guard.
 *
 * 2. -fno-gcse, AND THIS IS WHAT THE PARK WAS MISSING.  Batch 333 named the
 *    pass correctly and the diagnosis REPRODUCES on this function exactly as
 *    written: with the second name, `cand.c.07.gcse` prints
 *
 *        SET hash table ... Index 0 (set (reg/v:SI 33) (reg/v:SI 32))
 *        COPY-PROP: Replacing reg 33 in insn 42 with reg 32
 *        COPY-PROP: Replacing reg 33 in insn 59 with reg 32
 *        CPROP of Func_80b90ac, pass 1: 0 const props, 2 copy props
 *
 *    -- so gcse's copy propagation deletes the copy in pass 07, long before
 *    local-alloc or global-alloc see it.  THE BATCH-343 BRIEF SAID THIS
 *    DIAGNOSIS HAD BEEN CORRECTED IN BATCH 334 AND THAT THE PASS IS NOT CPROP.
 *    That is a conflation: batch 334 corrected the SIBLING specimen
 *    Func_80c1014 ("not a copy at all ... `mov r2, r8` is a reload"), and the
 *    correction does not transfer.  On THIS function cprop is the pass, the
 *    dump says so, and -fno-gcse is therefore the honest flag.
 *
 *    Once the copy survives, the allocation follows with no help:
 *    HARD_FRAME_POINTER_REGNUM is r7 (arm.h:898-899) and
 *    local-alloc.c:1978-1990 reserves it unconditionally, so only global-alloc
 *    can hand out r7 -- and the loop base crosses basic blocks, so it
 *    qualifies.  The two allocnos only have to EXIST.
 *
 * 3. THE FLAG BYTE IS ITS OWN STRUCT, FOR ITS ALIAS SET.  With 1 and 2 the
 *    function reads 3 of 34, all three in the loop: the reference re-reads the
 *    roster halfword BEFORE the byte store (`ldrh / strb`) and we emit it two
 *    slots later (`strb / sub / ldrh`).  The park called that "downstream of
 *    the allocation"; it is not, it is an ALIAS SET.  `_GetUnit` returns
 *    `unsigned char *`, a char lvalue is alias set 0, alias set 0 conflicts
 *    with everything, so the store/load dependence pins the load after the
 *    store and sched2 cannot hoist it.  Reaching the byte through its own
 *    struct tag gives the store a non-zero alias set that cannot conflict with
 *    the `short` roster read, and sched2 then hoists the load to the
 *    reference's position by itself.
 *
 *    This is the same machinery as docs/elevation.md's "Strict aliasing can
 *    SINK a store, and a `char *` lvalue pins it", run in the opposite
 *    direction: there a char lvalue was used to PIN a store that was moving;
 *    here a non-char lvalue is used to RELEASE a load that was pinned.
 *    -fno-strict-aliasing is the wrong tool for it -- it makes everything
 *    alias, which is what we are trying to stop.
 *
 * ALSO MEASURED AND NOT USED: pre-computing the second read into a local ahead
 * of the store does put it in the right place but costs an extra instruction
 * (30 against 29) because the `_GetUnit` result then needs a name too.
 * -fno-schedule-insns2 crossed with 1 and 2 reads 4, not 3 -- worse, exactly as
 * docs/elevation.md's "actively misleading probe" section predicts.
 */
extern int Func_80b6c08(int a, short *buf);
extern unsigned char *_GetUnit(int id);
extern void _CalcStats(int id);

/* The 0x12b flag byte, reached as its own object so that the store cannot
 * alias the halfword roster read beside it. */
struct UnitFlag {
    unsigned char v;
};

void Func_80b90ac(void)
{
    short buf[14];
    short *base;
    short *p;
    int count;
    int n;
    int off;
    int z;

    base = buf;
    count = Func_80b6c08(3, base);
    if (count > 0) {
        off = 0;
        p = base;
        z = 0;
        n = count;
        do {
            ((struct UnitFlag *)(_GetUnit(*(unsigned short *)(off + (int)p)) + 0x12b))->v = z;
            _CalcStats(*(unsigned short *)(off + (int)p));
            n--;
            off += 2;
        } while (n != 0);
    }
}
