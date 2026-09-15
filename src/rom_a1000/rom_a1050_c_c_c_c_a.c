/* Func_80a15f0 -- 0x080a15f0, text half of asm/rom_a1000/rom_a1050_c_c_c_c.s
 * (the data half is rom_a1050_c_c_c_c_b.s; see below).
 *
 * Draws three before/after stat comparisons: a caption, the new value, and --
 * when the value changed -- the old value and an up/down arrow.
 *
 * TWO LEVERS AND A SYMBOL.
 *
 *   1. THE STACK ARGUMENT IS A SEPARATE LOCAL PER BLOCK. Each block passes a
 *      different y to _Func_801e9d4 through the stack slot, and the ROM
 *      materialises each straight into a LOW callee-saved register
 *      (`mov r7, #0x28`). ONE reused variable puts it in a HIGH register, and
 *      Thumb cannot `mov` an immediate into r8..r11 -- so every block costs an
 *      extra `mov r3,#N / mov r8,r3` pair and the function comes out four bytes
 *      LONG. Three separate locals keep it in r7. 130 differing -> 52.
 *   2. _MSG_b20. The first arguments to _Func_801e7c0 are 0xb1c, 0xb1d and
 *      0xb20. gcc pools the first two on its own because they are not
 *      shiftable; 0xb20 IS shiftable (0xb2 << 4), so gcc builds it with
 *      `mov r0,#178 / lsl r0,#4` where the ROM has a single `ldr r0, =0xb20`.
 *      That is the pool tell in the exact shape message.sym's "shiftable
 *      __MessageID IDs" section exists for -- and THE TWO LITERAL NEIGHBOURS IN
 *      THE SAME FUNCTION ARE THE INTERNAL CONTROL. They reproduce as literals,
 *      so this is not a blanket claim about the id space; it is one constant
 *      that gcc can build and the ROM chose not to.
 *
 * objcmp reports ONE differing encoding on the final candidate: the reference's
 * pool word is the literal 0xb20 where ours is a relocation against _MSG_b20.
 * They resolve to the same value at link, which is what `make compare` settles
 * -- and it is green.
 *
 * THE .s CARRIED DATA. tools/datacheck.py flagged `.rodata` with four exported
 * labels alongside the function, so this is a TEXT/DATA SPLIT, not a whole-file
 * conversion: the function is this file, the data is
 * asm/rom_a1000/rom_a1050_c_c_c_c_b.s, and stage1.ld's .text and .rodata lines
 * point at them in their original positions. The layout was gated green before
 * this .c landed.
 *
 * EXACT after linking: 316 bytes, 140 encodings.
 */
#include "gba/types.h"

struct Stats {
    u8 pad00[0x3c];
    u16 f3c;
    u16 f3e;
    u16 f40;
};

extern int _MSG_b20;
extern void _Func_801e7c0(int id, void *w, int x, int y);
extern void _Func_801e9d4(int v, int a, void *w, int x, int y);
extern void Func_80ae99c(void *w, int x, int y, int d);
void Func_80a15f0(struct Stats *a, struct Stats *b, void *w)
{
    int y1;
    int y2;
    int y3;

    _Func_801e7c0(0xb1c, w, 0, 0x20);
    y1 = 0x28;
    _Func_801e9d4(b->f3c, 3, w, 0x10, y1);
    if (a->f3c != b->f3c) {
        _Func_801e9d4(a->f3c, 3, w, 0x40, y1);
        if (a->f3c > b->f3c)
            Func_80ae99c(w, 0x2c, 0x24, 0);
        else
            Func_80ae99c(w, 0x2c, 0x24, 1);
    }
    _Func_801e7c0(0xb1d, w, 0, 0x30);
    y2 = 0x38;
    _Func_801e9d4(b->f3e, 3, w, 0x10, y2);
    if (a->f3e != b->f3e) {
        _Func_801e9d4(a->f3e, 3, w, 0x40, y2);
        if (a->f3e > b->f3e)
            Func_80ae99c(w, 0x2c, 0x34, 0);
        else
            Func_80ae99c(w, 0x2c, 0x34, 1);
    }
    _Func_801e7c0((int)&_MSG_b20, w, 0, 0x40);
    y3 = 0x48;
    _Func_801e9d4(b->f40, 3, w, 0x10, y3);
    if (a->f40 != b->f40) {
        _Func_801e9d4(a->f40, 3, w, 0x40, y3);
        if (a->f40 > b->f40)
            Func_80ae99c(w, 0x2c, 0x44, 0);
        else
            Func_80ae99c(w, 0x2c, 0x44, 1);
    }
}
