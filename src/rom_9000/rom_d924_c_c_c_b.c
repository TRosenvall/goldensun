/* ActorCmd_Wander (0x0800dd70) -- BYTE-EXACT.  Batch 310a.
 *
 * objcmp AT PRODUCTION FLAGS FOR THIS FILE (CSE_CFLAGS):
 *   OK ActorCmd_Wander -- 404 bytes, 187 encodings and 13 relocations identical
 * Measured 3x, identical every time.  --whole on the post-split .s is also OK:
 *   OK whole file -- 404 bytes, 187 encodings and 13 relocations identical
 *
 * SHIMS: ZERO.  tools/shimcount.py reports nothing -- no fakematch.txt row.
 * The two `__asm__ ("" : "+r" (x))` barriers the park carried are GONE; they
 * were a verification shim and they are not needed on this route.
 *
 * INSTALLED PATH (whole-TU, after the split below):
 *   src/rom_9000/rom_d924_c_c_c_b.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_9000/rom_d924_c_c_c_b.c \
 *     asm/rom_9000/rom_d924_c_c_c_b.s --whole
 *
 * ==================== WHAT CHANGED, AND WHY IT WAS MISSED ====================
 * The park sat at 2 of 187 behind two "+r" barriers and recorded the real
 * answer in its own prose: the function is byte-exact under
 * -fno-rerun-cse-after-loop.  It then rejected that route on the grounds that
 * "A per-TU flag is all-or-nothing", because the file-mate ActorCmd_Unk9 in the
 * same .s needs the OPPOSITE of the flag.
 *
 * THAT IS A SPLIT, NOT A BLOCKER.  asm/rom_9000/rom_d924_c_c_c.s holds exactly
 * two functions and NO data (tools/datacheck.py is silent, exit 0), so
 * ActorCmd_Wander can be given its own translation unit and its own flag row
 * while ActorCmd_Unk9 stays hand-written asm and never sees the flag.  The
 * generalisable rule: WHEN A PARK REJECTS A PER-FILE FLAG BECAUSE OF A FILE-MATE,
 * CHECK THE FUNCTION COUNT BEFORE BELIEVING IT -- a 2-function .s makes the
 * objection free to remove.
 *
 * TWO INGREDIENTS, AND BOTH ARE NEEDED:
 *  1. THE FLAG.  CSE_CFLAGS (-fno-rerun-cse-after-loop).  cse pass 1 performs
 *     the cse.c:5972 copy swap that gives the ROM's shape (the load targets the
 *     division temp and the surviving copy carries the value); cse pass 2's
 *     make_regs_eqv undoes it.  The flag stops pass 2.
 *  2. THE STATEMENT ORDER: THE DIVISION FIRST, THE COORDINATE SECOND.
 *     The park's order (coordinate first) is 10 of 187 WITH the flag and 10
 *     without -- the flag alone is inert on it.  Swapping to division-first is
 *     EXACT with the flag and 10 without.  So the flag and the ordering are not
 *     alternatives, as the park concluded from the barrier experiments; they
 *     are a CONJUNCTION, and testing either alone reads as inert.  That is why
 *     seventeen barrier spellings never found it.
 *
 * MEASURED, all four orderings, objcmp --func, ref 187 encodings:
 *     order                                 -O2     +CSE_CFLAGS
 *     x = p.x then dx = p.x/...  (the park)  10       10
 *     dx = p.x/... then x = p.x  (below)     10       EXACT
 *     both divisions then both coords        10       EXACT
 *     both coords then both divisions        16       16
 *     coords then divisions reusing x, z     16       16
 * The park's two-barrier spelling measures 2 of 187 at -O2, which is CLOSER on
 * the count and a DEAD END on the route -- a worked example of the standing
 * warning that a closer figure is not a shorter path.
 *
 * THE .call_via VENEER WAS NEVER THE BLOCKER HERE.  The ROM's one inline
 * `.call_via r3` site (asm line 70) is already reached by `fx32_multiply` from
 * include/math.h, which expands to the unpinned `bx %0` form, and it is exact.
 * The residue was entirely the cse2 copy direction, 65 instructions away from
 * the call.  Because the pins live in the shared header and not in this file,
 * shimcount reports 0 and no fakematch row is due.
 *
 * ================= SPLIT SHAPE (split_s.py --dry-run, verbatim) =============
 *   $ python3 tools/split_s.py asm/rom_9000/rom_d924_c_c_c.s ActorCmd_Wander --dry-run
 *   [dry-run] would write asm/rom_9000/rom_d924_c_c_c_b.s  (1 function(s), 210 lines)
 *   [dry-run] would write asm/rom_9000/rom_d924_c_c_c_c.s  (1 function(s), 393 lines)
 *   [dry-run] would REMOVE asm/rom_9000/rom_d924_c_c_c.s
 *   [dry-run] would rewrite stage1.ld
 *   now: verify `make compare` is still green, THEN write
 *        src/rom_9000/rom_d924_c_c_c_b.c and delete asm/rom_9000/rom_d924_c_c_c_b.s
 *
 *   _b.s = ActorCmd_Wander (this file replaces it)
 *   _c.s = ActorCmd_Unk9   (stays hand-written, keeps -O2, no flag row)
 *   NO .global list: split_s printed no cross-reference warning, the two bodies
 *   share no local label, and datacheck.py reports no data section.
 *   Only stage1.ld is rewritten (two lines in the original order).
 *
 * ========================= THE MAKEFILE ROW THAT IS DUE =====================
 * YES, a flag row is required -- the function is 10 of 187 without it.
 * CSE_CFLAGS is already defined at Makefile:808.  Add, beside the other
 * CSE_CFLAGS rows:
 *
 *   # CSE_CFLAGS, batch 310.  ActorCmd_Wander reads p.x and p.z twice each -- once
 *   # for the `/ 0x10000` leash test and once for the Actor_TravelTo argument.  cse
 *   # pass 1 makes the load target the division temp and lets the surviving copy
 *   # carry the value, which is the ROM's `ldr r3,[r7] / mov r1,r3`; pass 2's
 *   # make_regs_eqv promotes the copy back and turns both into second loads.  The
 *   # flag is only half the answer: the division must also be written BEFORE the
 *   # coordinate.  Its old file-mate ActorCmd_Unk9 needs the opposite, which is why
 *   # asm/rom_9000/rom_d924_c_c_c.s was split first.
 *   asm/rom_9000/rom_d924_c_c_c_b.o: src/rom_9000/rom_d924_c_c_c_b.c
 *   	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
 *   	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *   	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * tryc.makefile_flags currently returns [] for src/rom_9000/rom_d924_c_c_c_b.c,
 * so without the row every future screen of this file is at the wrong flags.
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
        dx = p.x / 0x10000 - *(short *)(a + 0x64);
        x = p.x;
        dz = p.z / 0x10000 - *(short *)(a + 0x66);
        z = p.z;
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
