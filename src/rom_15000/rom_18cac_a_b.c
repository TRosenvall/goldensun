/* DrawText (0x08018cac, asm/rom_15000/rom_18cac_a.s).
 *
 * ONE differing encoding of 265, and it is a RELOCATION FORM, not a residue:
 *     XX ENCODINGS differ in 1 place(s) (ref 265, ours 265)
 *        first at index 183: ref 00000318  ours 00000000
 * SIZE AND INSTRUCTION COUNT BOTH MATCH (592 bytes, 265 instructions), and
 * tools/aligncmp.py reports 263 of 265 aligned-equal.  That one word is the pool
 * entry for `_SIZE_80155d0`; ours carries an R_ARM_ABS32 against it where the
 * reference carries the linked literal 0x318, and EVERY OTHER RELOCATION IS
 * IDENTICAL, same offsets and all.  Once the symbol is in size.sym the linked
 * bytes are the reference's.
 *
 * THIS NEEDS THREE THINGS TO LAND, none of them source:
 *   1. size.sym:      _SIZE_80155d0 = 0x318;
 *   2. a TEXT SPLIT of asm/rom_15000/rom_18cac_a.s -- TWO functions (DrawText,
 *      Func_8018efc) and NO data section; tools/datacheck.py reports no
 *      requirement at all, so the split exports nothing.
 *   3. `make compare`.
 * Shim count 0 (tools/shimcount.py): PIN-FREE, no fakematch.txt row.  The one
 * DMA3_COPY picks up include/dma.h's own register pins, which are the shared
 * header's and are not booked per file.
 *
 * ================================================================
 * _SIZE_80155d0 = 0x318 -- A NEW size.sym ENTRY, ARITHMETIC VERIFIED
 * ================================================================
 *
 * size.sym exists for "sizes of routines the game copies into RAM and runs
 * there", and this function is a textbook member: it allocates a scratch buffer
 * with Func_8004938, DMA3s the bytes of the ARM routine Func_80155d0 into it,
 * calls it there through `bl _call_via_r6`, and frees it.
 *
 * THE VALUE IS THE GAP TO THE NEXT SYMBOL, in the same file and by the same
 * method as the four entries already in size.sym:
 *     asm/rom_15000/rom_15430.s:123  .arm_func_start Func_80155d0  @ 0x080155d0
 *     asm/rom_15000/rom_15430.s:345  .arm_func_start Func_80158e8  @ 0x080158e8
 *     0x080158e8 - 0x080155d0 = 0x318
 * That is the SAME FILE as _SIZE_8015afc / _SIZE_8015d74 / _SIZE_8015e10 /
 * _SIZE_8015430, so this is a fifth routine out of rom_15430.s.
 *
 * CRITERION 1 IS MET IN ITS STRONG FORM.  0x318 is 0xc6 << 2, so
 * thumb_shiftable_const makes gcc BUILD it -- `mov r0, #0xc6 / lsl r0, #2` --
 * and the ROM POOLS it (`ldr r5, =0x318`).  A pool word holding a value gcc
 * would never pool cannot have come from a const_int.
 *
 * CRITERION 2 IS MET BY THE DMA WORD, exactly as _SIZE_8015430's entry predicts:
 * with a literal size gcc folds `0x84000000 | (len / 4)` to one pooled
 * 0x840000c6 and the length register disappears, so the whole block is THREE
 * instructions short.  With the symbol, `len` is opaque and gcc emits the ROM's
 * runtime form, instruction for instruction:
 *     ldr r5, =_SIZE_80155d0 / mov r0, r5 / bl Func_8004938 / mov r2, #0x84 /
 *     mov r6, r0 / lsr r5, #2 / lsl r2, #24 / ldr r3, =REG_DMA3SAD /
 *     ldr r0, =Func_80155d0 / mov r1, r6 / orr r2, r5 / stmia r3!, {r0,r1,r2}
 * -- the same register shared between the malloc argument and the DMA control
 * word that _SIZE_8015430's entry names as the tell.  Measured: the literal
 * spelling is 262 instructions against the ROM's 265, the symbol 265.
 *
 * ================================================================
 * THE LEVER THAT CLOSED IT, AND IT ALSO CLOSES MOST OF THE FILE-MATE'S PARK
 * ================================================================
 *
 * *** DISTRIBUTE THE SHIFT BY HAND WHEN THE ROM DOES NOT FOLD IT. ***
 *
 * The sprite position is `((win->x + (u16)(win->w - 2)) << 3) + 4`.  The ROM
 * emits the 16-bit wrap as its OWN instruction -- `ldr r1, =0xfffe / add r3, r1`
 * -- then `+ win->x`, then `lsl #3`, then `add #4`.  Written that way,
 * fold-const's `associate:` distributes the shift and merges the two constants
 * into one pooled `0x0007fff4` (= 0xfffe << 3 | 4), and the `0xfffe` pool word
 * and the `adds r2, #4` both disappear.  Written with a `u16` local instead, the
 * local is SUBREG_PROMOTED and its assignment zero-extends (`lsl #16 / lsr #16`)
 * -- four extra instructions.
 *
 * WRITING THE DISTRIBUTION OUT IN THE SOURCE gets both:
 *     s->f.x = (win->x << 3) + ((unsigned short)(win->w - 2) << 3) + 4;
 *     s->f.y = (win->y << 3) + ((unsigned char)(win->h - 2) << 3) - 1;
 * gcc then has nothing left to associate -- the constant is already outside the
 * shift -- and jump2/combine fold the two `<< 3`s back into the ROM's single
 * one.  99.2% aligned-equal against 93.6% for the best inline spelling.
 *
 * MEASURED PROGRESSION (aligncmp, of the ROM's 265):
 *     u16 local, 4 zero-extensions                  83.4%   269 insns
 *     inline with any cast (4 spellings, identical) 90.2%   266
 *     inline + `ret = 5` moved inside its branch    93.6%   266
 *     DISTRIBUTED BY HAND                           99.2%   265
 * The four casts probed on the inline form -- (short)/(u16)/(int)(u16)/
 * (unsigned)(u16), on either operand -- are all EXACTLY equal, which confirms
 * src/rom_15000/rom_18cac_a_c.c (LANDED; was src/non_matching/rom_15000/8018efc.c)'s conclusion that no cast reaches the
 * reassociation; what it did not try is distributing the shift.
 *
 * *** THE SAME LEVER IS WORTH 86 -> 17 ON THE FILE-MATE. ***
 * src/rom_15000/rom_18cac_a_c.c (LANDED; was src/non_matching/rom_15000/8018efc.c) is parked on precisely this blocker
 * ("the whole residue is FOUR EXTRA INSTRUCTIONS, the zero-extensions of `cx`
 * and `cy`", after a 450-spelling sweep).  Substituting the two distributed
 * expressions into that park, with nothing else changed, takes it from
 * "86 of 119, ours 123, 268 bytes against 260" -- counts disagreeing, so not a
 * distance -- to "17 of 119, ours 119", size and count BOTH matching.  Its
 * remaining 17 are a single r6/r7 role swap (`win` in r7 and the base pointer in
 * r6 in the ROM, the other way round in ours) plus one adjacent schedule swap
 * and the `strh r1, [r5, r2]` / `[r2, r5]` base-index order.  Three probes there
 * were inert (swapping the base/p declaration order, and two spellings of the
 * indexed halfword store).  THAT PARK SHOULD BE REWRITTEN ON THIS BASIS.
 *
 * ================================================================
 * EVERYTHING ELSE THAT WAS NEEDED, all read off the reference
 * ================================================================
 *
 *  - THE STRUCTS ARE THE FILE-MATE'S, with three refinements this function
 *    forces: a `u16` at node+0x0c (the park had `unsigned char pad[4]`), a `u16`
 *    at node+0x18, and `struct Spr` needing BOTH a byte view at +4 (`strb
 *    r3,[r7,#4]` in the sprite path) and a halfword view (`strh r3,[r5,#0x14]`
 *    in the text path).  A union of the field struct with `unsigned short w[4]`
 *    covers it.  `struct Win` needs a `u16` at 0x16 for the `& 8` flag test.
 *  - `typedef unsigned char B7[7]` and
 *    `slot = (B7 *)n - (B7 *)(base + 0x698)` -- the file-mate's exact-division
 *    idiom, which is where the ROM's bare `mul` by 0xb6db6db7 (the modular
 *    inverse of 7, with NO shift) comes from.  A 7-byte struct pads to 8 and
 *    gives mul/asr; only a 7-byte ARRAY type works.
 *  - THE WINDOW-OWNER TEST IS A DOUBLE INDIRECTION THROUGH A NEIGHBOURING
 *    SYMBOL: `*((struct Win ***)&iwram_3001e8c)[0x16] == win` reproduces
 *    `ldr r3, [r1, #0x58] / ldr r3, [r3] / cmp r3, r8`.  One `ldr` and it reads
 *    as a single pointer; two, and it does not.
 *  - ONE EXIT THROUGH `ret`.  The three early `ch == 0x20` returns are
 *    `goto done;`, so all four ret-bearing exits funnel through the ROM's
 *    `.L18ecc: mov r0, r9`, while `return 0` (no node) and `return r` (the RAM
 *    routine's result) jump straight to the epilogue, which is the ROM's
 *    two-label tail.
 *  - `ret = 5` MUST BE ASSIGNED INSIDE ITS OWN BRANCH.  Assigned before the
 *    `ch == 0x20` test it stays in r9 and post-reload CSE then writes
 *    `mov r2, r9` where the ROM re-materialises `movs r3, #5` for the
 *    `n->f5 = 5` switch arm.  Worth 90.2% -> 93.6% on its own.
 *  - A TEMP FOR THE GLYPH RESULT: `t = DrawMsgGlyph(...); if (t == 0) t = 1;
 *    ret = t;`.  Assigning `ret` directly writes the high register twice where
 *    the ROM writes it once after the join.
 *  - THE SWITCH ON `*(u16 *)(base + 0xeac)` IS A REAL `switch` and gcc's
 *    balanced tree (`cmp #3/beq, cmp #3/bgt, cmp #2/beq, b` then `cmp #4/beq,
 *    cmp #5/beq, b`) is the ROM's, including the shared `strb/strh` tail that
 *    cases 5 and 2 cross-jump into.
 *  - `len` and the DMA control word share one register; see the size.sym section.
 *
 * ================================================================
 * THE REFERENCE'S PROSE IS BROADLY RIGHT, with one correction
 * ================================================================
 *
 * `@ QueueGlyphs / r0 = window record, r1.. = the text run. ... fetches the font
 *  data through GetFile and Func_4938, and DMA3s the rasterised glyphs into
 *  place. ... Traced structurally.`
 *
 * Func_8004938 is NOT a font fetch: it is an ALLOCATOR.  What the ROM does with
 * it is allocate 0x318 bytes, DMA the ARM routine Func_80155d0 into the buffer,
 * CALL THE BUFFER, and free it -- a RAM trampoline, the same shape as the four
 * routines size.sym already covers.  `r1` is a single character, not a run (it
 * is compared against 0x20 and passed to DrawMsgGlyph); the parameters are
 * (window, char, x, y, mode), the same five as the file-mate Func_8018efc.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "file_table.h"

union Spr {
    struct {
        unsigned int a;
        unsigned char y;
        unsigned char b;
        unsigned short x : 9;
        unsigned short rest : 7;
    } f;
    unsigned short w[4];
};

struct Node {
    unsigned int next;      /* 0x00 */
    unsigned char f4;       /* 0x04 */
    unsigned char f5;       /* 0x05 */
    unsigned short x;       /* 0x06 */
    unsigned short y;       /* 0x08 */
    unsigned short fa;      /* 0x0a */
    unsigned short fc;      /* 0x0c */
    unsigned char slot;     /* 0x0e */
    unsigned char fF;       /* 0x0f */
    union Spr spr;          /* 0x10 */
    unsigned short f18;     /* 0x18 */
    unsigned short f1a;
};

typedef unsigned char B7[7];

struct Win {
    unsigned char pad[8];
    unsigned short w;       /* 0x08 */
    unsigned short h;       /* 0x0a */
    unsigned short x;       /* 0x0c */
    unsigned short y;       /* 0x0e */
    unsigned short pad2[3];
    unsigned short f16;     /* 0x16 */
};

typedef int (*TextFn)(struct Win *win, unsigned int ch, unsigned int x, unsigned int y, void *font);

extern unsigned char *iwram_3001e8c;
extern struct Node *Func_8015e8c(void);
extern int AllocSpriteSlot(void);
extern int Func_8016584(struct Win *win, struct Node *n);
extern void *GetFile(int id);
extern void *Func_8004938(int size);
extern void free(void *p);
extern int DrawMsgGlyph(unsigned int ch, void *buf);
extern void Func_80155d0(void);
extern unsigned char _SIZE_80155d0[];

int DrawText(struct Win *win, unsigned int ch, unsigned int x, unsigned int y, int mode)
{
    unsigned char *base;
    struct Node *n;
    union Spr *s;
    unsigned short *q;
    unsigned short *q2;
    void *buf;
    void *font;
    int len;
    int ret;
    int slot;
    int r;
    int t;
    unsigned int A;
    unsigned int B;
    unsigned short cx;
    unsigned char cy;
    unsigned char glyph[0x80];

    base = iwram_3001e8c;
    A = *(unsigned short *)(base + 0x12b0);
    B = *(unsigned short *)(base + 0xea8);
    if (mode != 1 && (win->f16 & 8) != 0) {
        if (*((struct Win ***)&iwram_3001e8c)[0x16] == win) {
            GetFile(FILE_14);
            GetFile(FILE_13);
            ret = 3;
            if (ch == 0x20)
                goto done;
        }
        font = GetFile(FILE_13);
        ret = 4;
        if (ch == 0x20)
            goto done;
        len = (int)&_SIZE_80155d0;
        buf = Func_8004938(len);
        DMA3_COPY((void *)Func_80155d0, buf, len);
        r = ((TextFn)buf)(win, ch, x, y, font);
        free(buf);
        return r;
    }
    if (ch == 0x20) {
        ret = 5;
        goto done;
    }
    n = Func_8015e8c();
    if (n == 0)
        return 0;
    slot = (B7 *)n - (B7 *)(base + 0x698);
    n->f5 = 1;
    n->f4 = 0;
    if (mode == 1) {
        ret = 1;
        n->f5 = 2;
    } else {
        switch (*(unsigned short *)(base + 0xeac)) {
        case 3:
            n->f5 = 5;
            break;
        case 4:
            n->f5 = 6;
            n->fc = 8;
            break;
        case 5:
            n->f5 = 7;
            n->fc = 0;
            break;
        case 2:
            n->f5 = 4;
            n->fc = 0;
            break;
        }
        t = DrawMsgGlyph(ch, glyph);
        if (t == 0)
            t = 1;
        ret = t;
    }
    if (n->f5 == 2) {
        q = (unsigned short *)(base + 0x12b6);
        s = &n->spr;
        if (*q == 0x63)
            *q = AllocSpriteSlot();
        s->f.x = (win->x << 3) + ((unsigned short)(win->w - 2) << 3) + 4;
        s->f.y = (win->y << 3) + ((unsigned char)(win->h - 2) << 3) - 1;
    } else {
        q2 = (unsigned short *)(base + 0x12b8);
        DMA3_COPY(glyph, (void *)(0x6010000 + ((*q2 + slot) << 5)), 0x80);
        n->spr.w[2] = (y + (B >> 1)) + (win->y << 3) - 2;
        n->spr.w[3] = ((x + (A >> 1)) + (win->x << 3) + 2) | 0x4000;
        n->f18 = *q2 + slot;
        s = &n->spr;
    }
    n->fF = 0xfe;
    n->x = s->f.x;
    n->y = s->f.y;
    n->slot = slot;
    n->next = 0;
    Func_8016584(win, n);
done:
    return ret;
}
