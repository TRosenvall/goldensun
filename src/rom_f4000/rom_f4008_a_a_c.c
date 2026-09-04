/* Func_80f4028 -- 0x080f4028  (SetUpProjection)
 *
 * Builds the minigame's 3D view in the camera block at iwram_3001e80: set the
 * pitch and the view distance, zero both position vectors and the yaw, walk the
 * matrix stack (identity, translate, yaw, pitch), transform one point, then
 * install the projection with the focal length divided in 16.16.
 *
 * A NEAR-TWIN of src/rom_b5000/rom_b7410_c_c_a_a_b.c (Func_80b7f9c), which is
 * where `struct View` comes from -- same camera block, same field offsets, and
 * the same two carrying idioms: `bl _call_via_r3` is a typed function-pointer
 * local assigned then called, and THE ZERO IS NAMED because its live range
 * crosses four calls and the ROM's prologue asks for the register. The store
 * order is the ROM's emission order, not declaration order.
 *
 * WHAT THIS ONE ADDS, and it is the whole residue -- the last twelve lines:
 *
 *   THE FOCAL LENGTH IS PASSED TWICE AND MUST BE REBUILT BOTH TIMES. `0xfa <<
 *   16` goes to the divide and again to Func_8005258. Written as a literal at
 *   both sites gcc commons it into a callee-saved register (`mov r5, #0xfa /
 *   lsl r5, #0x10`) and feeds both calls with `mov r0, r5`; the ROM emits
 *   `mov r0, #0xfa / lsl r0, #0x10` at each. Assigning a local pinned to r0
 *   separately per site rematerialises both. This is the reuse lever, not a
 *   register lever -- batch 214 established that it fires even when the two
 *   uses land in different argument registers.
 *
 *   THE DIVIDE'S ARGUMENT FILL IS CROSSED, with the callee's pool load wedged
 *   inside it: the ROM emits the movs r0, r1 but the shifts r1, r0. Pinning
 *   both operands and writing the movs and shifts as separate statements in the
 *   ROM's own order gets the SHIFTS right and leaves the two movs transposed;
 *   ONE barrier after the first mov settles it. Batch 215's rule -- pins first,
 *   barrier only if the pairs re-fuse -- with the barrier still needed, which is
 *   the case that rule predicts: a value that is merely LOADED (the function
 *   pointer) sits between the halves.
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
extern int gPhysVec[];
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int angle);
extern void MatrixPitch(int angle);
void Func_80009c0(vec3_t *a, vec3_t *b);
fx32 Func_80008ac(fx32 num, fx32 denom);
void Func_8005258(fx32 focal, fx32 zMin, fx32 zMax);

void Func_80f4028(void)
{
    struct View *s;
    vec3_t t;
    int z;
    void (*transform)(vec3_t *, vec3_t *);
    fx32 (*divide)(fx32, fx32);
    fx32 r;
    register fx32 f0 __asm__("r0");
    register fx32 f1 __asm__("r1");

    s = (struct View *)iwram_3001e80;
    s->pitch = 0x98 << 8;
    s->v2.z = 0xff << 17;
    z = 0;
    s->v1.x = z;
    s->v1.y = z;
    s->v1.z = z;
    s->yaw = z;
    s->v2.y = z;
    gPhysVec[3] = z;
    gPhysVec[4] = z;
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
    f0 = 0xfa;
    __asm__ volatile ("" : : "r" (f0));
    f1 = 0xc0;
    divide = Func_80008ac;
    f1 <<= 8;
    f0 <<= 16;
    r = divide(f0, f1);
    f0 = 0xfa;
    f0 <<= 16;
    Func_8005258(f0, r, 0x7fff0000);
}
