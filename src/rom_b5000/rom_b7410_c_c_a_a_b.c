/* Cluster Func_80b7f9c..Func_80b7f9c extracted from goldensun/asm/rom_b5000/rom_b7410_c_c_a_a.s.
 *
 * Total .text for this TU = 100 bytes.
 * Preserves the original ROM layout when slotted before
 * asm/rom_b5000/rom_b7410_c_c_a_a_c.o in goldensun/stage1.ld.
 *
 * Camera reset: zero two vectors, set pitch and yaw, then build the matrix and
 * transform one point.
 *
 * TWO THINGS CARRY THE MATCH:
 *   - `bl _call_via_r3` is ordinary C -- a typed function-pointer local,
 *     assigned then called, per the template in src/rom_c0/rom_49a8_b.c.
 *   - THE ZERO IS NAMED. `z` earns a callee-saved register because its live
 *     range crosses four calls; that is the documented exception to "do not
 *     name zeros", and the ROM's prologue asks for the register.
 * The store order is written exactly as the ROM emits it (0x36, 0x34, 0x20,
 * then 0xc/0x10/0x14, then 0x1c/0x18), not in declaration order.
 */
#include "gba/types.h"

struct View {
    vec3_t v0;
    vec3_t v1;
    vec3_t v2;
    unsigned char pad24[0x10];
    short pitch;
    short yaw;
};

extern void *iwram_3001e80;
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int angle);
extern void MatrixPitch(int angle);
void Func_80009c0(vec3_t *a, vec3_t *b);

void Func_80b7f9c(void)
{
    struct View *s;
    vec3_t t;
    int z;
    void (*transform)(vec3_t *, vec3_t *);

    s = (struct View *)iwram_3001e80;
    s->yaw = 0xc0 << 6;
    s->pitch = 0xfe << 8;
    z = 0;
    s->v2.z = 0xff << 17;
    s->v1.x = z;
    s->v1.y = z;
    s->v1.z = z;
    s->v2.y = z;
    s->v2.x = z;
    InitMatrixStack();
    MatrixTranslatev(&s->v1);
    MatrixYaw(s->yaw);
    MatrixPitch(s->pitch);
    t.x = z;
    t.y = z;
    t.z = s->v2.z;
    transform = Func_80009c0;
    transform(&t, &s->v0);
}
