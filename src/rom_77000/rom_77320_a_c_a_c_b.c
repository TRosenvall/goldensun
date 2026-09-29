/* GameInit -- 0x08077d38.  EXACT.
 *
 *     OK GameInit -- 520 bytes, 231 encodings and 11 relocations identical
 *
 * SIZE 520 bytes (0x208; 0x08077d38 + 0x208 = 0x08077f40 = Func_8077f40, the first
 * function of the next TU, so the size is confirmed against the ROM layout).
 * INSTRUCTION COUNT 203 (the two figures are separate: tryc reports 207 "lines",
 * which is 203 instructions plus the four labels L0..L3).
 * SHIMS: 0.  `python3 tools/shimcount.py` reports no register pins, no `.equ`,
 * no "+r" barrier, no empty asm -- a pin-free match, so no fakematch.txt row.
 *
 * SPLIT.  asm/rom_77000/rom_77320_a_c_a_c.s holds TWO functions (verified with the
 * anchored `^[[:space:]]*\.?thumb_func_start(_noalign)?[[:space:]]+` pattern):
 * Func_8077cb8 at line 8 and GameInit at line 71.  Landing GameInit therefore needs
 *
 *     asm/rom_77000/rom_77320_a_c_a_c_a.s   hand-written, Func_8077cb8 only
 *                                           (keep the two .include lines)
 *     src/rom_77000/rom_77320_a_c_a_c_b.c   this file (GameInit)
 *
 * and stage1.ld:653 `asm/rom_77000/rom_77320_a_c_a_c.o(.text)` split into
 * `..._a_c_a_c_a.o(.text)` then `..._a_c_a_c_b.o(.text)`, in that order.
 * `python3 tools/datacheck.py asm/rom_77000/rom_77320_a_c_a_c.s` is SILENT: the file
 * carries no data section and no required `.global` data export, so nothing is lost
 * when the hand-written half is replaced.
 *
 * WHAT THE FUNCTION DOES.  Four DMA3 zero-fills from one stack word -- gState 0x2c0,
 * ewram_2001000 0xf84, gFlags 0x200, gPartyStatus 0xa60 (each control word is
 * 0x85000000 | size/4) -- with a DMA3 busy-wait between the second and third, then
 * ResetPCs, then the new-game seeding of gState, AddPartyMember(0), and the packed
 * table pointer from Func_8077cb8 stored at gState+0x2b8.  (The reference .s prose
 * calls it "BuildPartyTables"; the ROM symbol is GameInit and that is what is used.)
 *
 * FIVE LEVERS, each measured alone.  Baseline: plain pointer arithmetic on
 * `unsigned char gState[]` with literal values and four DMA3_CLEAR calls --
 * 101 instructions in disagreeing regions of 207, frame `sub sp, #0x14`.
 *
 * 1. ONE SHARED FILL WORD, VIA DMA3_SET RATHER THAN FOUR DMA3_CLEARs.  dma.h's
 *    DMA3_CLEAR declares its own `u32 value`, and gcc-2.96 does NOT share the slot
 *    across four inlines: 4*4 + 4 (the spilled zero) = 0x14 against the ROM's 0x8.
 *    Hoisting the word into the caller and calling DMA3_SET(vp, dst, cnt) with the
 *    raw control word gives `sub sp, #0x8` and the ROM's `str r4,[r5]` /
 *    `stmia r3!,{r0,r1,r2}` / `sub r3,#0xc` shape.  101 -> 91.
 *
 * 2. THE gState WRITES MUST BE STRUCT MEMBERS, NOT POINTER ARITHMETIC.  This is
 *    docs/elevation.md "Putting an offset in the TYPE also stops the constant-
 *    derivation chain", and it fixed BOTH halves of that entry here at once:
 *      - a halfword store of a literal pools the value (`ldr r5,=0x1`) where the ROM
 *        has `mov r3,#0x1`, because the store is to a `u16`;
 *      - and pointer arithmetic makes gcc chain the offsets (`add r2,#0x4` to get
 *        0x214 from 0x210) where the ROM builds each independently.
 *    Members at their real offsets give the ROM's `mov r3,#0x84 / lsl r3,#0x2` for
 *    every multiple-of-four offset, the pool for 0x212 and 0x216, AND the ROM's own
 *    derivations that survive (`sub r3,#0x18` for the VALUE 0x200 out of the offset
 *    0x218, `add r3,#0x1a`, `add r2,#0x6`, `sub r1,#0x2c`).  91 -> 37, and the
 *    r8/r10 assignment of the four pooled QImode constants 0/1/4/8 fell out with it.
 *    NEGATIVE, measured first: an `int` local for each halfword value fixes the
 *    pooling but makes the derivation worse -- 176 in disagreeing regions -- exactly
 *    as that entry predicts ("no arrangement of int locals stops the derivation").
 *
 * 3. `iwram_3001d08` MUST BE `volatile`.  The ROM writes it and then RELOADS it with
 *    `ldrb r2,[r3]` for the gState+0x22a copy; a plain `unsigned char` lets cse fold
 *    the reload into the stored register, which is the one MISSING instruction (206
 *    against 207).  The encodings of `strb`/`ldrb` are unchanged by the qualifier.
 *
 * 4. THE -1 MUST GO TO A SIGNED `short`.  The ROM's pool word is `.word 0xffffffff`;
 *    `unsigned short` truncates at the front end and gives `.word 0xffff`.  This is
 *    docs/elevation.md "the sign follows the C type, not the mode" (batch 258).
 *
 * 5. `value` MUST BE A `volatile u32` READ AND WRITTEN THROUGH A NON-VOLATILE `u32 *`.
 *    Named blocker, read out of the -da dumps: cand6.c.03.cse turns
 *    `(set (mem:SI (reg/v:SI 34)) ...)` into
 *    `(set (mem:SI (addressof:SI (reg/v:SI 36) 33)) ...)` for the two stores in the
 *    FIRST basic block, and 04.addressof then lowers it to `(plus sfp 4)`, giving
 *    `str r4,[sp,#0x4]` where the ROM has `str r4,[r5]`.  (The last two stores escape
 *    because cse's table is reset at the loop.)  A `volatile` DECL keeps the local out
 *    of the ADDRESSOF promotion; the access must stay NON-volatile, because a volatile
 *    store instead leaves the address folded and only kills the separate cse of
 *    `&REG_DMA3SAD`.  As a bonus this is also what makes the busy-wait reload
 *    `ldr r2,=0x40000d4` instead of reusing r3 from the preceding `stmia`.
 *    4 -> 2 in disagreeing regions.  `u32 value[1]` does NOT work (gcc-2.96 still
 *    builds an ADDRESSOF for a one-element array); nor does a one-member struct.
 *
 * 6. THE 0xff STORE MUST WALK A NAMED POINTER.  `e = ewram_2001000; e += 0x104;
 *    *e = 0xff;` -- the sibling park src/non_matching/rom_15000/801f818.c lever 1.
 *    `ewram_2001000[0x104] = 0xff` gives the same six instructions there but leaves
 *    `mov r0,r5` two slots late in the FOURTH DMA3_SET.  2 -> 0.  EXACT.
 *
 * NEGATIVES MEASURED, so nobody re-runs them: `*(unsigned char *)(ewram_2001000 +
 * 0x104)` and a named `int` offset (`off = 0x82 << 1`) both regress to 20; four
 * orderings of the local declarations, a second pointer variable for the fourth
 * fill, and dropping the `dma` local all leave the same 2; a file-local DMA3_CLEAR-
 * shaped inline that binds r0 before the store gets the fourth site right and breaks
 * the other three (16).
 *
 * Verified with
 *   tools/objcmp.py <this file> asm/rom_77000/rom_77320_a_c_a_c.s --func GameInit
 * `make compare` is still the authority for the pool PLACEMENT -- the reference keeps
 * three mid-body pools behind `b` branches, which gcc-2.96 reproduces on its own
 * (docs/elevation.md "SETTLED: the branch-over-pool shape is NOT a blocker"), and
 * objcmp's byte-for-byte OK on all 520 bytes covers them.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct State {
    /* 0x000 */ u32 f000;
    /* 0x004 */ u32 f004;
    /* 0x008 */ u8 pad008[0x010 - 0x008];
    /* 0x010 */ u32 f010;
    /* 0x014 */ u8 pad014[0x11d - 0x014];
    /* 0x11d */ u8 f11d;
    /* 0x11e */ u8 f11e;
    /* 0x11f */ u8 f11f;
    /* 0x120 */ u8 f120;
    /* 0x121 */ u8 f121;
    /* 0x122 */ u8 f122;
    /* 0x123 */ u8 f123;
    /* 0x124 */ u8 f124;
    /* 0x125 */ u8 f125;
    /* 0x126 */ u8 f126;
    /* 0x127 */ u8 f127;
    /* 0x128 */ u8 f128;
    /* 0x129 */ u8 f129;
    /* 0x12a */ u8 f12a;
    /* 0x12b */ u8 f12b;
    /* 0x12c */ u8 pad12c[0x1f4 - 0x12c];
    /* 0x1f4 */ u32 f1f4;
    /* 0x1f8 */ u8 pad1f8[0x205 - 0x1f8];
    /* 0x205 */ u8 f205;
    /* 0x206 */ u8 f206;
    /* 0x207 */ u8 pad207[0x20a - 0x207];
    /* 0x20a */ u8 f20a;
    /* 0x20b */ u8 f20b;
    /* 0x20c */ u8 f20c;
    /* 0x20d */ u8 pad20d[0x210 - 0x20d];
    /* 0x210 */ u16 f210;
    /* 0x212 */ u16 f212;
    /* 0x214 */ u16 f214;
    /* 0x216 */ u16 f216;
    /* 0x218 */ u16 f218;
    /* 0x21a */ u16 f21a;
    /* 0x21c */ u16 f21c;
    /* 0x21e */ u16 f21e;
    /* 0x220 */ u16 f220;
    /* 0x222 */ u16 f222;
    /* 0x224 */ u8 pad224[0x22a - 0x224];
    /* 0x22a */ u8 f22a;
    /* 0x22b */ u8 pad22b[0x2b8 - 0x22b];
    /* 0x2b8 */ u32 f2b8;
};

extern struct State gState;
extern unsigned char gFlags[];
extern unsigned char gPartyStatus[];
extern unsigned char ewram_2001000[];
extern short ewram_2002004;
extern int iwram_3001c9c;
extern volatile unsigned char iwram_3001d08;
extern short iwram_3001d24;

extern void ResetPCs(void);
extern int AddPartyMember(int id);
extern int Func_8077cb8(void);

void GameInit(void)
{
    vu32 *dma;
    volatile u32 value;
    u32 *vp;
    struct State *g;
    unsigned char *e;

    vp = (u32 *)&value;
    *vp = 0;
    DMA3_SET(vp, &gState, 0x850000b0);
    *vp = 0;
    DMA3_SET(vp, ewram_2001000, 0x850003e1);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    *vp = 0;
    DMA3_SET(vp, gFlags, 0x85000080);
    e = ewram_2001000;
    e += 0x104;
    *e = 0xff;
    *vp = 0;
    DMA3_SET(vp, gPartyStatus, 0x85000298);
    ResetPCs();
    g = &gState;
    g->f210 = 1;
    g->f212 = 2;
    g->f214 = 4;
    g->f216 = 8;
    g->f218 = 0x200;
    g->f21a = 0x100;
    g->f21c = 2;
    g->f220 = 0;
    g->f222 = 0;
    g->f1f4 = 0;
    AddPartyMember(0);
    g->f010 = 0;
    g->f20c = 1;
    g->f20a = 1;
    g->f20b = 1;
    g->f205 = 0;
    g->f206 = 8;
    g->f000 = 0;
    g->f2b8 = Func_8077cb8();
    iwram_3001c9c = 0;
    iwram_3001d08 = 0;
    g->f004 = 0;
    g->f22a = iwram_3001d08;
    iwram_3001d24 = 0;
    ewram_2002004 = -1;
    g->f11d = 4;
    g->f11e = 4;
    g->f11f = 4;
    g->f120 = 8;
    g->f121 = 8;
    g->f122 = 8;
    g->f123 = 0x10;
    g->f124 = 0x10;
    g->f125 = 0x10;
    g->f126 = 0x20;
    g->f127 = 0x20;
    g->f128 = 0x20;
    g->f129 = 0x40;
    g->f12a = 0x40;
    g->f12b = 0x40;
}
