/* ActorCmd_Camera (0x0800daf0) -- NON-MATCHING: 36 encodings of 235 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800daf0.c asm/rom_9000/rom_d924_c_c_a.s --func ActorCmd_Camera
 *
 * SIZE 492 == 492, INSTRUCTIONS 235 == 235, RELOCATIONS IDENTICAL (8, same
 * offsets, same symbols).  36 is therefore a TRUE DISTANCE.
 * The first 101 instructions -- prologue, all four bound computations, both null
 * tests, all four clamps and the whole goalFacing snap branch -- are EXACT.
 *
 * ================== THE cse2 BARRIER IS THE LEVER HERE TOO ==================
 * THIS IS THE THIRD FUNCTION IN THE ActorCmd_* / rom_d924 FAMILY THAT
 * `__asm__ ("" : "+r" (v))` HAS MOVED, after ActorCmd_Wander (10 -> 2) and its
 * file-mate.  Measured on this function, single drop:
 *
 *     no barrier                     189 of 239, 500 bytes (4 insns TOO MANY)
 *     barrier on cy at its use site   36 of 235, 492 bytes (exact length)
 *
 * The value is `cy` -- the target actor's y, read once at the top of the guarded
 * block, stored straight through in the snap branch and SUBTRACTED from the
 * actor's own y on the main path.  Same shape as Wander's x/z: one value that is
 * both passed onward and consumed by an arithmetic op whose result outlives it.
 *
 * Without the barrier `cy` (pseudo 41) is DEAD LAST of the 23 allocnos in
 * global.c's priority sort, so minZ and minX take r5/r6 ahead of it and it is
 * pushed to r9.  The ROM has cy in r5 (sharing it with `step`, which is born
 * after cy dies) and minZ/minX one register higher, at r6/r7.  A high register
 * costs `mov r9,r7` at the load, `mov r4,r9 / mov r9,r4` around the subtract and
 * a re-copy of `a` -- exactly the four extra instructions.
 *
 * Computed from allocno_compare (global.c, read in the image): priority is
 * floor_log2(n_refs)*n_refs/live_length.  cy has 3 refs and ~2x minZ's live
 * length, so it CANNOT out-rank minZ by that formula at 3 refs -- it would need
 * 5.  The barrier is what reaches the ROM's assignment, and it is per-site where
 * a flag would be per-TU.  Neither -fno-rerun-cse-after-loop nor -fno-gcse moves
 * this function at all (both 38 lines dirty, same as plain -O2), which is a
 * DIFFERENCE from ActorCmd_Wander: there the flag reached the same shape as the
 * barrier.  Here only the barrier does.
 *
 * ================== THE RESIDUE: RELOAD SCRATCH ROTATION ==================
 * Every one of the 36 is a low-register scratch swap, r2 <-> r4 (with r3/r5
 * following), in the five blocks after the first division.  Sample:
 *
 *   ROM   mov r2, r8 / ldr r3, [r2, #0x10] / ldr r4, [sp, #4] ... sub r0, r4, r3
 *   OURS  mov r4, r8 / ldr r3, [r4, #0x10] / ldr r2, [sp, #4] ... sub r0, r2, r3
 *
 * The block BEFORE this one uses r2 in both, so the ROM REUSES r2 for the next
 * block's copy of `a` and we rotate to r4 -- allocate_reload_reg's
 * `last_spill_reg` walk (reload1.c:5003).  Same swap again at the `[r?,#0x30]`
 * read, the 0x4000 build, the low-distance store pair and the Func_80008ac pool
 * load.  Blocker class: SCRATCH-REGISTER SELECTION (docs/elevation.md
 * "SCRATCH-REGISTER SELECTION is a distinct wall"), pass = reload1.
 *
 * MEASURED AND REJECTED, all single changes from the 36 above:
 *   - a second barrier on `cx`: 48.   - a barrier on `dx`: 51.
 *   - a barrier on `cx`/`cz` INSTEAD of `cy` (the literal Wander spelling):
 *     177 of 237 at 496 bytes -- wrong value, and it does not fix the length.
 *   - merging `cy` and `dy` into one variable (the merge lever): 176 of 237,
 *     496 bytes.  The ROM's `sub r5,r3 / mov r9,r5` is two pseudos, not one.
 *   - naming `p3 = dz*dz` (the lever that closed Actor_TravelTo's sum in this
 *     same batch): 36, INERT here -- two terms, not three, so there is no third
 *     product to give its own register.
 *   - `mag = dz*dz + dx*dx`: 36 (inert).  Swapping the two divisions: 50.
 *     Dropping `mag` and inlining the sum in the call: 38.
 *   - splitting each division into two statements: 225 of 237, 496 bytes.
 *   - declaration-order permutations of cx/cy/cz: INERT (189 every time),
 *     as docs/elevation.md says for register-resident locals.
 *   - load-order permutations of cx/cy/cz: 189/192/193/196 -- all worse or equal.
 *   - reordering the four bound computations: 193, 201.
 *   - -fno-schedule-insns2: 92 (an actively misleading probe, as recorded).
 *
 * NEXT: this needs the reload scratch order, not another spelling.  Read
 * reload1.c allocate_reload_reg under gdb for the first differing block and find
 * what the ROM had live in r4 there that we have live in r2.
 *
 * THE BARRIER BELOW IS A VERIFICATION SHIM IN A PARK, not a landing route.  If
 * this function is ever taken to exact it must be booked in fakematch.txt.
 */
#include "math.h"

extern int Func_8000948(int v);
extern int Func_80008ac(int a, int b);
extern int FastIntSqrtFP1616_RAM(int v);
extern unsigned char *iwram_3001e70;

int ActorCmd_Camera(unsigned char *r0)
{
    unsigned char *a;
    unsigned char *t;
    unsigned char *m;
    int minX;
    int minZ;
    int maxX;
    int maxZ;
    int cx;
    int cy;
    int cz;
    int ty;
    int dx;
    int dy;
    int dz;
    int mag;
    int dist;
    int step;
    int (*fp)(int);
    int (*dv)(int, int);

    a = r0;
    m = iwram_3001e70;
    minX = *(int *)(m + 0xec) + (0xf0 << 15);
    t = *(unsigned char **)(a + 0x68);
    ty = *(int *)(t + 0xc);
    minZ = *(int *)(m + 0xf0) + ty + (0xc0 << 15);
    maxX = *(int *)(m + 0xf4) - (0xf0 << 15);
    maxZ = *(int *)(m + 0xf8) + ty - (0x80 << 15);
    *(a + 0x55) = 0;
    if (t == 0)
        goto done;
    if (*(int *)t == 0)
        goto done;
    cx = *(int *)(t + 8);
    cy = *(int *)(t + 0xc);
    cz = *(int *)(t + 0x10);
    *(int *)(a + 0x38) = 0x80 << 24;
    *(int *)(a + 0x3c) = 0x80 << 24;
    *(int *)(a + 0x40) = 0x80 << 24;
    if (cx < minX)
        cx = minX;
    if (cz < minZ)
        cz = minZ;
    if (cx > maxX)
        cx = maxX;
    if (cz > maxZ)
        cz = maxZ;
    if (*(short *)(a + 0x64) != 0) {
        *(int *)(a + 8) = cx;
        *(int *)(a + 0xc) = cy;
        *(int *)(a + 0x10) = cz;
        goto done;
    }
    dx = (cx - *(int *)(a + 8)) / 0x10000;
    dz = (cz - *(int *)(a + 0x10)) / 0x10000;
    mag = dx * dx + dz * dz;
    fp = Func_8000948;
    dist = fp(mag) << 16;
    dx = cx - *(int *)(a + 8);
    __asm__ ("" : "+r" (cy));
    dy = cy - *(int *)(a + 0xc);
    dz = cz - *(int *)(a + 0x10);
    if (dist < 0x80 << 15) {
        int m1;
        int m2;
        m1 = fx32_multiply(dx, dx);
        m2 = fx32_multiply(dz, dz);
        dist = FastIntSqrtFP1616_RAM(m1 + m2);
    }
    step = dist / 8;
    if (step > *(int *)(a + 0x30))
        step = *(int *)(a + 0x30);
    if (dist < 0x80 << 7) {
        *(int *)(a + 8) = cx;
        *(int *)(a + 0x10) = cz;
    } else {
        if (dist > step) {
            int q;
            dv = Func_80008ac;
            q = dv(dist, dx);
            dx = fx32_multiply(q, step);
            q = dv(dist, dz);
            dz = fx32_multiply(q, step);
        }
        *(int *)(a + 8) += dx;
        *(int *)(a + 0x10) += dz;
    }
    if ((dy < 0 ? -dy : dy) > 0x80 << 8)
        dy = dy / 4;
    *(int *)(a + 0xc) += dy;
done:
    *(unsigned short *)(a + 4) += 1;
    return 1;
}
