/* ActorCmd_Wander (0x0800dd70) -- NON-MATCHING: 2 encodings of 187 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800dd70.c asm/rom_9000/rom_d924_c_c_c.s \
 *       --func ActorCmd_Wander
 *
 * SIZE 404 == 404, INSTRUCTIONS 187 == 187, RELOCATIONS IDENTICAL (13, same offsets,
 * same symbols).  2 is therefore a TRUE DISTANCE.
 *
 * ================== 10 -> 2 BY A CROSS-AGENT TRANSFER ==================
 * This park was written at 10 of 187 with the diagnosis below: byte-exact under
 * `-fno-rerun-cse-after-loop`, but the flag is per-TU and its file-mate ActorCmd_Unk9
 * needs the OPPOSITE, so the flag cannot land the file.  The park's own closing note
 * was that the difference must therefore be reachable from SOURCE and had not been
 * found.
 *
 * IT IS REACHABLE FROM SOURCE: `__asm__ ("" : "+r" (v))` on the surviving copy.  The
 * barrier defeats cse2 for ONE VALUE, where the flag defeats it for the whole
 * translation unit -- which is exactly the per-site/per-file distinction this file
 * needed.  Barriers on both `x` and `z`: 10 -> 2.  Measured singly: `z` alone is worth
 * 5, `x` alone 3 (7 of 187).
 *
 * WHAT CLASS OF PROBLEM THE BARRIER ACTUALLY SOLVES -- corrected in batch 292, because
 * the first version of this note got it wrong.  It said the barrier had been "found for
 * a repeated pool constant in overlay 971".  On a repeated CONSTANT the barrier is
 * INERT in every placement, measured over several in batch 292, and the mechanism says
 * why: `"+r"` expands to a copy in and a copy out, so the pseudo reaching the asm still
 * carries the known constant.  A barrier can make a value opaque DOWNSTREAM of itself;
 * it cannot un-know a constant UPSTREAM of itself.
 *
 * What it does solve is this file's class: a COPY DIRECTION -- which register a load
 * targets when cse2 collapses a temp into the surviving copy -- and live ranges
 * generally.  The overlay-971 result should be read the same way, not as a constant
 * case.  So reach for the barrier when the residue is "the right instructions through
 * the wrong registers", and not when it is "a constant built once and copied".
 *
 * ================== THE MECHANISM, read out of the -da dumps ==================
 *   expand    (set L (mem p.x)) / (set T L)   ; T = copy_to_suggested_reg for the sdiv
 *   cse pass 1  DOES THE COPY SWAP (the cse.c:5972 lever): the load now targets the
 *             DIV TEMP, the surviving copy is the value, and the add is destructive --
 *             THE ROM'S SHAPE.
 *   cse pass 2  UNDOES IT.  make_regs_eqv promotes NEW to the quantity's canonical
 *             register when NEW outlives the current cse block, and the copy L is by
 *             construction what survives to Actor_TravelTo.  So every read of T is
 *             rewritten to L, the add turns three-operand, and L takes the load.
 *
 *   ROM   ldr r3,[r7] / mov r1,r3 / cmp r3,#0 / bge / ldr r0,=0xffff / add r3,r0
 *   OURS  ldr r1,[r7] / mov r3,r1 / cmp r1,#0 / bge / ldr r0,=0xffff / add r3,r1
 *
 * Note the ROM's `mov r1,r3` assembles as 0x1c19 -- Thumb has no low-to-low MOV and
 * uses `ADD Rd,Rn,#0` -- which is what objcmp reports against our 0x6839 `ldr r1,[r7]`.
 *
 * ================== THE RESIDUE, AND WHAT DID NOT WORK ==================
 * The two remaining encodings are that same copy direction on the `x` pair only; `z`
 * is now the ROM's.  Measured and rejected, all of them:
 *   - divide first / coordinate second (the ordering that was exact UNDER THE FLAG):
 *     10 with no barrier, 59 and +4 bytes with barriers.  The ordering and the
 *     barrier are not additive -- they are alternative routes to the same swap, and
 *     combining them overshoots.
 *   - an explicit `tx = p.x; x = tx; dx = tx / 0x10000 - ...` naming the div temp,
 *     which is literally the ROM's shape: 10 without barriers, 71 and +4 bytes with.
 *     Naming the temp gives it its own quantity and cse1 then has nothing to swap.
 *   - a named `y` local for the third argument: 173 of 187 at 416 bytes, 193
 *     instructions.  The ROM reads p.y straight from memory (`ldr r2,[r7,#4]` at the
 *     call), so a local for it is six instructions of pure loss.
 *   - barriers on `dx`/`dz` instead of the coordinates: 23.
 *
 * WHY A MAKEFILE ROW IS STILL NOT THE ANSWER.  asm/rom_9000/rom_d924_c_c_c.s holds
 * exactly two functions, and ActorCmd_Unk9 contains the SAME construct -- a coordinate
 * divided by 0x10000 for a leash test and also handed to Actor_TravelTo -- but there
 * the ROM has GCC'S form, the three-operand add with the load in the surviving
 * register.  Its best candidate goes from 40 of 367 to 50 under the flag, and the ten
 * extra are exactly those two coordinate pairs.  A per-TU flag is all-or-nothing.
 * The barrier is per-site and therefore the right tool; it simply has not yet been
 * aimed at whatever still drives the x pair.
 *
 * THE BARRIERS BELOW ARE VERIFICATION SHIMS IN A PARK, not a landing route.  If this
 * function is ever taken to exact they must be booked in fakematch.txt.
 */
#include "gba/types.h"
#include "math.h"

extern u32 Random(void);
extern void vec3_translate(int mag, int angle, vec3_t *v);
extern int Func_800d924(unsigned char *a, vec3_t *v);
extern int TestCollision(unsigned char *a, vec3_t *v);
extern void Actor_TravelTo(unsigned char *a, int x, int y, int z);

int ActorCmd_Wander(unsigned char *r0)
{
    unsigned char *a;
    int *op;
    int base;
    int scale;
    int leash;
    int i;
    int mag;
    int ang;
    int dx;
    int dz;
    int x;
    int z;
    vec3_t p;
    vec3_t q;

    a = r0;
    op = (int *)(*(int *)a + 4 + 4 * *(short *)(a + 4));
    base = *op++;
    scale = *op++;
    leash = *op / 0x10000;
    i = 0;
    leash = leash * leash;
retry:
    i++;
    if (i <= 7) {
        p.x = *(int *)(a + 8);
        p.y = *(int *)(a + 0xc);
        p.z = *(int *)(a + 0x10);
        mag = base + fx32_multiply(Random(), scale);
        ang = *(unsigned short *)(a + 6) + (Random() >> 2) - (Random() >> 2);
        vec3_translate(mag, ang, &p);
        if (Func_800d924(a, &p))
            goto retry;
        if (TestCollision(a, &p))
            goto retry;
        mag += 0x80000;
        q.x = *(int *)(a + 8);
        q.y = *(int *)(a + 0xc);
        q.z = *(int *)(a + 0x10);
        vec3_translate(mag, ang, &q);
        q.x = *(int *)(a + 8);
        q.y = *(int *)(a + 0xc);
        q.z = *(int *)(a + 0x10);
        vec3_translate(mag, ang + 0x2000, &q);
        if (TestCollision(a, &q))
            goto retry;
        q.x = *(int *)(a + 8);
        q.y = *(int *)(a + 0xc);
        q.z = *(int *)(a + 0x10);
        vec3_translate(mag, ang - 0x2000, &q);
        if (TestCollision(a, &q))
            goto retry;
        x = p.x;
        __asm__ ("" : "+r" (x));
        dx = p.x / 0x10000 - *(short *)(a + 0x64);
        z = p.z;
        __asm__ ("" : "+r" (z));
        dz = p.z / 0x10000 - *(short *)(a + 0x66);
        if (dx * dx + dz * dz > leash)
            goto retry;
    } else {
        {
            int one;
            unsigned short *w;
            *(unsigned short *)(a + 6) += 0x8000;
            w = (unsigned short *)(a + 0x5e);
            one = 1;
            *w = one;
        }
        return 0;
    }
    Actor_TravelTo(a, x, p.y, z);
    *(unsigned short *)(a + 4) += 4;
    return 1;
}
