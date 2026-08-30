/* Func_80b5864  [rom_b5000]
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b5368.s
 *
 * BLOCKER CLASS: register-role swap -- BUT ONLY UNDER A FLAG THIS TU DOES NOT
 * CURRENTLY GET. Two numbers matter:
 *
 *     default flags                        56 differing (116 lines vs 117)
 *     -fno-rerun-cse-after-loop             4 differing (117 vs 117)
 *
 * So this is a THIRD CANDIDATE for the CSE_CFLAGS group in the Makefile, which
 * currently covers two TUs and whose comment flags itself FOR REVIEW as an
 * assumption about the original build on thin evidence. A third independent
 * function needing the same flag is worth weighing there; asm/rom_b5000/
 * rom_b5368.* has no override today.
 *
 * WHAT REMAINS UNDER THE FLAG is four lines in the `sd / 16` expansion, and it
 * is a pure role swap -- same instructions, same count, two registers
 * exchanged:
 *
 *     rom    asr r3, #0x10 / mov r2, r3 / add r2, #0xf / asr r3, r2, #0x4
 *     ours   asr r2, r3, #0x10 / mov r3, r2 / add r3, #0xf / asr r3, #0x4
 *
 * The ROM keeps the dividend in r3 and the +15 bias temp in r2; gcc does the
 * reverse.
 *
 * THE PROGRESSION IS WORTH KEEPING, because most of it was structural and none
 * of it was the flag: 99 differing on first transcription; 71 after inverting
 * the (f64 & 3) test so the increment arm falls through and reading the third
 * global as a negative offset from the first; 62 after reordering the three
 * globals, which made the prologue exact; 57 after naming the SIO id in a
 * local -- writing `if (sio != a->f50)` fixes the emission order but flips the
 * `cmp` operands, and the named local fixes both; 56 after using a `short`
 * intermediate; then 4 with the flag.
 *
 * TWO FACTS ESTABLISHED. `int` intermediates and `(short)` casts are NOT
 * interchangeable here: `int d = (short)(x)` loses the `lsl/asr #16` entirely
 * when the operand came from an unsigned halfword, while a `short` variable
 * keeps it. And `(REG_SIOCNT << 26) >> 30` is read as a WORD at 0x4000128,
 * not through REG_SIOCNT's `vu16` macro.
 *
 * Also measured, all 4 or worse: `sd/16 + cur`; three declaration orders;
 * `cur` as int with an explicit halfword read (6); naming the quotient;
 * hand-written rounding, which gcc re-canonicalises to identical output;
 * -fno-cse-follow-jumps, -fno-expensive-optimizations, -fno-schedule-insns,
 * -fno-strength-reduce, -fno-thread-jumps all 4; -fno-gcse 56; --no-sched2 33;
 * -fno-force-mem 93.
 */
#include "gba/types.h"

struct A {
    unsigned char pad00[0x44];
    unsigned char f44;
    unsigned char pad45[0xb];
    unsigned char f50;
    unsigned char f51;
    unsigned char f52;
};

struct V {
    unsigned char pad00[0xc];
    vec3_t vec;
    unsigned char pad18[4];
    vec3_t *f1c;
    fx32 f20;
    unsigned char pad24[0x10];
    short f34;
    short f36;
};

struct C {
    int f0;
    int f4;
    unsigned char pad08[0xc];
    int f14;
};

extern void *iwram_3001e80[];
extern unsigned short iwram_3001f64;
extern volatile unsigned int REG_SIOCNT_W __asm__("REG_SIOCNT");
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int angle);
extern void MatrixPitch(int angle);
extern void Func_80009c0(vec3_t *a, void *b);
extern void Func_80c0a24(int a, int b, int c, int d, int e);

static inline void Vec3Transform(vec3_t *a, void *b) {
    void (*transform)(vec3_t *, void *) = Func_80009c0;
    transform(a, b);
}

void Func_80b5864(void)
{
    struct V *v;
    struct A *a;
    struct C *c;
    vec3_t *m;
    vec3_t t;
    short sd;
    int id;
    unsigned short cur;

    v = iwram_3001e80[0];
    c = iwram_3001e80[0x20];
    a = *(void **)((char *)iwram_3001e80 - 0xc);
    if (a->f44 != 0) {
        if ((iwram_3001f64 & 3) != 3) {
            a->f51++;
            if (a->f51 > 24)
                a->f52 = 1;
        } else {
            id = (REG_SIOCNT_W << 26) >> 30;
            if (a->f50 != id)
                a->f52 = 1;
            a->f51 = 0;
        }
    }
    if (c->f4 != 0) {
        cur = v->f36;
        sd = c->f0 - cur;
        v->f36 = cur + sd / 16;
        c->f4 = c->f4 - 1;
    }
    m = &v->vec;
    if (v->f1c != 0)
        m = v->f1c;
    InitMatrixStack();
    MatrixTranslatev(m);
    MatrixYaw(v->f36);
    MatrixPitch(v->f34);
    t.x = 0;
    t.y = 0;
    t.z = v->f20;
    Vec3Transform(&t, v);
    if (c->f14 == 0)
        Func_80c0a24(0xf0 << 15, 0xf0 << 15, 0, 0, 0x80 << 9);
}
