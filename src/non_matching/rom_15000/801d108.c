/* Func_801d108 (DrawDialScreen) -- 0x0801d108, asm/rom_15000/rom_1ca1c_c_c_a_a_a.s
 *
 * NON-MATCHING, 337 of 416 encodings differ.
 *
 * SIZE  ref 964 bytes, ours 944   (ours is 20 bytes / 10 instructions SHORT)
 * COUNT ref 416 encodings, ours 406.  The reference's own instruction count is
 *       388 (350 halfword instructions + 38 `bl`); objcmp's 416 counts the 28
 *       pool words as encodings too.  Because the counts are NOT equal objcmp's
 *       337 IS SATURATED and ranks nothing -- use tools/aligncmp.py, on which
 *       this candidate reads aligned-equal 289 (69.5% of ref), 145 in 78 hunks.
 * RELOCATIONS: the SEQUENCE IS EXACT.  All 38 `bl` targets and all 7 ABS32 pool
 *       symbols are the ROM's, in the ROM's order; only their byte offsets move,
 *       which is downstream of the 10 missing instructions.  `XX RELOCATIONS
 *       differ` here is offsets only -- read the symbol sequence, not the flag.
 *
 * NO SHIMS, NO PINS, NO asm (tools/shimcount.py: silent).  No fakematch row.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801d108.c \
 *     asm/rom_15000/rom_1ca1c_c_c_a_a_a.s --func Func_801d108
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_15000/801d108.c \
 *     asm/rom_15000/rom_1ca1c_c_c_a_a_a.s Func_801d108 -v
 *
 * SPLIT SHAPE.  tools/datacheck.py is SILENT on rom_1ca1c_c_c_a_a.s: no data
 * section, so this is a plain function cut, not a text/data split.  The .s holds
 * three functions and Func_801d108 is the FIRST:
 *     src/rom_15000/rom_1ca1c_c_c_a_a_a.c   Func_801d108
 *     asm/rom_15000/rom_1ca1c_c_c_a_a_b.s   Menu_Settings, Func_801d94c
 * stage1.ld line 468 becomes those two in that order.  NO exports.s change:
 * `.thumb_func_start` already emits `.global` for all three, so Menu_Settings's
 * `bl Func_801d108` (line 433 of the reference) resolves across the new object
 * boundary against the C definition, and Func_801d94c -- which other objects
 * reach as a `.word` -- stays global in the tail .s.
 *
 * THE REFERENCE'S PROSE IS WRONG ABOUT ITS OWN FUNCTION, in two ways.
 *   * "fades with Func_1e41c / Func_1e7c0 / Func_1eadc".  None of the three is a
 *     fade.  Func_801e41c draws the box's horizontal dividers (four calls at
 *     y = 2, 4, 7, 0xA), Func_801e7c0 draws message text (six calls, message ids
 *     0xC07, 0xC08, 0xC0D, 0xC0E, 0xC0F, 0xC12), and Func_801eadc attaches an OAM
 *     record to the box (eight calls).  The sibling parks agree:
 *     src/non_matching/rom_15000/801d9d4.c and 802106c.c both declare
 *     Func_801eadc as returning the sprite record.
 *   * "395 lines" is a LINE count offered where a reader wants an instruction
 *     count; the body is 388 instructions.  It is honest about the unit, but it
 *     is the same trap docs/elevation.md warns about, so it is worth saying that
 *     the two numbers here are 395 and 388.
 *   The prose also omits the whole second half: a `__divsi3` position algebra
 *   (`s->f594 * 60 / s->f599`) that places two sprites, and a 3x3 table of
 *   Func_8021750 handles at +0x5EC.
 *
 * WHAT THE FUNCTION ACTUALLY DOES, and where the structure came from.
 * It is the near-twin of Func_801d9d4 (DrawSubScreen), parked at
 * src/non_matching/rom_15000/801d9d4.c: same `iwram_3001ea0` state block (0x628
 * bytes, allocated by Func_801d980), same +0x5A4 sprite slot, same `.L367c9` /
 * `.L367cc` / `.L367ce` option tables, same Data_310a4 sprite graphic.  Five
 * things were taken from that park and all five held:
 *   1. `struct SubScr`'s +0x5A4 `void *obj` and the `pp = &s->obj; *pp = ...`
 *      shape.
 *   2. `box[6]` / `box[7]` (offsets 0xC and 0xE) are the box's tile x and y.
 *   3. `extern signed char L367c9[] __asm__(".L367c9");` -- the tree's spelling
 *      for a `.L` local label.  All three tables live in
 *      asm/rom_15000/rom_1ca1c_c_c_c.s and are ALREADY `.global` there (lines
 *      567-571), so no new export is needed -- they are ABS32 relocations in the
 *      reference object too.
 *   4. `if (slot <= 0x5f)` for the ROM's `cmp #0x5f / bgt`.
 *   5. The int-carrier lever, IN ITS OPPOSITE DIRECTION.  801d9d4 needed
 *      `int c = *p;` to keep `ldrb / lsl #24 / asr #24`; here the ROM really does
 *      use the register-offset `mov r0,#N / ldrsb r0,[r5,r0]`, so the table is
 *      read as `signed char` DIRECTLY.  Two functions, one table, two spellings
 *      -- exactly the contrast that park predicted.
 * New here: +0x5EC is a `void *tabs[3][3]`.  The ROM writes 0x5EC, 0x5F0, 0x5F4,
 * 0x5F8, 0x5FC, 0x604, 0x608 and SKIPS 0x600, which is what identifies it as
 * three rows of three rather than a flat array of seven; rows 1 and 2 are only
 * two-thirds filled.  gcc materialises 0x5F0/0x5F8/0x608 as `mov #0xBE/0xBF/0xC1
 * / lsl #3` and pools the rest, for free, because they are the multiples of 8.
 *
 * THE OAM RECORD IS A BITFIELD STRUCT, and that is measured, not assumed:
 *     unsigned short x:10;  unsigned short a:2;  unsigned short hi:4;
 * `s->x += 4` gives the ROM's `ldrh / lsl #22 / lsr #22 / add #4 / and 0x3FF /
 * and 0xFFFFFC00 / orr / strh` exactly, and `s->hi = 0xe` gives `ldrb [.,#0x19] /
 * and #0xF / orr #0xE0 / strb` while `s->hi = 0xf` gives `orr #0xF0` alone --
 * gcc drops the AND because the OR already covers the masked-out bits, which is
 * why blocks 1/2 and 3/4 look like different operations and are not.
 * The alternative spelling was tried and is WORSE: writing byte 0x19 and
 * halfword 0x18 through a raw `unsigned char *` reads 49.5% aligned and goes
 * 421 encodings OVER (scratch_elev/b300h/d108_V4.c).  So is `volatile` on the
 * same struct: 65.6%.
 *
 * THE RESIDUE, ITEMISED.  The 10 missing instructions are all of one kind, a
 * COPY OUT OF r0 THAT gcc DELETES AND THE ROM KEEPS:
 *   (a) 6 x `mov r4, r0` after `bl Func_801eadc`.  The ROM names the returned
 *       record in r4 and addresses it as `[r4, #0x19]` / `[r4, #0x18]` /
 *       `[r4, #0x15]`; we address `[r0, ...]` directly.
 *   (b) 1 x `mov r7, r3` in the first sprite block: the ROM computes
 *       `bx->y * 8` into r3 and copies to r7 before `add r7, #0xc`, two pseudos
 *       where we coalesce to one.  Note it is NOT a spelling difference between
 *       the three sprite blocks -- block 3 writes the same source as
 *       `add r7, r3, #4`, one instruction, because +4 fits Thumb's 3-bit
 *       `adds rd, rn, #imm3` and +0xC / +0x14 do not.
 *   (c) 2 x the subreg spill in the LAST bitfield write: the ROM stores the
 *       computed halfword container to `[sp, #4]` and reloads it with
 *       `add r2, sp, #4 / ldrh r2, [r2]` (Thumb `ldrh` has no sp-relative form),
 *       where we store it straight with `strh`.  This is also the whole of the
 *       `sub sp, #8` vs our `sub sp, #4`: the ROM's frame is one outgoing
 *       argument word plus one spill slot.  The IDENTICAL sequence 90 bytes
 *       earlier does NOT spill, which is what makes this reload pressure and not
 *       a source difference -- forcing it with a union scores 32.2% and 420
 *       encodings (scratch_elev/b300h/d108_V3.c), so the union is not the answer.
 *   (d) 1 x a reload of `ldr =0xFFFFFC00`: the ROM rematerialises it in the
 *       fourth block where we keep the first block's copy in r6.
 * Everything else that differs is a REGISTER NUMBER at equal count: `slot` is
 * r6 in the ROM and r7 in ours at all four AllocSpriteSlot sites, and the
 * outgoing 5th-argument constants 0 / 0x10 take the register `slot` vacated.
 *
 * BLOCKER: LOCAL-ALLOC TIES THE CALL RESULT TO r0.  PASS .17.lreg.
 * The `-da` dumps say this is not global-alloc: only nine pseudos reach
 * `.18.greg` ("9 regs to allocate: 35 41 40 38 39 36 33 37 32") and none of them
 * is a sprite pointer.  88 `(set (reg:SI N) (reg:SI 0 r0))` copies survive
 * `.15.regmove` into `.17.lreg`, and local-alloc's copy preference then gives
 * those quantities r0 -- `;; Register 48 in 0.` and `;; Register 49 in 0.` are
 * two of them.  For the ROM's r4 local-alloc has to REFUSE r0, and r0 is free at
 * every one of the six sites, so nothing in the source reaches it.  This is the
 * corpus's REG_ALLOC_ORDER class ({3,2,1,0,12,14,4,5,6,7,...}, no Thumb
 * override) seen from the copy-elimination side.
 *
 * MEASURED AND INERT, so nobody need repeat them (all against 69.5%):
 *   separate `struct Spr *` per call site instead of one variable      0
 *   an `int` carrier for the record pointer (`t = (int)f(); p = (Spr*)t`)  0
 *   `slot` declared first / last / before x,y -- four orders           0
 *   `_Func_80b0a20(pp, x, y + 0xc)` instead of `y += 0xc` first        0 (-1 hunk)
 *   -fno-gcse                                                          0
 *   -fno-rerun-cse-after-loop                                          0
 *   -fno-strict-aliasing                                       0 (9 fewer lines
 *                                                       differ, same alignment)
 * MEASURED AND WORSE:
 *   -fno-schedule-insns2                                     69.5 -> 61.5%
 *   `volatile` on the OAM struct                              69.5 -> 65.6%
 *   raw `unsigned char *` record, no bitfields                69.5 -> 49.5%
 *   a union for the halfword container                        69.5 -> 32.2%
 *
 * NEXT.  (a) is 6 of the 10 and everything else is downstream of it.  The thing
 * to find is a source shape that makes local-alloc reject r0 for a pseudo whose
 * only definition is a call return -- or a demonstration that gcc-2.96 cannot,
 * in which case this joins the copy-deletion class and the entry belongs in
 * docs/elevation.md rather than here.
 */
#include "dma.h"

struct Box { unsigned char pad[0xc]; unsigned short x; unsigned short y; };

struct Spr {
    unsigned char pad0[0x15];
    unsigned char f15;
    unsigned char pad1[2];
    unsigned short x:10;
    unsigned short a:2;
    unsigned short hi:4;
};

struct DialScr {
    unsigned char pad0[0x594];
    signed char f594;
    signed char f595;
    unsigned char pad1[3];
    signed char f599;
    signed char f59a;
    unsigned char pad2[0x5a4 - 0x59b];
    void *obj;
    unsigned char pad3[0x5b4 - 0x5a8];
    void *o5b4;
    unsigned char pad4[0x5c4 - 0x5b8];
    void *o5c4;
    unsigned char pad5[0x5ec - 0x5c8];
    void *tabs[3][3];
};

extern struct DialScr *iwram_3001ea0;
extern signed char L367c9[] __asm__(".L367c9");
extern signed char L367cc[] __asm__(".L367cc");
extern signed char L367ce[] __asm__(".L367ce");
extern unsigned char Data_310a4[];
extern unsigned char Data_73812[];
extern unsigned char Data_29910[];
extern unsigned char *CreateUIBox(int x, int y, int w, int h, int mode);
extern void Func_801e41c(void *box, int a, int b, int c, int d);
extern void Func_801e7c0(int id, void *box, int x, int y);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern struct Spr *Func_801eadc(int slot, unsigned int m, void *box, int d, int e);
extern void _Func_80b0a20(void *p, int x, int y);
extern void *Func_8021750(int a, int b, void *box, int c, int d);

unsigned char *Func_801d108(void)
{
    struct DialScr *s;
    unsigned char *box;
    struct Box *bx;
    struct Spr *spr;
    void **pp;
    signed char *t;
    int m, slot, x, y;

    s = iwram_3001ea0;
    box = CreateUIBox(1, 5, 0x1c, 0xe, 2);
    Func_801e41c(box, 0, 2, 0x1b, 2);
    Func_801e41c(box, 0, 4, 0x1b, 4);
    Func_801e41c(box, 0, 7, 0x1b, 7);
    Func_801e41c(box, 0, 0xa, 0x1b, 0xa);
    m = 0xc07;
    Func_801e7c0(m, box, 8, 0);
    m++;
    Func_801e7c0(m, box, 8, 0x10);
    m = 0xc0d;
    Func_801e7c0(m, box, 8, 0x20);
    m++;
    Func_801e7c0(m, box, 0x20, 0x28);
    Func_801e7c0(0xc0f, box, 8, 0x40);
    Func_801e7c0(0xc12, box, 8, 0x58);

    slot = AllocSpriteSlot();
    if (slot <= 0x5f) {
        UploadSpriteGFX(slot, 0x80, Data_310a4);
        spr = Func_801eadc(slot, 0x40000000, box, 0, 0);
        pp = &s->obj;
        *pp = spr;
        bx = (struct Box *)box;
        x = bx->x * 8;
        y = bx->y * 8;
        y += 0xc;
        _Func_80b0a20(pp, x, y);
    }

    slot = AllocSpriteSlot();
    if (slot <= 0x5f) {
        unsigned int mode;
        DMA3_COPY16(Data_73812, (void *)0x50003c0, 0x80);
        UploadSpriteGFX(slot, 0x80 << 1, Data_29910);
        mode = 0x40004000;
        spr = Func_801eadc(slot, mode, box, 0x86, 0);
        spr->hi = 0xe;
        spr = Func_801eadc(slot, mode, box, 0xa6, 0);
        spr->x += 4;
        spr->hi = 0xe;
        spr = Func_801eadc(slot, mode, box, 0x86, 0x10);
        spr->hi = 0xf;
        spr = Func_801eadc(slot, mode, box, 0xa6, 0x10);
        spr->x += 4;
        spr->hi = 0xf;
    }

    slot = AllocSpriteSlot();
    if (slot <= 0x5f) {
        UploadSpriteGFX(slot, 0x80 << 1, 0);
        spr = Func_801eadc(slot, 0x40000000, box, 0, 0);
        spr->f15 |= 0x20;
        pp = &s->o5b4;
        *pp = spr;
        bx = (struct Box *)box;
        x = bx->x * 8;
        x += 0x8c;
        x += s->f594 * 60 / s->f599;
        y = bx->y * 8 + 4;
        _Func_80b0a20(pp, x, y);
    }

    slot = AllocSpriteSlot();
    if (slot <= 0x5f) {
        UploadSpriteGFX(slot, 0x80 << 1, 0);
        spr = Func_801eadc(slot, 0x40000000, box, 0, 0);
        spr->f15 |= 0x20;
        pp = &s->o5c4;
        *pp = spr;
        bx = (struct Box *)box;
        x = bx->x * 8;
        x += 0x8c;
        x += s->f595 * 60 / s->f59a;
        y = bx->y * 8;
        y += 0x14;
        _Func_80b0a20(pp, x, y);
    }

    t = L367c9;
    y = 0x1c;
    s->tabs[0][0] = Func_8021750(t[0], 0, box, 0x54, y);
    s->tabs[0][1] = Func_8021750(t[1], 0, box, 0x6c, y);
    s->tabs[0][2] = Func_8021750(t[2], 0, box, 0x84, y);
    t = L367cc;
    y = 0x34;
    s->tabs[1][0] = Func_8021750(t[0], 0, box, 0x64, y);
    s->tabs[1][1] = Func_8021750(t[1], 0, box, 0x7c, y);
    t = L367ce;
    y = 0x4c;
    s->tabs[2][0] = Func_8021750(t[0], 0, box, 0x64, y);
    s->tabs[2][1] = Func_8021750(t[1], 0, box, 0x7c, y);
    return box;
}
