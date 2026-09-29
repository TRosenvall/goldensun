/* Func_8018efc -- NON-MATCHING, 17 of 119 encodings differ.
 * Reference asm//rom_15000/rom_18cac_a_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/8018efc.c \
 *       asm//rom_15000/rom_18cac_a_c.s --func Func_8018efc
 *
 * IMPROVED IN BATCH 298 FROM 86 TO 17, and it is now a TRUE DISTANCE -- count 119 ==
 * 119, where the previous revision was 123 against 119 at 268 bytes against 260.
 * PIN-FREE.
 *
 * The previous revision was parked after a 450-SPELLING SWEEP, and every spelling in
 * it left the constant INSIDE the shift.  The fix came from DrawText in the same
 * batch and is now documented: DISTRIBUTE THE SHIFT BY HAND.  Dropping the two
 * distributed expressions in changed nothing else.
 * The remaining 17 are one r6/r7 role swap.
 */
/* *** BATCH-298f UPDATE, DROP THIS IN OVER src/non_matching/rom_15000/8018efc.c ***
 * The ONLY change from the tracked park is the two sprite-position expressions,
 * with the shift DISTRIBUTED BY HAND:
 *     s->x = (win->x << 3) + ((unsigned short)(win->w - 2) << 3) + 4;
 *     s->y = (win->y << 3) + ((unsigned char)(win->h - 2) << 3) - 1;
 * (the `cx` / `cy` locals are now unused and can be deleted).
 *
 *   before:  86 encodings of 119 differ, ours 123, 268 bytes against 260
 *            -- counts AND size disagree, so 86 was never a distance
 *   after:   17 encodings of 119 differ, ours 119, SIZE AND COUNT BOTH MATCH
 *            -- 17 IS a distance
 *
 * The park's own blocker section is what this retires: "the whole residue is
 * FOUR EXTRA INSTRUCTIONS, the zero-extensions of `cx` and `cy`", after a
 * 450-spelling sweep of casts and `<< 3` vs `* 8`.  Every one of those spellings
 * left the CONSTANT INSIDE the shift, so fold's `associate:` had something to
 * move; writing the distribution out leaves it nothing, and combine folds the
 * two `<< 3`s back into the ROM's one.  Validated independently on the file-mate
 * DrawText, where the same lever took 83.4% -> 99.2% aligned-equal and left one
 * pool word.
 *
 * THE REMAINING 17 ARE ONE REGISTER-ROLE SWAP plus two small things:
 *   - `win` in r7 and the base/queue pointer in r6 in the ROM; the other way
 *     round here.  13 of the 17 are that swap.
 *   - one adjacent schedule swap, `ldrh r3, [r7, #8]` against `ldr r1, =0xfffe`.
 *   - `strh r1, [r5, r2]` against ours `strh r1, [r2, r5]` -- the base/index
 *     order on the indexed halfword store.
 * Three probes were INERT (all 17): swapping the `base` / `p` declaration order,
 * `*(unsigned short *)(p + pos * 2)`, and both together.
 */
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
        s->x = (win->x << 3) + ((unsigned short)(win->w - 2) << 3) + 4;
        s->y = (win->y << 3) + ((unsigned char)(win->h - 2) << 3) - 1;
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
