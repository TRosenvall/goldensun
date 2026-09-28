/* Func_801de5c -- 0x0801de5c, asm/rom_15000/rom_1de5c_a_a.s.
 * NON-MATCHING, 351 encodings of 456.  NOT a distance (ref 1028 bytes / 456 encodings against ours 1008 / 452).  READ `--align`: 276 of 465.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801de5c.c \
 *     asm/rom_15000/rom_1de5c_a_a.s --func Func_801de5c
 * PARKED, batch 294 brief H.  402 instructions.
 *
 * WHOLE-FILE CONVERSION, confirmed: `grep -ci func_start` = 1 and
 * `python3 tools/datacheck.py <ref>` exits 0.  The 27 `.word` entries in the
 * reference are gcc's own inline thumb jump table for the control-code switch,
 * not a data section -- the same reading as the landed family member
 * src/rom_15000/rom_1de5c_c_a_a.c, which has six of them.
 *
 * PARKED AT 276 of 465 (tools/tryc.py --align).  objcmp --whole says
 *     XX Func_801de5c   351 of 456 differ (ours 452), first at index 8
 *     XX SIZE  ref 1028 bytes, ours 1008
 * NOT a true distance: 452 encodings against 456 and 20 bytes short.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_15000/801de5c.c \
 *     asm/rom_15000/rom_1de5c_a_a.s --align
 *
 * SHIMS: ZERO in both reported classes.  No `register ... __asm__`, no
 * `__asm__(".equ ...")`.  The single `__asm__` is the `.L371b4` asm-name binding
 * for the 16-byte palette-index table that lives in
 * asm/rom_15000/rom_1de5c_c_c_c_c_c_c.s and is already `.global` there.
 *
 * .sym: `_FILE_13` (file_table.sym, already present) is required and is a clean
 * pooling tell -- the ROM writes `ldr r0, =0x13` where an 8-bit `mov` would do,
 * and the consumer is GetFile, which is exactly what file_table.sym is for.
 * Nothing else here wants a symbol; every other pooled value in the function
 * (0xf000, 0x4040404, 0xe0e0e0e, 0xff00ff0, 0xff00ff, 0xf01d/e/f, 0x6000000..1c)
 * is either too wide for a `mov` or a shifted byte the ROM also builds inline.
 *
 * ========================= WHAT THE FUNCTION IS =========================
 * The general glyph rasteriser: RenderTextToBuffer.  Four phases.
 *   1. allocate a 0x800-byte scratch with Func_8004938, fetch the font asset
 *      with GetFile(_FILE_13), build a 16-entry colour map on the frame at
 *      sp+0x14 by calling Func_80008d4 THROUGH A POINTER (`bl _call_via_r3`),
 *      then fill the scratch with 0x04040404 or 0x0e0e0e0e depending on whether
 *      the palette word (state[0xea7] << 12) is 0xf000, again through a pointer.
 *   2. walk the u16 text run.  op <= 0x1e is a control code handled by a
 *      27-entry jump table over 3..0x1d; anything higher is a glyph, rasterised
 *      as 8 rows x 8 nybble-pairs out of a 32-byte cell into a 256-byte-stride
 *      buffer, then the pen advances by 8 (ops 0xf01d/0xf01f), 3 (0xf01e),
 *      Data_370d4[op - 0x20], or 1.
 *   3. re-pack the 8bpp scratch into 4bpp tiles with the classic
 *      x |= x<<4 / &0x0ff00ff0 / x |= x<<8 / shift-merge interleave.
 *   4. allocate a VRAM tile for each cell out of the state's own used-tile
 *      bitmap at state[0xda0], write the 8 words to 0x6000000 + tile*32, and
 *      publish `palette | tile` into both output halfword pointers.
 * Returns the cell count.  Structurally traced end to end; every block, every
 * branch polarity and both jump tables reproduce.
 *
 * ============== THE ONE LEVER THAT MOVED IT, AND WHY IT IS BIG ==============
 * THE MASKED CHARACTER AND THE TESTED CHARACTER MUST BE THE SAME VARIABLE.
 * 350 -> 279, and it is the whole reason this target is at 276 rather than 350.
 *
 *     wrong   while ((ch = *text++) != 0) { op = ch; ... op &= 0xff; ... }
 *     right   while ((op = *text++) != 0) { ch = op; ... op &= 0xff; ... }
 *
 * Both spellings are semantically identical and the ROM is unambiguous about
 * which it wants -- `ldrh r1, [r6] / add r6, #2 / mov r12, r1 / cmp r1, #0`
 * loads into the tested register and COPIES to r12.  What the wrong spelling
 * costs is not the copy.  It is that the glyph block's whole 14-instruction head
 * gets SPECULATIVELY MOVED ABOVE THE `cmp r1, #0x1e` BRANCH, and that in turn
 * forces the glyph pointer onto the stack, which adds a sixth frame word
 * (`sub sp, #0x48` against the ROM's `#0x44`), which shifts every `[sp, #N]` in
 * the function.
 *
 * THE PASS IS `if_convert`, AND I LOCATED IT PRECISELY -- this is the durable
 * part of this park.  A `-da` sweep over the dumps puts the `(and ... 255)` insn
 * AFTER the `cmp ... 30` in `.13.combine` and BEFORE it in `.14.ce`, so the mover
 * is ifcvt.  The `.14.ce` dump names the routine: "IF-CASE-2 found, start 5,
 * else 13 / Conversion succeeded", i.e. `find_if_case_2` at ifcvt.c:1721 calling
 * `dead_or_predicable` at ifcvt.c:1831 with reversep = 0.  With no conditional
 * execution on thumb, `dead_or_predicable` takes the branch at ifcvt.c:1903 and
 * moves the block anyway provided (a) it contains no call, no trapping insn and
 * NO MEMORY REFERENCE AT ALL (the `for_each_rtx (..., find_memory, ...)` test --
 * note this runs BEFORE reload, so the spill `ldr`s you see in the final asm do
 * not count), and (b) the registers the block SETS are neither live nor set in
 * the test range.  Test (b) is the one the fix reaches: with one variable, the
 * block's `and` writes the very pseudo the `cmp` reads, MERGE_SET meets
 * TEST_LIVE, and the conversion is cancelled.  With two variables copy
 * propagation gives the compare a different pseudo and nothing intersects.
 *
 * THIS IS UNREACHABLE BY FLAG.  `-fno-gcse` leaves the motion in place (verified
 * by grepping the generated asm, not just by the score -- and `-fno-gcse` is a
 * 21-instruction regression on its own), and so do `-fno-cse-skip-blocks` and
 * `-fno-schedule-insns2`.  There is no `-fno-if-conversion`: batch 293 already
 * verified that toplev.c's two "if-conversion" hits are comments at the
 * unguarded call sites with no `f_options` row.  So the ONLY handle on this
 * class is the source, and the two gates a source change can reach are
 * `count_bb_insns (then_bb) > BRANCH_COST` at ifcvt.c:1763 -- note it counts
 * THEN, not ELSE, despite its "ELSE is small" comment -- and the
 * MERGE_SET/TEST_LIVE intersection.  Worth putting in docs/elevation.md: the
 * register-allocation blocker class has a member that is really a CFG pass, and
 * "use one variable where the ROM uses one variable" is its lever.
 *
 * ============ THE OTHER THINGS THAT WERE LOAD-BEARING, MEASURED ============
 *   - BRANCH POLARITY, both of them, read off the ROM and not guessed.
 *     `if (pal == 0xf000)` (the `ldrh`/3/0x04040404 arm) and
 *     `if (op <= 0x1e) { switch } else { glyph }`, so the switch is the
 *     fall-through and the glyph block is the branch target.  Written the other
 *     way round: 441 against 276.
 *   - `unsigned` FOR THE CHARACTER.  The ROM's `cmp r1, #0x1e / bhi` and
 *     `cmp r3, #0x1a / bls` are unsigned; `int` gives `bgt`/`ble`.  2.
 *   - CASE BODIES IN THE ROM'S LAYOUT ORDER, which is the emit_case_nodes lever
 *     from the brief.  The dispatch table is sorted by value but the bodies come
 *     out in SOURCE order, and the ROM lays them out case 8, then 7/9/0xa, then
 *     3, then 0xe/0xf/0x1c, then 0xb/0xc/0x11/0x1d.  Written in value order the
 *     jump table entries point at the wrong labels.
 *   - THE 0xe/0xf/0x1c CASE FALLS THROUGH INTO THE 0xb/0xc/0x11/0x1d CASE.
 *     The ROM is `.L1dfb0: add r6, #2` falling into `.L1dfb2: add r6, #2`; two
 *     separate `text++` statements fold to `add r6, #4` plus a branch.  1.
 *   - AN `int` CARRIER FOR THE 0xf IN CASES 7/9/0xa, and the SAME carrier used as
 *     the table index.  276 against 277.  The ROM has
 *     `mov r2, #0xf / strh r2, [r3] / ldrb r3, [r3, r2]`: the HImode store of a
 *     literal pools without the carrier (`ldr r3, =0xf`), and the index has to be
 *     the same register or the load becomes `ldrb r3, [r3, #0xf]`.  Two birds.
 *   - THE TWO FUNCTION POINTERS HAVE OPPOSITE RETURN TYPES, read off the ROM per
 *     the recorded rule in src/rom_c9000/rom_cd508_a_b.c.  Func_80008d4 has r0
 *     filled LAST (`mov r1, #0x10 / ldr r3, =Func_80008d4 / add r0, sp, #0x14`)
 *     so it is `int (*)(void *, s32)`; Func_80008d8 has r0 filled FIRST
 *     (`ldr r0, [sp, #8] / ldr r3, =Func_80008d8 / mov r1, r5`) so it is
 *     `void (*)(void *, u32, u32)`.  The brief's warning that the int-return
 *     lever runs BACKWARDS in rom_15000 is right about the bank and wrong as a
 *     blanket rule: one callee in this single function wants each direction.
 *   - `u8 tab[0x2c]` rather than `[0x30]`.  277 against 279.  This is a
 *     WORKAROUND, not a reading: 0x2c makes the total frame 0x44 like the ROM's,
 *     but only because our spill area is one word wider, so the table still
 *     starts at sp+0x18 where the ROM has sp+0x14.  If the extra spill word is
 *     ever removed this must go back to 0x30.
 *
 * ================== MEASURED AND REJECTED ==================
 *   spelling                                                       --align
 *   -------------------------------------------------------------- -------
 *   first candidate, natural C, both branches inverted                  470
 *   branch polarity fixed                                               350
 *   the one-variable character fix (KEPT)                               279
 *   tab[0x2c] (KEPT)                                                    277
 *   the 0xf int carrier (KEPT)                                          276
 *   a second table pointer `t2 = t` for the first nybble lookup          276 (inert)
 *   `L371b4[0xf & a]` / `t[0xf & w]` operand order                      277 / 277
 *   glyph as the THEN arm and the switch as the ELSE                    441
 *   `u8 *volatile buf`                                                  411
 *   + `volatile` on font and pal as well                                417
 *   + `volatile` on dst1/dst2 too                                       433
 *   `-fno-gcse`                                                         329 (from the 350 base)
 *   `-fno-schedule-insns2`                                              370
 *
 * The `volatile` row is worth keeping: the ROM DOES reload buf from [sp+8] and
 * font from [sp+4] at each use, so `volatile` looks like the right description
 * of the spill -- but it also forces a reload inside both inner loops, where the
 * ROM keeps the value in a register, and costs 135.  `volatile` reproduces "this
 * lives in memory", never "this is spilled at these particular points".
 *
 * ================== WHAT IS LEFT, AND HOW FAR ==================
 * The instruction COUNT is 452 against 456 and the SIZE is 20 bytes short, i.e.
 * four instructions and three pool words.  One blocker accounts for most of the
 * 276:
 *
 * THE GLYPH POINTER IS SPILLED AND THE ROM KEEPS IT IN r5.  In the inner
 * rasteriser loop the ROM has `ldmia r5!, {r2}`; we have
 * `ldr r4, [sp] / ldmia r4!, {r2} / mov r3, r4 / str r3, [sp]` -- three extra
 * instructions on the hottest path in the function, inside two nested loops.
 * The cause is one extra live quantity: loop.c hoists the inner loop's `0xf`
 * mask into a register in the glyph head (`mov r2, #0xf / mov r14, r2`), where
 * the ROM rematerialises `mov r7, #0xf` at BOTH uses inside the loop and spends
 * r14 on a second copy of the colour-map pointer instead.  With 0xf resident
 * there is one register too few and the glyph pointer loses.  So the ROM's build
 * had MORE register pressure than ours, not less, and the fix is to make gcc
 * spend a register the way the ROM does -- add the second map pointer (measured
 * inert on its own, so this is a COUPLED PAIR in exactly the sense the brief
 * warns about, and the two must be tried together with whatever prevents the
 * 0xf from being hoisted).  I did not find a way to stop the hoist: the mask is
 * a genuine loop invariant and loop.c is not flag-gated here.
 *
 * SECOND, SMALLER: the ROM computes `&tab` FRESH in each arm of the palette test
 * (`add r3, sp, #0x14 / mov r10, r3` in one, `mov r7, #0x14 / add r7, sp` in the
 * other, with `mov r10, r7` at the end), and gcc computes it once before the
 * branch.  That is the same one-variable-per-region shape as brief H's target 2
 * lever #1, and the second arm writing through its OWN pointer and only then
 * assigning the shared one is already in this candidate; what is still missing is
 * that gcc will not build the address twice.
 *
 * HONEST ASSESSMENT: reachable, but not in one more pass.  The structure is done
 * and the remaining residue is one identified pressure problem plus its
 * cascade.  A batch that gives this function its own brief, starts from this
 * file, and spends itself on the glyph-loop allocation with `.17.lreg` /
 * `.18.greg` dumps in hand has a real chance; a four-target slot does not.
 */
#include "gba/types.h"

extern u8 *iwram_3001e8c;
extern u8 L371b4[] __asm__(".L371b4");
extern u8 Data_370d4[];
extern int _FILE_13;

extern void *Func_8004938(u32 size);
extern void *GetFile(int id);
extern int Func_80008d4(void *dst, s32 len);
extern void Func_80008d8(void *dst, u32 len, u32 val);
extern void free(void *p);

int Func_801de5c(u16 *text, u16 *dst1, u16 *dst2, int pen)
{
    u8 *st;
    u8 *buf;
    u8 *font;
    u8 *t;
    u8 *p;
    u8 *g;
    u8 *fp8;
    u16 *curp;
    u32 pal;
    u32 sz;
    u32 w;
    u32 a;
    u32 b;
    u32 cells;
    u32 lim;
    u32 idx;
    u32 f;
    u32 ch;
    u32 op;
    int n;
    int row;
    int k;
    int c;
    int i;
    int adv;
    int (*fp1)(void *, s32);
    void (*fp2)(void *, u32, u32);
    u8 tab[0x2c];

    st = iwram_3001e8c;
    sz = 0x80;
    sz <<= 4;
    buf = Func_8004938(sz);
    font = GetFile((int)(&_FILE_13));
    pal = st[0xea7];
    pal <<= 12;
    fp1 = Func_80008d4;
    fp1(tab, 0x10);
    if (pal == (u32)(0xf0 << 8)) {
        t = tab;
        t[1] = L371b4[*(u16 *)(st + 0xeae) & 0xf];
        t[3] = 3;
        fp2 = Func_80008d8;
        fp2(buf, sz, 0x4040404);
    } else {
        p = tab;
        p[1] = L371b4[st[0xeae] & 0xf];
        p[3] = 1;
        fp2 = Func_80008d8;
        fp2(buf, sz, 0xe0e0e0e);
        t = p;
    }
    if (text != 0) {
        while ((op = *text++) != 0) {
            ch = op;
            if (op <= 0x1e) {
                switch (op) {
                case 8:
                    a = *text;
                    *(u16 *)(st + 0xeae) = a;
                    text++;
                    t[1] = L371b4[a & 0xf];
                    break;
                case 7:
                case 9:
                case 0xa:
                    k = 0xf;
                    *(u16 *)(st + 0xeae) = k;
                    t[1] = L371b4[k];
                    break;
                case 3:
                    pen += Data_370d4[0];
                    break;
                case 0xe:
                case 0xf:
                case 0x1c:
                    text++;
                case 0xb:
                case 0xc:
                case 0x11:
                case 0x1d:
                    text++;
                    break;
                }
            } else {
                op &= 0xff;
                g = font + (op << 5);
                p = buf + pen;
                row = 0;
                do {
                    w = *(u32 *)g;
                    g += 4;
                    k = 3;
                    do {
                        c = t[w & 0xf];
                        if (c != 0)
                            *p = c;
                        w >>= 4;
                        c = t[w & 0xf];
                        p++;
                        if (c != 0)
                            *p = c;
                        k--;
                        p++;
                        w >>= 4;
                    } while (k >= 0);
                    row++;
                    p += 0xf8;
                } while (row <= 7);
                if (ch == 0xf01d || ch == 0xf01f) {
                    pen += 8;
                } else {
                    if (ch == 0xf01e)
                        adv = 3;
                    else if (op > 0x1f)
                        adv = Data_370d4[op - 0x20];
                    else
                        adv = 1;
                    pen += adv;
                }
            }
        }
    }
    cells = (u32)(pen + 7) >> 3;
    p = buf;
    fp8 = buf;
    n = 7;
    do {
        if (cells != 0) {
            i = cells;
            do {
                a = *(u32 *)fp8;
                b = *(u32 *)(fp8 + 4);
                a |= a << 4;
                b |= b >> 4;
                a &= 0xff00ff0;
                b &= 0xff00ff;
                a |= a << 8;
                b |= b >> 8;
                i--;
                fp8 += 8;
                *(u32 *)p = ((a << 4) >> 16) | (b << 16);
                p += 4;
            } while (i != 0);
        }
        p = (p - cells * 4) + (0x80 << 1);
        fp8 = (fp8 - cells * 8) + (0x80 << 1);
        n--;
    } while (n >= 0);
    if (cells != 0) {
        curp = (u16 *)(st + (0xea << 4));
        p = buf;
        n = cells;
        do {
            f = st[0xea2];
            lim = 0x7f;
            if (f != 0)
                lim = 0xff;
            idx = *dst1 & 0x3ff;
            if ((u32)(idx - 0x80) > 0x7f) {
                if (f == 0 || idx < (u32)(0x80 << 2) || idx >= (u32)(0xa0 << 2)) {
                    idx = *curp & lim;
                    i = 0;
                    if (st[idx + (0xda << 4)] != 0) {
                        do {
                            idx++;
                            i++;
                            idx &= lim;
                            if ((u32)i > lim)
                                break;
                        } while (st[idx + (0xda << 4)] != 0);
                    }
                    *curp = (idx + 1) & lim;
                    st[idx + (0xda << 4)] = 1;
                    if (idx <= 0x7f)
                        idx |= 0x80;
                    else
                        idx += 0xc0 << 1;
                    a = pal | idx;
                    *dst1 = a;
                    *dst2 = a;
                }
            }
            *(u32 *)((idx << 5) + 0x6000000) = *(u32 *)p;
            *(u32 *)((idx << 5) + 0x6000004) = *(u32 *)(p + (0x80 << 1));
            *(u32 *)((idx << 5) + 0x6000008) = *(u32 *)(p + (0x80 << 2));
            *(u32 *)((idx << 5) + 0x600000c) = *(u32 *)(p + (0xc0 << 2));
            *(u32 *)((idx << 5) + 0x6000010) = *(u32 *)(p + (0x80 << 3));
            *(u32 *)((idx << 5) + 0x6000014) = *(u32 *)(p + (0xa0 << 3));
            *(u32 *)((idx << 5) + 0x6000018) = *(u32 *)(p + (0xc0 << 3));
            *(u32 *)((idx << 5) + 0x600001c) = *(u32 *)(p + (0xe0 << 3));
            n--;
            dst1++;
            dst2++;
            p += 4;
        } while (n != 0);
    }
    free(buf);
    return cells;
}
