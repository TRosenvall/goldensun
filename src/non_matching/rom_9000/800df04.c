/* ActorCmd_Unk9 (0x0800df04) -- NON-MATCHING: 40 encodings of 367 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/ActorCmd_Unk9.c asm/rom_9000/rom_d924_c_c_c_c.s \
 *       --func ActorCmd_Unk9
 *
 * SIZE 796 == 796, INSTRUCTIONS 367 == 367, RELOCATIONS IDENTICAL (28, same offsets,
 * same symbols).  40 is a TRUE DISTANCE.  First candidate was 326 of 367 at 792 bytes.
 *
 * The leash-aware sibling of ActorCmd_Wander in the same .s.  The file is 2 of 2 and
 * would convert whole, so read ActorCmd_Wander's park with this one: its residue and
 * this one's are the SAME construct wanting OPPOSITE compiler behaviour, which is the
 * interesting fact about this file.
 *
 * WHAT THIS FUNCTION PROVES ABOUT ITS FILE-MATE.  ActorCmd_Wander is byte-exact under
 * -fno-rerun-cse-after-loop.  This function is 40 differing with cse2 ON and 50 with
 * it OFF, and the ten extra are exactly the two coordinate divisions of its in-loop
 * leash test flipping to the destructive/copy-swapped form:
 *
 *     rom (cse2 ON)    ldr r1,[r7,#0] / adds r2,r1,#0 / cmp r1,#0 / ... adds r2,r1,r4
 *     flag (cse2 OFF)  ldr r2,[r7,#0] / adds r1,r2,#0 / cmp r2,#0 / ... adds r2,r2,r4
 *
 * ActorCmd_Wander's ROM has the SECOND form for the identical construct.  A per-TU
 * Makefile row is all-or-nothing, so -fno-rerun-cse-after-loop cannot land this file,
 * and the difference between the two functions must be source-level.
 *
 * WHAT IS RIGHT (326 -> 160 -> 40, each step measured):
 *   - THE NAMED POINTERS ARE NOT IN THE SOURCE.  `*(short *)(a + 0x64)` written inline
 *     at BOTH of its two sites -- the pre-loop leash test and the in-loop one -- is
 *     what produces the ROM's `adds r4,r6,#0 / adds r4,#0x64 / str r4,[sp,#8]`: cse
 *     builds the common address itself and the spill is gcc's, not a declared local.
 *     A declared `short *sxp` is 160 of 367 at best and costs the two instructions that
 *     commit it early.  This is the counterpart of the recorded "do not name the
 *     sub-struct" rule, for an address rather than a struct.
 *   - BOTH ANGLE TRUNCATIONS ARE IN THE SOURCE AND THEY MUST BE TWO STATEMENTS.
 *     `ang = (unsigned short)face + (Random() >> 2) - (Random() >> 2); ang =
 *     (unsigned short)ang;` gives the ROM's four shift instructions per loop.  Folded
 *     into one expression, `(unsigned short)((unsigned short)face + ...)`, gcc drops
 *     the inner truncation as redundant mod 2^16 and the function is 365 instructions
 *     against 367 -- the count is the tell.  `& 0xffff`, an `unsigned short face`, and
 *     a separate truncation statement before the Randoms are all 369 (two too many).
 *   - `face` IS AN `int` HOLDING A SIGNED SHORT READ.  `*(short *)(a + 6)` is `ldrsh`
 *     and the outside path's `(short)(atan2(dz, dx) + 0x8000)` is `lsl #16 / asr #16`
 *     into a word slot -- an `unsigned short face` would give `lsr` and `strh`.
 *   - THE OUTSIDE PATH IS ONE LABEL REACHED FROM TWO PLACES (the pre-loop leash test
 *     and `i > 7`), which is why a plain `if (i > 7) goto outside;` gives the ROM's
 *     block order here where ActorCmd_Wander needed an `if/else`: gcc cannot move a
 *     block with two predecessors up next to the first test.
 *   - `dx`/`dz` ARE ONE PAIR OF LOCALS REUSED by the pre-loop test, the in-loop test
 *     and `atan2` -- the ROM keeps them in r11/r9 across all three.
 *   - Two separate angle and magnitude locals for the two loops: loop A's angle is
 *     spilled to sp+0 and reloaded after every call while loop B's lives in r7, so they
 *     cannot be one pseudo.
 *   - The first `vec3_translate` of each loop takes the LITERAL 0x80000, not `mag`.
 *   - `void Actor_TravelTo`, `u32 Random`, `fx32_multiply` from include/math.h,
 *     field-by-field vec3 copies, `*op++`, `p` declared before `q` -- all as in
 *     ActorCmd_Wander's park, same evidence.
 *
 * MEASURED NEGATIVES: moving the vec3 copy before or after the angle statement is 253
 * and 291 (the ROM's interleaving is sched2's, not source order); `2 | *(a + 0x59)`
 * and `&= ~2` are both inert against `|= 2` and `&= 0xfd`; splitting the pre-loop
 * division from its subtraction is 160 and costs an early `mov fp`.
 *
 * WHERE THE 40 ARE: about 24 in the two angle blocks, where the ROM keeps the whole
 * chain in one low register and interleaves the vec3 stores into it while ours commits
 * the subtraction straight into the angle's own register; about 6 in the prologue
 * scheduling of the two `str [sp,#16]` stores and `mov sl`; 2 in the `orr` operand
 * order at +0x59; the rest scattered register choices.
 *
 * NEXT: the angle chain's register.  The ROM's `subs r2,r2,r0` keeps the sum in the
 * scratch register and only the final `lsr` writes the angle's own; ours writes the
 * angle's register at the `subs`.  That is the same "which pseudo does the copy go to"
 * question as ActorCmd_Wander's residue, one statement earlier.
 */
#include "gba/types.h"
#include "math.h"

extern u32 Random(void);
extern u16 atan2(int y, int x);
extern void vec3_translate(int mag, int angle, vec3_t *v);
extern int Func_800d924(unsigned char *a, vec3_t *v);
extern int TestCollision(unsigned char *a, vec3_t *v);
extern void Actor_TravelTo(unsigned char *a, int x, int y, int z);

int ActorCmd_Unk9(unsigned char *r0)
{
    unsigned char *a;
    int *op;
    int base;
    int scale;
    int leash;
    int face;
    int i;
    int mag;
    int ang;
    int mag2;
    int ang2;
    int dx;
    int dz;
    vec3_t p;
    vec3_t q;

    a = r0;
    op = (int *)(*(int *)a + 4 + 4 * *(short *)(a + 4));
    base = *op++;
    scale = *op++;
    leash = *op / 0x10000;
    i = 0;
    face = *(short *)(a + 6);
    leash = leash * leash;
    dx = *(int *)(a + 8) / 0x10000 - *(short *)(a + 0x64);
    dz = *(int *)(a + 0x10) / 0x10000 - *(short *)(a + 0x66);
    if (dx * dx + dz * dz > leash)
        goto outside;
retryA:
    i++;
    if (i > 7)
        goto outside;
    mag = base + fx32_multiply(Random(), scale);
    ang = (unsigned short)face + (Random() >> 2) - (Random() >> 2);
    ang = (unsigned short)ang;
    p.x = *(int *)(a + 8);
    p.y = *(int *)(a + 0xc);
    p.z = *(int *)(a + 0x10);
    vec3_translate(0x80000, ang, &p);
    if (Func_800d924(a, &p))
        goto retryA;
    p.x = *(int *)(a + 8);
    p.y = *(int *)(a + 0xc);
    p.z = *(int *)(a + 0x10);
    vec3_translate(mag, ang, &p);
    if (TestCollision(a, &p))
        goto retryA;
    q.x = *(int *)(a + 8);
    q.y = *(int *)(a + 0xc);
    q.z = *(int *)(a + 0x10);
    mag += 0x80000;
    vec3_translate(mag, ang, &q);
    if (TestCollision(a, &q))
        goto retryA;
    q.x = *(int *)(a + 8);
    q.y = *(int *)(a + 0xc);
    q.z = *(int *)(a + 0x10);
    vec3_translate(mag, ang + 0x2000, &q);
    if (TestCollision(a, &q))
        goto retryA;
    q.x = *(int *)(a + 8);
    q.y = *(int *)(a + 0xc);
    q.z = *(int *)(a + 0x10);
    vec3_translate(mag, ang - 0x2000, &q);
    if (TestCollision(a, &q))
        goto retryA;
    q.x = *(int *)(a + 8);
    q.y = *(int *)(a + 0xc);
    q.z = *(int *)(a + 0x10);
    vec3_translate(mag, ang + 0x4000, &q);
    if (TestCollision(a, &q))
        goto retryA;
    q.x = *(int *)(a + 8);
    q.y = *(int *)(a + 0xc);
    q.z = *(int *)(a + 0x10);
    vec3_translate(mag, ang - 0x4000, &q);
    if (TestCollision(a, &q))
        goto retryA;
    dx = p.x / 0x10000 - *(short *)(a + 0x64);
    dz = p.z / 0x10000 - *(short *)(a + 0x66);
    if (dx * dx + dz * dz > leash)
        goto retryA;
    *(a + 0x59) |= 2;
    Actor_TravelTo(a, p.x, p.y, p.z);
    goto done;
outside:
    i = 0;
    face = (short)(atan2(dz, dx) + 0x8000);
retryB:
    i++;
    if (i > 7)
        goto done;
    mag2 = base + fx32_multiply(Random(), scale);
    ang2 = (unsigned short)face + (Random() >> 2) - (Random() >> 2);
    ang2 = (unsigned short)ang2;
    p.x = *(int *)(a + 8);
    p.y = *(int *)(a + 0xc);
    p.z = *(int *)(a + 0x10);
    vec3_translate(0x80000, ang2, &p);
    if (Func_800d924(a, &p))
        goto retryB;
    p.x = *(int *)(a + 8);
    p.y = *(int *)(a + 0xc);
    p.z = *(int *)(a + 0x10);
    vec3_translate(mag2, ang2, &p);
    if (TestCollision(a, &p))
        goto retryB;
    *(a + 0x59) &= 0xfd;
    Actor_TravelTo(a, p.x, p.y, p.z);
done:
    *(unsigned short *)(a + 4) += 4;
    return 1;
}
