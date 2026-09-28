/* Func_80cc960 -- 0x080cc960, first of the two functions that were in
 * asm/rom_c9000/rom_cc5d8_a_a_a.s.  Text-only split (datacheck reports no data
 * section); Anim_DjinnSet stays in assembly as `_a` and remains parked.
 *
 * 396 bytes, 180 encodings and 15 relocations identical.  No pins, no shims, no
 * flags, no fakematch row -- the whole function is reached from plain C.
 *
 * FIVE LOAD-BEARING CONSTRUCTS, each confirmed by a single drop from the exact file:
 *
 *   (a) the particle pointer is TWO LOCALS, one per loop.  The ROM uses r5 in the
 *       init loop and r8 in the draw loop; merged into one it costs 6 instructions
 *       (164 of 186 differing).
 *   (b) an explicit `unsigned mask = 0xffff;` local.  This fixes the constant-pool
 *       WORD ORDER -- 0xffff before gBuffer -- and therefore the gBuffer relocation
 *       offset.  As a bare literal the mask is hoisted by move_movables AFTER
 *       `h = gBuffer`, so it lands second (5 differing).
 *   (c) `g++; i++;` and not `i++; g++;` at the outer loop's bottom, which decides
 *       which of `#1` / `#0x1c` gets r3 (4 differing).
 *   (d) `DrawLine` declared `void`, NOT `int` -- the int-return lever firing the
 *       OTHER way, worth 3.  Consistent with the rom_15000 reversal found this same
 *       batch: the direction of that lever is per callee, not per bank.
 *   (e) `int z = 0; vin.y = z; vin.z = z; i = z;` -- an int carrier for the SHARED
 *       zero, which flips the r2/r3 pair in the second loop's pre-header.  The last
 *       8 encodings.
 */
#include "gba/types.h"

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
} Part;

extern int *iwram_3001eec[];
extern Part gBuffer[];

extern int Random(void);
extern void InitMatrixStack(void);
extern void MatrixRoll(int a);
extern void MatrixPitch(int a);
extern void MatrixYaw(int a);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void DrawLine(int x0, int y0, int x1, int y1, int c);

void Func_80cc960(void)
{
    vec3_t vin;
    vec3_t v1;
    vec3_t v2;
    unsigned char *base;
    Part *g;
    Part *h;
    int n;
    int i;
    int w;
    int z;
    unsigned mask;

    base = (unsigned char *)iwram_3001eec[0];
    n = *(int *)(base + 0x778c);
    *(int *)(base + 0x778c) = n + 1;
    if (n == 0) {
        mask = 0xffff;
        i = 0;
        h = gBuffer;
        do {
            int v = Random() & 0xf;
            h->f0 = v + 0x30;
            h->f4 = v + 0x28;
            h->fc = Random() & mask;
            h->f10 = Random() & mask;
            h->f14 = Random() & mask;
            i++;
            h++;
        } while (i != (0x80 << 1));
    }
    z = 0;
    vin.y = z;
    vin.z = z;
    i = z;
    g = gBuffer;
    do {
        if (n > i / 4 && g->f0 > 0) {
            InitMatrixStack();
            MatrixRoll(g->f14);
            MatrixPitch(g->fc);
            MatrixYaw(g->f10);
            vin.x = g->f0;
            Func_80e3944(&vin, &v1);
            v1.x += 0x40;
            v1.y += 0x50;
            vin.x = g->f4;
            Func_80e3944(&vin, &v2);
            v2.x += 0x40;
            v2.y += 0x50;
            g->f4 -= 4;
            g->f0 -= 4;
            if (g->f4 < 0) {
                g->f4 = 0;
            }
            w = -g->f4 / 2;
            DrawLine(v2.x - 1, v2.y, v1.x - 1, v1.y, w + 0x30);
            DrawLine(v2.x, v2.y - 1, v1.x, v1.y - 1, w + 0x30);
            DrawLine(v2.x, v2.y, v1.x, v1.y, w + 0x38);
        }
        g++;
        i++;
    } while (i != 0x40);
    *(int *)(base + 0x7824) = 1;
}
