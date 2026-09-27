/* PreloadSpriteGFX -- NON-MATCHING, 98 encodings of 106.  ref 224 bytes / ours 216,
 * ref 106 instructions / ours 102 -- FOUR SHORT, so do not read the 98 as a distance.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/800b6b8.c \
 *     asm/rom_9000/rom_b074_c.s --func PreloadSpriteGFX
 *
 * PROLOGUE AND EPILOGUE ARE EXACT, including `sub sp, #4` and the caller-save spill of `id`
 * to that slot, and loop 1's body matches the ROM instruction for instruction.
 *
 * LANDING NEEDS A TEXT/DATA SPLIT even though this is the ONLY function in its .s.
 * datacheck.py reports one `.rodata`, one exported label, sitting AFTER the function:
 * `.L12fa0: .incrom 0x12fa0, 0x1307c` (220 bytes) at line 136, referenced at line 43 inside
 * this function.  stage1.ld:277 already carries `asm/rom_9000/rom_b074_c.o(.rodata)`.
 *
 * THE TRANSFERABLE FINDING, AND IT IS COMPILER-SOURCE-GROUNDED WITH THE WIDEST REACH OF
 * ANYTHING IN THIS BATCH -- `break` AND `goto` OUT OF A LOOP COMPILE TO DIFFERENT LOOP
 * SHAPES:
 *
 *   `expand_end_loop` (stmt.c:2340-2551) scans from the loop top for a CONDITIONAL JUMP
 *   WHOSE TARGET IS THE LOOP'S `end_label` -- i.e. a `break`, or a `while` condition -- and
 *   ROLLS everything above the LAST such jump to the bottom of the loop.  jump.c:325 then
 *   fires `duplicate_loop_exit_test` on the resulting NOTE_INSN_LOOP_BEG plus unconditional
 *   jump and PEELS a copy of the test above the loop.  A `goto` to a label OUTSIDE the loop
 *   is NOT `end_label`, so the scan finds nothing and NO ROLL HAPPENS.
 *
 * So, as a rule for reading loop layout off the ROM:
 *
 *   A ROM LOOP WITH ITS EXIT TESTS AT THE TOP AND AN UNCONDITIONAL `b` BACK-EDGE AT THE
 *   BOTTOM WAS WRITTEN WITH `goto`, NOT `break`.
 *
 * That is loop 1 here: `break` gives the rotated form with a duplicated test block and
 * `mov r4,#1` (115 insns, wrong shape); `goto found` gives the ROM's `b L4` exactly.
 *
 * AND THE CONVERSE HOLDS, which is what makes it a rule rather than a preference: loop 2 in
 * the ROM IS rolled (peeled entry test, conditional `bne` back-edge).  Writing ITS counter
 * exit as `break` SUPPRESSES the roll, because the break's jump becomes `last_test_insn`
 * and equals `get_last_insn()`, failing the `last_test_insn != get_last_insn()` guard.
 * Written as `goto done`, only the `while` condition is found and the roll happens.
 * Drop ladder: goto->break in loop 2 costs 3 instructions and the ROM's shape.
 *
 * Loop 1 also needs a SEPARATE `short *ip` WALKING POINTER for the offset-2 field
 * (`ip = &e[1].id;` ... `v = *ip; ip += 2;`).  With `e->id` the address is base+const and
 * reload emits `mov rK,#2 / ldrsh [r2,rK]`; with a walking pointer reload manufactures a
 * zero register and emits `ldrsh [r1,r6]` with `add r1,r2,#6` in the preheader -- the ROM's
 * form.  THAT IS THE `extendqisi2`-AT-EXPAND LEVER FROM BATCH 284 HOLDING FOR
 * `extendhisi2`.  Dropping `ip`: 108 insns and loop 1 no longer matches.
 * Both counters must be `unsigned` -- the ROM's `bhi`, not `bgt`.
 *
 * BLOCKER: CROSS-JUMPING MERGES THE TWO `lsl #16 / lsr #16` TRUNCATIONS -- it places the
 * loop label between the `ldrsh` and the shift so both paths share them, where the ROM
 * keeps two copies -- plus the r4/r5 colouring in loop 2 and its `mov r2,r3` loop-carried
 * copy.  PASS: 60-permutation declaration sweep ALL IDENTICAL at 102 differing / 109 insns,
 * INERT.  Also inert or worse: `unsigned int v` with an explicit `(unsigned short)` cast
 * (105 insns), `int v` with `& 0xffff` (108).
 */
extern unsigned char *iwram_3001e68;
extern unsigned char L12fa0[] __asm__(".L12fa0");
extern unsigned char Data_92b8[];
extern unsigned char *_GetSpriteInfo(int id);
extern void *GetFile(int index);
extern int DecompressLZ(void *src, void *dst);

struct E {
    unsigned short file;
    short id;
};

int PreloadSpriteGFX(int slot, unsigned char *buf, int id, int sel)
{
    unsigned char *info;
    struct E *e;
    short *ip;
    unsigned int *s;
    unsigned int *p;
    unsigned char *q;
    unsigned char *end;
    unsigned char *tbl;
    unsigned short v;
    unsigned int w;
    int f;
    unsigned int i;
    int k;
    int len;

    if ((unsigned int)slot > 7)
        return 0;
    s = (unsigned int *)(iwram_3001e68 + slot * 8);
    info = _GetSpriteInfo(id);
    s += 7;
    s[0] = (slot << 12) | id;
    s[1] = (unsigned int)buf;

    e = (struct E *)L12fa0;
    v = e->id;
    f = e->file;
    ip = &e[1].id;
    i = 0;
    for (;;) {
        e++;
        if (v == 0)
            return 0;
        if (v == id)
            goto found;
        if (++i > 0xff)
            goto found;
        v = *ip;
        f = e->file;
        ip += 2;
    }
found:

    len = DecompressLZ(GetFile(f), buf);

    p = (unsigned int *)buf;
    i = 0;
    while ((w = *p) != 0) {
        *p = w + (unsigned int)buf;
        p++;
        if (++i > 0xff)
            goto done;
    }
done:

    if (sel != 0) {
        q = (unsigned char *)(p + 1);
        end = buf + len;
        k = sel - 1;
        if ((unsigned int)k > 4)
            k = 0;
        tbl = Data_92b8 + (k << 8);
        while (q < end) {
            if (*q <= 0xdf)
                *q = tbl[*q];
            q++;
        }
    }
    return info[1] * info[0];
}
