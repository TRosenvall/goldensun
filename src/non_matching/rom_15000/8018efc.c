/* Func_8018efc -- 0x08018efc, asm/rom_15000/rom_18cac_a.s (2 functions: DrawText first, so
 * NON-MATCHING: 86 encodings of 119 differ (objcmp; ours 123, four zero-extensions long).
 * landing needs a split).
 *
 * NOT MATCHING: 86 differing of 119 encodings (ours 123) -- but that count is a SIZE ARTEFACT.
 * The first 45 lines match the ROM exactly (tryc: first diff at 46); the whole residue is
 * FOUR EXTRA INSTRUCTIONS, the zero-extensions of `cx` and `cy` (lsl #16/lsr #16 and
 * lsl #24/lsr #24), and everything after them only mismatches because it is shifted by 8 bytes.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8018efc.c asm/rom_15000/rom_18cac_a.s --func Func_8018efc
 *   python3 tools/tryc.py src/non_matching/rom_15000/8018efc.c --ref asm/rom_15000/rom_18cac_a.s --full
 *
 * THE BLOCKER: the ROM computes the sprite x as
 *     ldrh w / ldr r1,=0xfffe / add r3,r1 / add r2,r3 (x) / lsl #3 / add #4 / and 0x1ff
 * i.e. `(u16)(w - 2)` as its OWN insn (0xfffe = -2 in HImode, so the arithmetic is unsigned
 * short), then `+ win->x`, with NO zero-extension -- the chain is HImode throughout and the
 * 9-bit bitfield store makes the extension unnecessary. The y byte is the same shape at u8
 * (`ldrb h / add #0xfe / add y`).
 *   - As a u16 local (below) the -2 stays separate but the local is PROMOTED to SImode, so its
 *     assignment zero-extends (store_expr on a SUBREG_PROMOTED target) and combine cannot drop
 *     the extension: the use is `(plus (zero_extend ..) x)`, four insns from the `& 0x1ff`.
 *   - Inline, the chain IS HImode (narrowed by convert_to_integer for the bitfield store) but
 *     fold's `associate:` (fold-const.c, split_tree) reassociates `(w + 0xfffe) + x` into
 *     `(w + x) + 0xfffe`, which combine then folds with the <<3/+4 into one pooled 0x7fff4.
 *     Measured: casts (u16)/(short)/(int)/(unsigned) on each of w-2, win->x and the whole
 *     expression, `<< 3` vs `* 8` (a 450-spelling sweep) -- ALL associate. An inline helper
 *     returning u16 extends like the local. Split_tree only refuses a conversion that changes
 *     signedness, but convert_to_integer picks an unsigned narrowing type whenever any operand
 *     is unsigned, so the two levels always end up the same type.
 *
 * WHAT LANDED ON THE WAY (from 103 differing lines at the first tryc screen):
 *   - `(B7 *)n - (B7 *)(base + 0x698)` with `typedef unsigned char B7[7]`: the ROM's slot index
 *     is a bare `mul` by 0xb6db6db7 (exact division by 7, no shift). A struct of 7 bytes pads
 *     to 8 on ARM (asr #3); `(n - pool) << 2` gives mul/asr/lsl. Only a 7-byte ARRAY type works.
 *   - ONE POINTER FOR THE BASE AND THE NODE: the ROM loads iwram into r5, copies it to r6, and
 *     later reuses r5 for the node. `p = iwram; base = p; ... p = Func_8015e8c();` with the node
 *     accessed through p reproduces the two registers; a separate `n` gets one base register.
 *   - `y++; x++;` before `pos = ((win->y + y) << 5) + (win->x + x)`: the ROM increments the
 *     parameters in place (add r4,#1 / add r0,#1); `+ 1` inside the expression is folded.
 *   - `if (n->f5 == 0) n->f5 = mode` reloads the byte, as the ROM does.
 *   - `ldrh r2, .L` for the pooled 0x1ff and 0xfffe assembles to the same `ldr` encoding.
 */
struct Spr {
    unsigned int a;
    unsigned char y;
    unsigned char b;
    unsigned short x : 9;
    unsigned short rest : 7;
};

struct Node {
    unsigned int next;
    unsigned char f4;
    unsigned char f5;
    unsigned short x;
    unsigned short y;
    unsigned char pad[4];
    unsigned char slot;
    unsigned char fF;
    struct Spr spr;
    unsigned int pad2;
};

typedef unsigned char B7[7];

struct Win {
    unsigned char pad[8];
    unsigned short w;
    unsigned short h;
    unsigned short x;
    unsigned short y;
};

extern unsigned char *iwram_3001e8c;
extern struct Node *Func_8015e8c(void);
extern int AllocSpriteSlot(void);
extern int Func_8016584(struct Win *, struct Node *);

#define n ((struct Node *)p)
void Func_8018efc(struct Win *win, unsigned int ch, unsigned int x, unsigned int y, int mode)
{
    unsigned char *base;
    unsigned char *p;
    struct Spr *s;
    unsigned short *q;
    int slot;
    unsigned int pos;
    unsigned short cx;
    unsigned char cy;

    p = iwram_3001e8c;
    base = p;
    if (y > win->h - 2)
        return;
    if (x > win->w - 2)
        return;
    if (mode == 1) {
        p = (unsigned char *)Func_8015e8c();
        if (p == 0)
            return;
        slot = (B7 *)n - (B7 *)(base + 0x698);
        n->f5 = 2;
        q = (unsigned short *)(base + 0x12b6);
        s = &n->spr;
        if (*q == 0x63)
            *q = AllocSpriteSlot();
        cx = win->w - 2;
        s->x = ((cx + win->x) << 3) + 4;
        cy = win->h - 2;
        s->y = ((cy + win->y) << 3) - 1;
        n->x = s->x;
        n->y = s->y;
        n->next = 0;
        n->slot = slot;
        if (n->f5 == 0)
            n->f5 = mode;
        Func_8016584(win, n);
    } else {
        if (ch > 0xff)
            return;
        y++;
        x++;
        pos = ((win->y + y) << 5) + (win->x + x);
        if (pos >= 0x280)
            return;
        ((unsigned short *)p)[pos] = ch | 0xf000;
    }
}
