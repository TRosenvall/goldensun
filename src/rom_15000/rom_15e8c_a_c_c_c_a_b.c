/* Cluster Func_8016670..Func_8016670 extracted from goldensun/asm/rom_15000/rom_15e8c_a_c_c_c.s.
 *
 * Total .text for this TU = 172 bytes (= 0xac). Never attempted before batch 274.
 * No pins, no flags.
 *
 * ITS STRUCT CAME FROM THE PARK NEXT DOOR. src/non_matching/rom_15000/80165d8.c holds the
 * sibling Func_80165d8, which stays in assembly, and its `struct Slot` transferred unchanged
 * except that `f0` is retyped `struct Slot *` -- the ROM does `ldr r3, [r5] / ldrh r3, [r3,
 * #0x14]`, so the first word points at a record with a halfword at 0x14, which is the same
 * layout. No new struct was invented. Fourth function in two batches to land off a park.
 *
 * FIVE LEVERS:
 *   - THE `< 0xd00` ARM MUST BE THE `if` AND THE CALL THE `else`. Written the other way
 *     gcc inverts the branch, cross-jumps the two `strh r3, [r5, #6]` tails into one, and
 *     rebuilds 0xd00 twice.
 *   - THE FIELD MUST BE RE-READ, NOT CACHED. `v = s->f6;` then `s->f6 = v + 0xd00;` is one
 *     register short; reading `s->f6` three times makes gcse turn the redundant load into
 *     the ROM's `mov r2, r3` copy, which IS the missing 94th instruction. Five caching
 *     spellings are all inert at 93/53. The `(unsigned int)` cast on the range test is what
 *     gives the ROM's `bcs`.
 *   - THE TRAILING CLEAR-LOOP COUNTER MUST BE `unsigned` with `i <= 3`: that matches
 *     neither arm of check_dbra_loop's `LT || (LE && no_use_except_counting)` gate, so the
 *     loop stays ascending with `bls`, where `int` reverses it to `mov #3 / bge`.
 *   - AND IT MUST BE THE SAME VARIABLE as the slot-scan counter -- that is what puts it in
 *     the ROM's r4.
 *   - THE STORED 0 MUST BE A NAMED LOCAL SHARED WITH THE FIELD ZEROS. `*p = 0` through a
 *     `u16 *` is a pooled HImode zero (`ldr r1, =0`), the recorded halfword exception, also
 *     confirmed in the sibling park; the shared `z` lets it use the ROM's `mov r2, #0`.
 *   - `i = 0;` BEFORE `p = s->f8;` -- the `for`-init form emits the pointer init first and
 *     transposes `mov r4, #0` with `add r3, #8`.
 *
 * MEASURED (rom 94): first candidate 95/53; branch order plus the unsigned counter 95/54;
 * the shared `z` 93/53; the re-read field 94/11; one shared counter 94/2; hoisting `i = 0`
 * exact. Worse: `s->f8[k] = 0` and `p[k] = 0` both 95/54, storing `s->f14` 94/47.
 */
#include "gba/types.h"

struct Slot {
    struct Slot *f0;
    u16 f4;
    u16 f6;
    u16 f8[4];
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    u16 f18;
    u16 f1a;
    u16 pad1c;
    u16 f1e;
    u16 f20;
    u16 pad22;
    u16 f24;
    u16 pad26;
};

extern unsigned char *iwram_3001e8c;
extern void Func_80167d8(struct Slot *s);

struct Slot *Func_8016670(struct Slot *a, int b, int c)
{
    struct Slot *e;
    struct Slot *s;
    u16 *p;
    unsigned int i;
    int z;

    e = (struct Slot *)(iwram_3001e8c + (0xc4 << 3));
    s = NULL;
    for (i = 0; i != 3; i++) {
        if (e->f0 == NULL || e->f0->f14 != 0) {
            s = e;
            break;
        }
        e = (struct Slot *)((u8 *)e + 0x28);
    }
    if (s != NULL) {
        if (s->f0 == NULL) {
            s->f6 = 0xa0 << 4;
            s->f0 = a;
            s->f4 = 0xc0 << 2;
        } else if (c == 0) {
            if (s->f6 == 0) {
                s->f6 = 0xa0 << 4;
            } else if ((unsigned int)s->f6 < (0xd0 << 4)) {
                s->f6 = s->f6 + (0xd0 << 4);
            } else {
                Func_80167d8(s);
            }
            s->f4 = 0xc0 << 2;
        }
        z = 0;
        s->f1e = 0xc0 << 2;
        s->f0->f14 = z;
        s->f16 = 0xf;
        s->f1a = 0xa;
        s->f12 = b;
        s->f14 = z;
        s->f18 = z;
        s->f10 = z;
        s->f20 = z;
        i = 0;
        p = s->f8;
        for (; i <= 3; i++) {
            *p = z;
            p++;
        }
    }
    return s;
}
