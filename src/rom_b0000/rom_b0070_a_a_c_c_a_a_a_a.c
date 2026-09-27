/* Func_80b08b8  --  0x080b08b8, was asm/rom_b0000/rom_b0070_a_a_c_c_a_a_a_a.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * - The ROM reloads m->x0 with `ldrh` after the division call instead of
 *   reusing the earlier `ldrsh`: `(unsigned short)m->x0 + ...` narrows the
 *   operand so the load lands after the call.
 * - `(f16 & 0xfffffe00) | (x & 0x1ff)` with both constants pooled in ROM order
 *   is a 9-bit BITFIELD store, `o->f16x = x`; the hand-masked spelling
 *   misorders the schedule.
 * - `int step = (signed char)++m->step` gets both the load order and the mul
 *   operand order.
 */
struct Obj {
    unsigned char pad0[6];
    short x;
    short y;
    unsigned char pad1[0x14 - 0xa];
    unsigned char b14;
    unsigned char pad2;
    unsigned short f16x:9;
    unsigned short f16r:7;
};

struct Mover {
    struct Obj *obj;
    short x0;
    short y0;
    short x1;
    short y1;
    unsigned char step;
    signed char total;
};

void Func_80b08b8(struct Mover *m)
{
    struct Obj *o;
    int step;
    int total;
    int x;
    int y;

    if (m == 0)
        return;
    total = m->total;
    if (total == 0)
        return;
    o = m->obj;
    step = (signed char)++m->step;
    x = (unsigned short)m->x0 + (m->x1 - m->x0) * step / total;
    o->x = x;
    o->f16x = x;
    y = (unsigned short)m->y0 + (m->y1 - m->y0) * step / total;
    o->y = y;
    o->b14 = y;
    if (step == total) {
        m->total = 0;
        m->step = 0;
    }
}
