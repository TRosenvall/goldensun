/* Func_80c0be4 and Func_80c0cec  --  0x080c0be4 / 0x080c0cec, cut from the tail
 * of goldensun/asm/rom_b5000/rom_bffb8_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/rom_b5000/rom_bffb8_a_c_c.o and before asm/rom_b5000/rom_bffb8_a_c_c_c.o
 * in goldensun/stage1.ld.  The parent .s keeps AnimTransitionIn (0x080c08ec)
 * and Func_80c0a24 (0x080c0a24); this is a TAIL SPLIT, not a whole-file
 * replacement.
 *
 * TWO NEAR-IDENTICAL CAMERA SETUPS. Both drop a translation into the view
 * block's second vector, run the yaw/pitch/look chain, push one point through
 * Func_80009c0, then hand a screen-space rectangle to Func_80c0a24. They differ
 * in exactly two places: 0x80c0cec takes the percentage as its fourth
 * parameter and 0x80c0be4 reads it from a local that is NEVER ASSIGNED; and
 * 0x80c0cec's transform input takes the CONSTANT 0xff<<17 for its z where
 * 0x80c0be4 takes the view block's own v2.z.
 *
 * Func_80c0cec's signature was already fixed by its caller in
 * src/rom_b5000/rom_bffb8_a_c_c_c.c: (int x, int y, int z, int w).
 * Func_80c0be4 has no caller anywhere in the tree.
 *
 * The three shifted constants are WHOLE VALUES, not source-level shifts.
 * gcc-2.96 splits a Thumb immediate with the SMALLEST shift that fits the mov
 * in 8 bits, so 0xc000 comes out `mov #0xc0 / lsl #8` and 0x780000 comes out
 * `mov #0xf0 / lsl #15` -- neither is evidence of a `<<` in the source.
 *
 * ---------------------------------------------------------------------------
 * `lsl r0, r4, #16` IN Func_80c0be4 IS AN UNINITIALISED READ.
 *
 * r4 is never written there and is not a parameter register. This is the
 * recorded shape from "a dead `mov rN, r14` right after `push {lr}` means an
 * uninitialised read": a pseudo that is live-in with no reaching definition
 * takes the first free hard register out of REG_ALLOC_ORDER, which arm.h:989
 * gives as `3, 2, 1, 0, 12, 14, 4, ...`.  r0-r2 hold the arguments, r3 is
 * consumed by the `ldr r3, =iwram_3001e80` that the missing fourth parameter
 * leaves free, and r12/r14 are not LO_REGS so a Thumb `lsl` cannot use them.
 * r4 is next and costs no push under `-fcall-used-r4`.  A bare `int u;`
 * divided as `(u << 16) / 100` reproduces it exactly.  That is almost
 * certainly the original's own bug: the twin divides its fourth parameter and
 * this one was copied without one.
 *
 * NEW (grepped first as "uninitialised", "never assigned", "garbage"): the
 * recorded entry is about r14 and about GetUnit.  This is the same mechanism
 * landing on r4, and it says something the r14 case could not -- the register
 * the read lands on is a READABLE STATEMENT ABOUT THE ARGUMENT COUNT.  r4
 * rather than r3 is only reachable if r3 is busy, i.e. if the function takes
 * fewer than four arguments and gcc has already spent r3 on the global load.
 *
 * A file-scope `register int __asm__("r4")` produces the same instruction and,
 * against the finished source, the same object -- both spellings measure 0.
 * It was the ONLY spelling that worked while the Func_80009c0 site was still
 * wrong (12 differing against the plain local's 22), which is a trap: it looked
 * like the lever and was really masking one, because reserving r4 TU-wide also
 * stopped gcc reusing it as scratch.  Once the real cure was found the mask
 * became unnecessary.  The plain local ships: it reserves nothing, warns
 * nothing, and is what the original almost certainly said.
 *
 * ---------------------------------------------------------------------------
 * THE INTERLEAVE. Both functions carry the batch-127 shape at the first
 * Func_80008ac call -- three zero stores landing inside the split builds of
 * 0x1fe0000 and 0xc000:
 *
 *     mov r6,#0xff / ldr r2,=Func_80008ac / add r3,sp,#4 / mov r5,#0 /
 *     lsl r6,#0x11 / mov r1,#0xc0 / str r5,[r3] x3 / ... / lsl r1,#8
 *
 * A BARE UNIFORM CALL DOES NOT GET IT (28 differing / 31), and the cause is not
 * scheduling: at -O2 gcc COMMONS 0xc000 across the two Func_80008ac sites into
 * r7, which pushes the division result out of r7 into r11 and renames five
 * registers downstream.  Neither `-fno-rerun-cse-after-loop` nor separately
 * named locals -- the two remedies on record for a commoned constant -- moves
 * it, and the push-list tell is unavailable because r7 is pushed either way.
 *
 * What works is the pinned whole-value fill, ASCENDING, at both sites, and the
 * pin sizes are per-site exactly as the note warns:
 *
 *     site 1   q0 AND q1        dropping q0 costs 2, dropping q1 costs 26
 *     site 2   q0 only          dropping q0 costs 2, adding q1 is inert
 *
 * ---------------------------------------------------------------------------
 * THE LEVER THAT CARRIED Func_80c0be4 is not a pin.  Hoisting
 *
 *     k = 0xff << 17;
 *
 * ABOVE the `__divsi3` call, so its live range crosses the division, takes it
 * from 13 differing to 0.  gcc rematerialises the mov/lsl pair after the call
 * anyway -- the constant is cheaper to rebuild than to keep -- but the range is
 * enough to make `k` conflict with the y parameter, which moves y out of low r6
 * into r9 and lets the division result keep r7.  Without it y takes r6, the
 * ROM's second `mov r2, ...` in the v1 store block disappears and the function
 * comes out an instruction short.  Assigning a constant where it is first used,
 * the obvious spelling, is what is wrong here.
 *
 * The hoist is INERT in Func_80c0cec (measured 0 either way) and is written
 * there only to keep the twins identical; it is the load-bearing statement in
 * Func_80c0be4.
 *
 * ---------------------------------------------------------------------------
 * ONE SITE, TWO OPPOSITE CURES -- the Func_80009c0 call.
 *
 * Func_80c0be4 reads `s->v2.z` for the transform input.  `s` lives in r8, a HI
 * register, so the load needs a LO copy -- and so does the second argument.
 * The ROM makes two:
 *
 *     mov r2, r8 / ldr r3, [r2, #0x20] / mov r1, r8 / str r3, [r0, #8]
 *
 * Written the obvious way gcc emits ONE: reload inherits the load's base from
 * the argument copy already sitting in r1, and the function is an instruction
 * short.  Nothing about the LOAD's spelling changes that -- a `char *` cast, a
 * second struct tag, a `volatile` read, a pinned r2 base and an `__asm__`
 * identity were all measured and all still inherit, and so does a pinned
 * p0/p1 argument fill (20).  What settles it is WHERE THE CALLEE POINTER IS
 * MATERIALISED.  With
 *
 *     t.z = s->v2.z;
 *     g = Func_80009c0;
 *     g(&t, &s->v0);
 *
 * the load is demanded while r1 is still free and reload has to spill the base
 * into r2.  Swapping those two statements costs 44; assigning `g` early, beside
 * `f`, costs 21 because gcc then keeps it in a callee-saved register instead of
 * rebuilding the pool load at the call.
 *
 * Func_80c0cec has no load there -- its z is the constant -- and wants the
 * OPPOSITE: a bare t.x/t.y/t.z fill costs 2 and the pinned p0/p1 whole-value
 * fill is what matches.  Two sites of identical silhouette, opposite cures.
 *
 * REFINES "`mov rLow, rHigh` before a store is a SECOND reload", which reads
 * the same instruction from the store side and concludes it is evidence that a
 * `bl` intervenes.  Here there is NO call between the two copies -- they are a
 * LOAD BASE and an ARGUMENT REGISTER competing for the same value in the same
 * straight line, and whether gcc emits one or two is decided by whether the
 * argument's own materialisation has already been demanded when the load is
 * reloaded.  So the second copy has a second cause, and the lever for it is
 * statement order after all, which is exactly what that entry says not to
 * sweep.  Check for an intervening `bl` first; if there is none, this is the
 * other case.
 *
 * ---------------------------------------------------------------------------
 * `bl _call_via_fp` vs the ROM's `bl _call_via_r11` is NOT a difference.
 * src/lib/call_via.s defines both names at one address ("gcc emits `fp` for
 * r11"); `arm-none-eabi-nm src/lib/call_via.o` shows `_call_via_fp` and
 * `_call_via_r11` both at 0x2c.  tools/objcmp.py compares relocations by SYMBOL
 * NAME, so it reports "RELOCATIONS differ" on these two calls per function
 * while SIZE and ENCODINGS are both silent.  Renaming the symbol in a scratch
 * copy of the reference gives
 *
 *     OK Func_80c0be4 -- 264 bytes, 110 encodings and 18 relocations identical
 *     OK Func_80c0cec -- 264 bytes, 109 encodings and 18 relocations identical
 */
#include "gba/types.h"

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")

#define PIN2P register vec3_t *p0 __asm__("r0"); \
              register vec3_t *p1 __asm__("r1")

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
extern int Func_80008ac(int a, int b);
extern void Func_8005258(int a, int b, int c);
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int a);
extern void MatrixPitch(int a);
extern void Func_80009c0(vec3_t *a, vec3_t *b);
extern void MatrixSetLook(struct View *a, vec3_t *b);
extern void PhysMove(vec3_t *in, vec3_t *out);
extern void Func_80c0a24(int a, int b, int c, int d, int e);

void Func_80c0be4(int x, int y, int z)
{
    struct View *s;
    vec3_t *v;
    vec3_t t;
    vec3_t out;
    vec3_t in;
    int q;
    int u;
    int zero;
    int k;
    int (*f)(int, int);
    void (*g)(vec3_t *, vec3_t *);

    s = (struct View *)iwram_3001e80;
    v = &s->v1;
    k = 0xff << 17;
    q = (u << 16) / 100;
    v->x = x;
    v->y = y;
    v->z = z;
    zero = 0;
    in.x = zero;
    in.y = zero;
    in.z = zero;
    f = Func_80008ac;
    { PIN2; q0 = k; q1 = 0xc0 << 8; Func_8005258(k, f(q0, q1), k * 2); }
    InitMatrixStack();
    MatrixTranslatev(v);
    MatrixYaw(s->yaw);
    MatrixPitch(s->pitch);
    t.x = zero;
    t.y = zero;
    t.z = s->v2.z;
    g = Func_80009c0;
    g(&t, &s->v0);
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x78;
    InitMatrixStack();
    MatrixSetLook(s, v);
    { PIN2P; p1 = &out; PhysMove(&in, p1); }
    Func_80c0a24(0xf0 << 15, 0xf0 << 15, (0x78 - out.x) << 8, (0x78 - out.y) << 8, q);
    { PIN2; q0 = q * 510; Func_8005258(q * 510, f(q0, 0xc0 << 8), q * 1020); }
}

void Func_80c0cec(int x, int y, int z, int w)
{
    struct View *s;
    vec3_t *v;
    vec3_t t;
    vec3_t out;
    vec3_t in;
    int q;
    int zero;
    int k;
    int (*f)(int, int);
    void (*g)(vec3_t *, vec3_t *);

    s = (struct View *)iwram_3001e80;
    v = &s->v1;
    k = 0xff << 17;
    q = (w << 16) / 100;
    v->x = x;
    v->y = y;
    v->z = z;
    zero = 0;
    in.x = zero;
    in.y = zero;
    in.z = zero;
    f = Func_80008ac;
    { PIN2; q0 = k; q1 = 0xc0 << 8; Func_8005258(k, f(q0, q1), k * 2); }
    InitMatrixStack();
    MatrixTranslatev(v);
    MatrixYaw(s->yaw);
    MatrixPitch(s->pitch);
    g = Func_80009c0;
    { PIN2P; p0 = &t; p1 = &s->v0; p0->x = zero; p0->y = zero; p0->z = k; g(p0, p1); }
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x78;
    InitMatrixStack();
    MatrixSetLook(s, v);
    { PIN2P; p1 = &out; PhysMove(&in, p1); }
    Func_80c0a24(0xf0 << 15, 0xf0 << 15, (0x78 - out.x) << 8, (0x78 - out.y) << 8, q);
    { PIN2; q0 = q * 510; Func_8005258(q * 510, f(q0, 0xc0 << 8), q * 1020); }
}
