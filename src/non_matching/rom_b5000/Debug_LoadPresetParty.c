/* Debug_LoadPresetParty -- NON-MATCHING, 193 of 213 encodings differ.
 * Reference asm//rom_b5000/rom_b5368_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching//Debug_LoadPresetParty.c \
 *       asm//rom_b5000/rom_b5368_a.s --func Debug_LoadPresetParty
 *
 * NOT a distance: ours is 210 instructions against 213 and 456 bytes against 460.
 * The FRAME SIZE IS EXACT and the slots are PERMUTED.  Shim-free.
 * BLOCKER: reload's spill-slot permutation.  Note the spill-slot rule from batch 298
 * -- declared locals take the HIGH offsets in declaration order -- is the first thing
 * to try here, since an exact frame with permuted slots is precisely that shape.
 */
/* Debug_LoadPresetParty (0x080b5368) -- NON-MATCHING.
 * NON-MATCHING: 193 encodings of 213 differ (objcmp), ours 210, size 456 against
 * the ROM's 460.  THE COUNTS DISAGREE, so 193 is NOT a distance.
 * tools/aligncmp.py: 79 of 213 aligned-equal, 164 differing/ins/del in 34 hunks.
 * This one is the LEAST FINISHED of batch 298f's five and is filed for the
 * structure, not the count.
 *
 * THE FRAME IS SOLVED AND IT IS EXACT: `sub sp, #0x2c`, with `int djinn[4]` at
 * sp+0x1c (ARM gcc-2.96 puts stack locals above reload's spill slots) and SEVEN
 * spill words below it at sp+0x00..0x18.  Getting there needed two things:
 * indexing the preset table DIRECTLY (`Lc3f34[idx].field`, no named `tbl`
 * pointer -- a named one is an eighth spill slot) and a NAMED `int *slots =
 * djinn;` for the djinn counters, which is the ROM's sp+4
 * (`mov r3, sp / add r3, #0x1c / str r3, [sp, #4]`).  Either change alone is
 * wrong: without the second the frame is 0x30, without the first 0x34.
 *
 * THE BLOCKER IS THE SPILL-SLOT PERMUTATION, the class
 * src/non_matching/rom_c9000/cfef4_ShiningStar.c names.  The frame SIZE matches
 * and every value is spilled, but reload's ASSIGNMENT of the seven words does
 * not: the ROM's order (lowest word allocated last) is scratch, slots, idx*4,
 * count, idx, ret, preset; ours puts `ret` where the ROM puts `idx*4`, and since
 * every reference is an `[sp, #N]` displacement, one transposition mismatches
 * every use of two variables at once.  That is most of the 164.
 *
 * ALSO OUTSTANDING, and independent of the slots:
 *   - `unit` is in r10 in the ROM (`mov r0, sl` at four call sites) and in r8
 *     here.  The four high registers are r8 = the inner djinn counter, r9 = the
 *     k*4 offset, r10 = unit, r11 = the table+2 byte pointer; ours rotates them.
 *   - THREE INSTRUCTIONS SHORT, all in the djinn-array zeroing loop.  The ROM
 *     compares a DESCENDING POINTER against the base held in ip:
 *         ldr r5, [sp, #4] / mov ip, r5 / add r3, sp, #0x28
 *         L: str r2, [r3] / sub r3, #4 / cmp r3, ip / bge L
 *     `for (i = 3; i >= 0; i--) djinn[i] = 0;` gives a counter instead, and the
 *     explicit pointer form (`p = &djinn[3]; do { *p = 0; p--; } while (p >=
 *     slots);`) reaches the pointer compare but costs two instructions elsewhere
 *     and ends 5 short rather than 3.  Both frames are 0x2c.
 *
 * ================================================================
 * WHAT IS ESTABLISHED, and it is the point of this file
 * ================================================================
 *
 * 1. THE PRESET TABLE LAYOUT, 20 bytes per entry, read off the four access
 *    patterns in the reference:
 *        +0x00  signed char unit;       ldrsb, -1 terminates the entry list
 *        +0x01  unsigned char level;    ldrb, passed to _SetMinLevel
 *        +0x02  signed char dj[4];      ldrsb at +2+k, the per-element count
 *        +0x06  unsigned short items[4];  ldrh, _GiveItemTo -> _EquipItem
 *        +0x0e  unsigned short moves[2];  ldrh, _GiveInnateMove
 *        +0x12  unsigned short pad;
 *    `.Lc3f34` is ALREADY `.global`-exported (asm/rom_b5000/rom_b5368_c.s:7), so
 *    landing this needs NO new data export -- only the text split.  Declared the
 *    tree's way: `extern struct Preset Lc3f34[] __asm__(".Lc3f34");`.
 *
 * 2. THE LOOP BOUND IS 0x14c AND UNSIGNED.  `mov r3, #0xa6 / lsl r3, #1 /
 *    cmp r1, r3 / bls` -- 333 entries scanned, `idx` an `unsigned int`, and the
 *    `> 0x14c` exit is the only place the function returns 1.
 *
 * 3. `idx * 20` IS SYNTHESISED AS `(idx * 4 + idx) * 4` AND `idx * 4` IS THE
 *    CSE.  It is spilled at sp+8 and reloaded at three later sites, each of which
 *    redoes `add r3, r1, r2 / lsl r3, #2`.  That is gcc's own expand_mult for 20
 *    plus plain CSE -- the source indexes `Lc3f34[idx]` at each site and does NOT
 *    hold a `&Lc3f34[idx]` pointer; a named entry pointer computes the address
 *    once and is wrong.
 *
 * 4. THE POOLED ZERO IN THE 32-ENTRY CLEAR LOOP IS NOT A SYMBOL.
 *    `ldr r3, .Lb543c @ 0` where `mov r3, #0` would do is const.sym's tell, but
 *    it is the KNOWN EXCEPTION that file's header records: the zero is the
 *    operand of a HALFWORD store (`strh r3, [r0]`), so gcc pools it as a HImode
 *    constant, and per batch 292 a PC-relative `ldrh` assembles to the same
 *    halfword as `ldr`.  NO const.sym ENTRY IS NEEDED.
 *
 * 5. ARGUMENT ORDERS, all read off the reference: `_SetMinLevel(unit, level)`,
 *    `_GiveDjinni(unit, k, slots[k])` and `_SetDjinni` the same,
 *    `_Func_80788c4(unit, 0)` sixteen times, `_EquipItem(unit,
 *    _GiveItemTo(unit, item))`, and the five `_Func_8079664` calls are
 *    0, 1, 2, 3, 5 -- FIVE, skipping 4, which is what the reference's prose says
 *    and which checks out.
 *
 * 6. THE INNER DJINN LOOP RELOADS ITS BOUND.  `ldrsb r3, [r2, r4]` sits INSIDE
 *    the loop with r2 = `.Lc3f34 + 2` held in r11 and r4 = idx*20 + k, so the
 *    count is re-read every iteration -- i.e. the source really is
 *    `for (j = 0; j < Lc3f34[idx].dj[k]; j++)`, not a hoisted count.  And the
 *    outer loop increments the BYTE offset r4 by 1 per k while r9 tracks k*4,
 *    which is the two-induction-variable shape that a `dj[k]` / `slots[k]` pair
 *    produces.
 *
 * ================================================================
 * THE REFERENCE'S PROSE IS ACCURATE HERE
 * ================================================================
 *
 * `@ LoadPartyPreset / r0 = preset index. Wipes the current party -- Func_79664
 *  removes characters 0, 1, 2, 3 and 5 -- then rebuilds it from the preset,
 *  giving each member its equipment through EquipItem / GiveInnateMove,
 *  refreshing stats with SetMinLevel and re-adding them with AddPartyMember.`
 * Every clause checks out.  What it does not mention: the return value is 1 only
 * when the scan runs off the end of the table (idx > 0x14c) and 0 otherwise, and
 * the per-member work also clears two 4-entry byte arrays at u+0x118/u+0x11c,
 * two 4-entry word arrays at u+0xf8/u+0x108, and a 32-entry halfword array
 * walked DOWN from u+0xd4, before the djinn are handed out.
 *
 * ================================================================
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b5368_LoadPresetParty.c \
 *     asm/rom_b5000/rom_b5368_a.s --func Debug_LoadPresetParty
 * FOUR functions in asm/rom_b5000/rom_b5368_a.s (Debug_LoadPresetParty,
 * Func_80b5534, Debug_BattleTest, Func_80b5864) and no data section, so a text
 * split is needed; tools/datacheck.py reports no requirement.
 * Shim count 0 (tools/shimcount.py): pin-free, no fakematch.txt row.
 */

struct Preset {
    signed char unit;
    unsigned char level;
    signed char dj[4];
    unsigned short items[4];
    unsigned short moves[2];
    unsigned short pad;
};

extern struct Preset Lc3f34[] __asm__(".Lc3f34");

extern void _Func_8079664(int slot);
extern void _AddPartyMember(int unit);
extern void _SetMinLevel(int unit, int level);
extern unsigned char *_GetUnit(int unit);
extern void _GiveInnateMove(int unit, int move);
extern void _GiveDjinni(int unit, int elem, int id);
extern void _SetDjinni(int unit, int elem, int id);
extern void _Func_80788c4(int unit, int a);
extern int _GiveItemTo(int unit, int item);
extern void _EquipItem(int unit, int slot);

int Debug_LoadPresetParty(int preset)
{
    int djinn[4];
    int *slots;
    unsigned int idx;
    int count;
    int ret;
    int unit;
    int i, k, j;
    unsigned char *u;
    int it;
    int m;

    ret = 0;
    _Func_8079664(0);
    _Func_8079664(1);
    _Func_8079664(2);
    _Func_8079664(3);
    _Func_8079664(5);
    slots = djinn;
    count = 0;
    for (i = 3; i >= 0; i--)
        djinn[i] = 0;
    idx = 0;
    while (idx <= 0x14c) {
        unit = Lc3f34[idx].unit;
        if (unit == -1) {
            if (count == preset)
                goto done;
            count++;
        } else if (count == preset) {
            _AddPartyMember(unit);
            _SetMinLevel(unit, Lc3f34[idx].level);
            u = _GetUnit(unit);
            for (i = 0; i < 4; i++) {
                *(unsigned char *)(u + 0x118 + i) = 0;
                *(unsigned char *)(u + 0x11c + i) = 0;
                *(int *)(u + 0xf8 + i * 4) = 0;
                *(int *)(u + 0x108 + i * 4) = 0;
            }
            for (i = 31; i >= 0; i--)
                *(unsigned short *)(u + 0x58 + i * 4) = 0;
            for (i = 0; i < 2; i++) {
                m = Lc3f34[idx].moves[i];
                if (m != 0)
                    _GiveInnateMove(unit, m);
            }
            for (k = 0; k < 4; k++) {
                for (j = 0; j < Lc3f34[idx].dj[k]; j++) {
                    _GiveDjinni(unit, k, slots[k]);
                    _SetDjinni(unit, k, slots[k]);
                    slots[k]++;
                }
            }
            for (i = 0; i < 16; i++)
                _Func_80788c4(unit, 0);
            for (i = 0; i < 4; i++) {
                it = Lc3f34[idx].items[i];
                if (it != 0)
                    _EquipItem(unit, _GiveItemTo(unit, it));
            }
        }
        idx++;
    }
    ret = 1;
done:
    return ret;
}
