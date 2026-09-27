/* Func_8017aa4  --  0x08017aa4, was asm/rom_15000/rom_178b0_c_a.s (this function
 * alone; its jump table is inline in .text), so it converts whole. This
 * completes rom_178b0_c.s: all four of its functions are now C.
 *
 * THE RELOAD REGISTER FOR A POOLED CONSTANT IS CHOSEN ROUND-ROBIN
 * (allocate_reload_reg, reload1.c:5003, via last_spill_reg), skipping registers
 * that hold live pseudos -- not find_reg's choice (.greg says reg 3, the output
 * uses r2). `ldr r2,=0x12b2` became the ROM's r3 only after `s = base + 0xeb0`
 * moved above `q = base + 0x12b2`, so a live pseudo occupies the unwanted
 * register at that insn. Per-case stores plus an `int v` temporary reproduce
 * the ROM's cross-jumped `ldr / ldrh / add / strh` tail.
 */
struct Box {
    unsigned char pad00[0x16];
    unsigned short flags;
};

extern unsigned char *iwram_3001e8c;
struct Glyph { unsigned short w; unsigned char pad[30]; };
extern struct Glyph Data_32224[];
extern void Func_80173ac(void);
extern int DrawText(struct Box *box, int ch, int x, int y, int e);

void Func_8017aa4(unsigned short *s, struct Box *box, int x, int y)
{
    unsigned char *base = iwram_3001e8c;
    short x0;
    unsigned short *q;
    unsigned int ch;
    unsigned int c2;
    int off;
    unsigned short w;
    int v;

    ch = 0;
    x0 = x;
    if (s == 0) {
        s = (unsigned short *)(base + 0xeb0);
        q = (unsigned short *)(base + 0x12b2);
        off = (*q << 1) + 0xeb0;
        *(unsigned short *)(base + off) = ch;
        *q = (*q + 1) & 0x1ff;
    }
    for (;;) {
        ch = *s++;
        if (ch > 0xff)
            ch = 0x40;
        if (ch == 0)
            break;
        if (ch <= 0x1e) {
            switch (ch) {
            case 8:
                v = *s;
                *(unsigned short *)(base + 0xeae) = v;
                goto skip;
            case 9:
                v = *s;
                *(unsigned short *)(base + 0xeac) = v;
                goto skip;
            case 10:
                v = *s;
                *(unsigned short *)(base + 0xea8) = v;
            case 11: case 12: case 17: case 29:
            skip:
                s++;
                break;
            case 7:
                Func_80173ac();
                break;
            case 3:
                x = x0;
                y += 0xf;
                break;
            case 14: case 15: case 28:
                s++;
                goto skip;
            }
        } else {
            if ((box->flags & 8) == 0) {
                c2 = *s;
                if (ch > 0x20 && c2 > 0x20) {
                    w = Data_32224[ch - 0x20].w + Data_32224[c2 - 0x20].w;
                    if (w <= 0xf) {
                        ch |= c2 << 8;
                        s++;
                    }
                }
            }
            x += DrawText(box, ch, x, y, 0);
        }
    }
}
