/* Func_80b5864 -- 0x080b5864, split out of asm/rom_b5000/rom_b5368_a.s.
 *
 * LANDING: 248 bytes, 114 encodings and 9 relocations identical, at PRODUCTION
 * FLAGS. No per-file flag group. PINS: 0 (tools/shimcount.py).
 *
 * Verify with (inside the build container):
 *   python3 tools/objcmp.py src/rom_b5000/rom_b5368_a_b.c \
 *     asm/rom_b5000/rom_b5368_a_b.s --whole
 *
 * SPLIT SHAPE. The stem held FOUR functions and no data (tools/datacheck.py is
 * silent on it): Debug_LoadPresetParty, Func_80b5534, Debug_BattleTest and this
 * one, which is the LAST. tools/split_s.py leaves rom_b5368_a_a.s with the other
 * three and rom_b5368_a_b.s with this one, so it ships alone -- the piece did NOT
 * have to be converted whole. Its three file-mates stay hand asm and keep their
 * parks; split_s.py repoints their recipes.
 *
 * The camera/telemetry tick: it ages a serial-link watchdog, eases the yaw
 * angle one sixteenth of the way towards a target each frame while a countdown
 * runs, then rebuilds the view matrix and hands a forward offset to the
 * transform.
 *
 * TWO THINGS HAD TO CHANGE, and the park had the second one pointed the wrong
 * way.
 *
 * 1. THE WORD AT THE SIO REGISTER IS A POOL CONSTANT, NOT A SYMBOL. The park
 *    reached it with `extern volatile unsigned int X __asm__("REG_SIOCNT")`,
 *    which emits an R_ARM_ABS32 against a symbol of that name -- a TENTH
 *    relocation the reference does not have. `REG_SIOCNT` in include/gba.inc
 *    line 49 is a `.set`, an assembler constant, so the reference's
 *    `ldr r3, =REG_SIOCNT` assembles to a plain word with no relocation at all.
 *    Reading it as `(*(vu32 *)REG_ADDR_SIOCNT)` through the tree's own
 *    include/gba/io.h macro gives the same word and no relocation.
 *    GENERAL: an `__asm__` alias to a name that is only a `.set` in an .inc is
 *    always one relocation too many, and no encoding count can see it.
 *
 * 2. REUSING `sd` FOR THE QUOTIENT IS WHAT PUTS IT IN r3 -- AND IT RETIRES THE
 *    PARK'S REQUEST FOR A PER-FILE -fno-rerun-cse-after-loop RULE. Under that
 *    flag the park sat at four differing, a pure r2/r3 role swap in the
 *    `sd / 16` expansion; the ROM keeps the dividend in r3 and the +15 bias
 *    temp in r2 and gcc did the reverse. Every spelling of that expansion
 *    measured the same four: a two-statement subtract, an int temp for the
 *    target, a self-assignment, a short copy, `register short`, `signed short`,
 *    declaration order, `sd/16 + cur`, updating `cur` in place, and a
 *    hand-written ternary bias (worse). A saturated dimension.
 *
 *    The mechanism is in the compiler, not in the spelling. In .15.regmove the
 *    expansion is two pseudos: the sign-extended dividend (one def, two uses)
 *    and gcc's own bias temp (two defs, two uses, spanning three basic blocks
 *    because of the sign test). Both are global allocnos. `allocno_compare`
 *    (global.c:598-621) ranks by
 *    `floor_log2(n_refs) * n_refs / live_length`, so the bias temp scores 2*4
 *    against the dividend's 1*3, is allocated FIRST, and takes r3 -- the head
 *    of REG_ALLOC_ORDER (arm.h:989) -- leaving the dividend r2.
 *
 *    So raise the dividend's reference count, and the free way to do it is to
 *    reuse the user variable for the quotient instead of letting gcc use a
 *    temp:  `sd = c->f0 - cur; sd /= 16; v->f36 = cur + sd;`. That is one more
 *    def and one more use of the same pseudo, which flips the sort and hands it
 *    r3. `sd = sd / 16;` is byte-identical to `sd /= 16;`.
 *
 *    This needs NO FLAG: byte-exact at production -O2, where the park's own
 *    best flag-free figure had been far away and the flag was thought to be the
 *    mechanism. A SEPARATE named quotient -- `int q; q = sd / 16;`, which the
 *    park had tried -- is still four under the flag and worse without it: it is
 *    reusing the SAME variable that matters, not naming the quotient.
 *
 *    GENERAL RULE, new: when two temps of one gcc expansion compete for the
 *    head of REG_ALLOC_ORDER, reusing a user variable for the later value
 *    raises its n_refs and `allocno_compare` hands it the register.
 *
 * WHAT CARRIED OVER FROM THE PARK, unchanged and right: inverting the `(f64 & 3)`
 * test so the increment arm falls through, reading the third global as a negative
 * offset from the first, the order of the three globals (which is what makes the
 * prologue exact), the SIO id in a named local, and a `short` intermediate rather
 * than an `int` with a cast -- `int sd = (short)(x)` loses the `lsl/asr` pair
 * when the operand came from an unsigned halfword, while a `short` variable keeps
 * it. Measured again here: an `int` with an explicit `(short)` cast, and the cast
 * written inline with no variable at all, are both far worse than the `short`.
 */
#include "gba/types.h"
#include "gba/io.h"

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
            id = ((*(vu32 *)REG_ADDR_SIOCNT) << 26) >> 30;
            if (a->f50 != id)
                a->f52 = 1;
            a->f51 = 0;
        }
    }
    if (c->f4 != 0) {
        cur = v->f36;
        sd = c->f0 - cur;
        sd /= 16;
        v->f36 = cur + sd;
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
