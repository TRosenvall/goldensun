/* Func_80164d4 -- 0x080164d4, asm/rom_15000/rom_15e8c_a_c_c_a_a_c_a_c.s (1 function,
 * no data section, so landing needs no split).
 *
 * NOT MATCHING: 148 bytes against 152, 70 encodings against 72 -- TWO INSTRUCTIONS
 * SHORT. Candidate below.
 *
 * WHAT IS ALREADY EXACT: all the arithmetic, both loops, the `h` spill to the 4-byte
 * slot, the base in r8, the pool ORDER (0xF020, iwram, 0xEA3), the mid-function pool
 * dump and its `b`, and the epilogue.
 *
 * THE RESIDUE IS TWO REGISTER-ALLOCATION FACTS:
 *   - the ROM loads `bottom` into r1 and the pool address into r2, so it needs
 *     `mov r7, r2 / mov r5, r1` first; ours takes the free r4/r6 and needs neither.
 *   - the ROM's `p` is a FRESH pseudo, giving `mov r2, r8 / add r0, r3, r2`, where ours
 *     accumulates into the existing one and emits `add r5, r5, r8`.
 *
 * SWEPT, AND THE FAMILY FLOORS AT 65 OF 68 INSTRUCTIONS in every variant -- roughly
 * 5,500 candidates: all 1,750 topological statement orders, 3,430 variable-reuse
 * variants, six `p` spellings, three base positions, eight callee prototype forms, four
 * declaration orders, five base types, five loop spellings, ox/oy naming, and
 * fold-blocking casts. No pins tried.
 *
 * THE TENSION THAT MAKES IT FLOOR, which is the useful part: ordering `w, h` BEFORE
 * `x, y` removes an r10 use, but it also lets combine fold `(r0+ox)-(l0+ox)` to
 * `r0-l0` and drop an instruction. The two effects pull in opposite directions, so no
 * statement order can have both -- which is why a pure order sweep cannot close this
 * and why the next attempt should change the EXPRESSIONS so the fold is unavailable,
 * not the order.
 */
struct Win {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

extern unsigned char *iwram_3001e8c;
extern void Func_801e260(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

void Func_80164d4(struct Win *win, unsigned int left, unsigned int top,
                  unsigned int right, unsigned int bottom)
{
    unsigned char *base;
    unsigned short *p;
    unsigned int i, j;
    unsigned int x, y, w, h;
    unsigned int l, t, r, b;

    base = iwram_3001e8c;
    l = (left >> 3) + win->fc;
    t = (top >> 3) + win->fe;
    r = ((right + 7) >> 3) + win->fc;
    b = ((bottom + 7) >> 3) + win->fe;
    y = t + 1;
    h = b - t;
    x = l + 1;
    w = r - l;
    Func_801e260(x, y, w, h);
    p = (unsigned short *)(((y << 5) + x) * 2 + (unsigned int)base);
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            *p = 0xf020;
            p++;
        }
        p += 0x20 - w;
    }
    base[0xea3] = 1;
}
