/* Func_80c1ffc -- 0x080c1ffc, asm/rom_b5000/rom_c1a34_a_a_a_c.s.
 * NON-MATCHING, 419 encodings of 414.  NOT a distance (ref 876 bytes / 414 encodings against ours 920 / 435).  READ `--align`: 453 of 441.
 * Furthest out of the three; fails four of five conditions in elevation.md's own selection filter.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80c1ffc.c \
 *     asm/rom_b5000/rom_c1a34_a_a_a_c.s --func Func_80c1ffc
 * PARKED, batch 294 brief H.  406 instructions.  OPENED THIS BATCH; first
 * candidate, structure verified, a long way out.
 *
 * WHOLE-FILE CONVERSION, confirmed: `grep -ci func_start` = 1 and
 * `python3 tools/datacheck.py <ref>` exits 0.
 *
 * PARKED AT 453 of 441 (tools/tryc.py --align).  objcmp --whole says
 *     XX Func_80c1ffc   419 of 414 differ (ours 435), first at index 7
 *     XX SIZE  ref 876 bytes, ours 920
 * NOT a true distance by a wide margin: 435 encodings against 414, 44 bytes
 * over, and the --align figure EXCEEDS the reference length because our extra
 * instructions inflate the disagreeing regions.  Treat it as "opened and
 * transcribed", not as a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_b5000/80c1ffc.c \
 *     asm/rom_b5000/rom_c1a34_a_a_a_c.s --align
 *
 * SHIMS: ZERO in both reported classes.  The two `__asm__` uses are asm-name
 * bindings for the `.Lc5c38` / `.Lc5c48` record tables, both already `.global`
 * in asm/rom_b5000/rom_c1a34_c.s and both already used this way by the landed
 * siblings src/rom_b5000/rom_c1a34_a_a_a_a_a.c and rom_c1a34_a_a_a_b.c.
 * No .sym entry is wanted: 0x173, 0x17c and 0x7fff are all too wide for an
 * 8-bit `mov`, so their pool words are not a tell.
 *
 * ============== THE STRUCTURE, FULLY DECODED -- THIS IS THE DELIVERABLE ==============
 * The encounter builder.  It picks an enemy-group record, spends a budget of 6
 * "points" across up to five enemy slots, expands the result into a u16 id list
 * on the frame, and instantiates units 0x80..0x85 from it.
 *
 * struct Rec IS ALREADY ESTABLISHED by the landed family member
 * src/rom_b5000/rom_c1a34_a_a_a_a_a.c and this function confirms every field:
 *     struct Rec { u8 f0; u8 ids[5]; u8 f6[5]; u8 flags[5]; };   /* 16 bytes * /
 * The ROM's `mov r4, r3 / add r4, #2 / ldrb r3, [r4, #4]` reads e->f6[0] and
 * `add r5, r4, #4` makes &e->f6[0]; [sp+8] is e->ids (e+1) and r9 is e->flags
 * (e+0xb).  The `e + 2` base is not explained by any field and is the one piece
 * of the address arithmetic I could not reproduce -- see WHAT IS LEFT.
 *
 * THE FRAME, 0x7c, read slot by slot:
 *     [sp+0x00], [sp+0x04]   spill slots for r4 and r1 around calls, NOT
 *                            arguments -- every call in this function takes at
 *                            most one.  r4 is caller-saved because
 *                            GCC296_CFLAGS carries -fcall-used-r4, which is the
 *                            same reading src/non_matching/ovl_7e3e08/2008f94.c
 *                            records for its own `str r4, [sp]` pairs.
 *     [sp+0x08]  e->ids      [sp+0x0c]  &arrA
 *     [sp+0x10]  k, the output index -- a MEMORY variable, re-read and rewritten
 *                inside every emit loop
 *     [sp+0x14]  e           [sp+0x18]  the field-state pointer (*iwram_3001e74)
 *     [sp+0x1c]  &out        [sp+0x20]  the `flag` out-parameter for Func_80c1afc
 *     [sp+0x24]  int perm[5]     [sp+0x38]  int arrA[5]
 *     [sp+0x4c]  int arrB[5]     [sp+0x60]  u16 out[14]
 *
 * PHASES:
 *  1. state[0x40] = 0; flag = 0; if (_GetFlag(0x173)) id = Func_80c1afc(&flag);
 *     if (id >= 0xbe<<1) id = 1; e = &Lc5c38[id].
 *  2. scan e->f6[0..4] for the first non-zero; if all five are zero fall back to
 *     the single record at .Lc5c48.  The ROM tests e->f6[0] SEPARATELY and then
 *     runs a `do { i++; if (i > 4) break; p++; } while (*p == 0)` over a running
 *     pointer -- an entry-test rotation, i.e. `jump.c:1137
 *     duplicate_loop_exit_test`, and the sibling's indexed
 *     `while (i <= 4 && e->f6[i] == 0)` is NOT the same shape.  The counter is
 *     SIGNED here (`cmp r1, #4 / bgt`) while every later loop counter is
 *     unsigned (`bls`).
 *  3. budget = 6.  For i in 0..4, if e->f6[i] then
 *     budget -= cost(i) * e->f6[i], where
 *         cost(i) = 2 - (Func_80c23c0(e->ids[i] + 8) != 0)
 *     and the ROM emits that as `neg r3, r0 / orr r3, r0 / mov r2, #2 /
 *     lsr r3, #31 / sub r3, r2, r3`, which is what `2 - (x != 0)` produces.
 *     `e->f6[i]` is RE-READ after the call, not cached -- the do-not-cache rule
 *     the sibling also records.
 *  4. For i in 0..4: arrA[i] = e->f6[i]; span = e->flags[i] - e->f6[i]; if
 *     span > 0 then arrB[i] = ((min(span, budget / cost(i)) + 1) * Random()) >> 16
 *     else arrB[i] = 0.  The division is a real `__divsi3` (rom_b5000 is not an
 *     overlay, so no `_divsi3_RAM` alias question arises).
 *  5. `do { changed = 0; for i in 0..4 if (arrB[i]) { c = cost(i);
 *     if (c > budget) arrB[i] = 0; else { arrA[i]++; arrB[i]--; budget -= c;
 *     changed = 1; } } } while (changed);`  Note the ROM holds TWO pointers to
 *     arrB -- r6 for the read and r1 for the decrement -- exactly the
 *     two-pointers-to-one-array shape brief H's target 1 also shows.
 *  6. state[0x42] = e->f0, then RE-READ state[0x42] and dispatch three ways:
 *       0  -> shuffle: perm[i] = i for 0..4, then TEN random swaps with both
 *             `Random() * 5 >> 16` indices computed BEFORE the swap, then emit
 *             arrA[perm[i]] copies of e->ids[perm[i]] + 8 in shuffled order.
 *       1  -> round-robin: repeatedly collect the indices with arrA[j] != 0 into
 *             perm contiguously (`stmia r2!, {r7}` advances only when it stores),
 *             pick one with `Random() * m >> 16`, emit it, decrement arrA[j],
 *             until none are left.
 *       else-> plain: emit arrA[i] copies of e->ids[i] + 8 for i in 0..4, as a
 *             `while` with the test at the bottom and an entry jump.
 *  7. out[k] = 0 -- and the ROM writes `ldr r3, =0` for it, which is the HImode
 *     literal rule in its "bare literal pools" direction, NOT a symbol.
 *     state[0x3c] = 6; state[0x3e] = 0.
 *  8. `for (u = 0x80; u <= 0x85; u++) Func_80008d4(_GetUnit(u), 0xa6 << 1);`
 *     through a pointer (`bl _call_via_r5`), with r0 flowing straight from
 *     _GetUnit into the callee's first argument.
 *  9. for each non-zero out[i], i <= 5: n = Func_80c1df4(out[i], 1); if
 *     (n & 0x8000) Func_80c1f50(out[i]); _InitEnemyUnit(0x80 + i, out[i],
 *     n & 0x7fff); _GetUnit(0x80 + i) FOR EFFECT, result discarded; if (flag)
 *     Func_80c1c54(0x80 + i).  Returns i.
 *
 * ============== WHAT WAS MEASURED ==============
 *   spelling                                                       --align
 *   -------------------------------------------------------------- -------
 *   first candidate                                                     495
 *   locals declared in reverse order (out, arrB, arrA, perm)            495 (inert)
 *   a SEPARATE counter for the phase-2 scan loop (KEPT)                 459
 *   `unsigned char *` for the e->flags walk instead of an int cast      453
 *
 * The separate-counter result is the useful one and it is the same mechanism as
 * brief H's target 1: sharing one `i` across the phase-2 scan and the three
 * later loops makes one long-lived quantity, and it lands in r8, where every
 * increment costs `mov r3, #1 / add r8, r3 / mov r0, r8` instead of `add r1, #1`.
 * That is the signature the brief describes ("separate counters took low
 * registers where the ROM keeps one in r8") running in the opposite direction --
 * here it is OUR build that pays the high register.
 *
 * ============== WHAT IS LEFT, AND HOW FAR ==============
 * 21 instructions over and 44 bytes over.  Three identified items, none of them
 * a wall, all of them work:
 *   (a) THE FRAME IS 0x80 AND THE ROM'S IS 0x7c.  The array block is right
 *       (28 + 60 = 0x58 bytes) but gcc allocates TEN spill words and uses only
 *       NINE, leaving [sp+0x00] dead.  Brief H's target 1 shows the identical
 *       symptom -- a reserved-and-unused word at offset 0 -- so this is likely
 *       one phenomenon, worth chasing once for both.
 *   (b) THE `e + 2` BASE.  The ROM builds a pointer two bytes into the record and
 *       reads e->f6[0] as `[r4, #4]` and &e->f6[0] as `add r4, #4`; no field of
 *       struct Rec sits at +2 and no spelling I tried produced it.  Ours reads
 *       `ldrb r3, [r3, #0x6]` off e directly, which is shorter, so this costs
 *       nothing yet -- but it means the phase-2 address arithmetic is not the
 *       ROM's and everything downstream of it is suspect.
 *   (c) THE BYTE-OFFSET INDUCTION VARIABLES.  The ROM walks arrA and arrB with a
 *       separate byte counter (r8 stepping by 4, used as `ldr r3, [r5, r6]`) and
 *       keeps BOTH a `k` in memory and a `k*2` in a register through the emit
 *       loops.  Ours indexes with `a[i]` and pays a `lsl` per access.  That is
 *       loop strength reduction choosing differently, and it is where a good
 *       fraction of the 21 extra instructions live.
 *
 * HONEST ASSESSMENT: the furthest out of brief H's three.  406 instructions,
 * eleven frame slots, all four of r8-r11 in use, three nested loops, two
 * `Random()`-driven index computations and a division -- it fails four of the
 * five conditions in docs/elevation.md's own selection filter.  It should not be
 * given another four-target slot.  What it should get, if anything, is a deep
 * dive on (c) alone, with the `.08.loop` dump, because the induction variables
 * are the only part of the residue that is systematic rather than incidental.
 */
#include "gba/types.h"

struct Rec {
    unsigned char f0;
    unsigned char ids[5];
    unsigned char f6[5];
    unsigned char flags[5];
};

extern struct Rec Lc5c38[] __asm__(".Lc5c38");
extern struct Rec Lc5c48 __asm__(".Lc5c48");
extern unsigned char *iwram_3001e74;

extern int _GetFlag(int flag);
extern int Func_80c1afc(int *out);
extern int Func_80c23c0(int id);
extern int Random(void);
extern void Func_80008d4(void *dst, s32 len);
extern u8 *_GetUnit(int slot);
extern void _InitEnemyUnit(int slot, int id, int flags);
extern int Func_80c1df4(int id, int a);
extern void Func_80c1f50(int id);
extern void Func_80c1c54(int slot);

int Func_80c1ffc(unsigned int id)
{
    int perm[5];
    int arrA[5];
    int arrB[5];
    unsigned short out[14];

    unsigned char *st;
    struct Rec *e;
    unsigned char *ids;
    unsigned char *p;
    unsigned char *pf;
    int *a;
    int *b;
    int *c;
    int *q;
    unsigned short *o;
    void (*fp)(void *, s32);
    int flag;
    int budget;
    int i;
    int j;
    int m;
    int k;
    int n;
    int cost;
    int span;
    int cnt;
    int changed;
    int t;
    int u;
    int x;
    int y;

    st = iwram_3001e74;
    o = out;
    flag = 0;
    st[0x40] = 0;
    if (_GetFlag(0x173) != 0)
        id = Func_80c1afc(&flag);
    if (id >= (unsigned int)(0xbe << 1))
        id = 1;
    e = &Lc5c38[id];
    m = 0;
    if (e->f6[0] == 0) {
        p = e->f6;
        do {
            m++;
            if (m > 4)
                break;
            p++;
        } while (*p == 0);
    }
    if (m == 5)
        e = &Lc5c48;
    ids = e->ids;
    k = 0;
    budget = 6;
    p = e->f6;
    for (i = 0; (unsigned int)i <= 4; i++) {
        if (*p != 0) {
            n = Func_80c23c0(ids[i] + 8);
            cost = 2 - (n != 0);
            budget -= cost * *p;
        }
        p++;
    }
    a = arrA;
    b = arrB;
    p = e->f6;
    pf = e->flags;
    for (i = 0; (unsigned int)i <= 4; i++) {
        t = *p;
        p++;
        n = *pf;
        pf++;
        a[i] = t;
        span = n - t;
        if (span > 0) {
            n = Func_80c23c0(ids[i] + 8);
            cost = 2 - (n != 0);
            n = budget / cost;
            if (n < span)
                span = n;
            b[i] = ((span + 1) * Random()) >> 16;
        } else {
            b[i] = 0;
        }
    }
    do {
        changed = 0;
        for (i = 0; (unsigned int)i <= 4; i++) {
            n = ids[i] + 8;
            if (b[i] != 0) {
                cost = 2 - (Func_80c23c0(n) != 0);
                if (cost > budget) {
                    b[i] = 0;
                } else {
                    a[i] += 1;
                    b[i] -= 1;
                    budget -= cost;
                    changed = 1;
                }
            }
        }
    } while (changed != 0);
    st[0x42] = e->f0;
    t = st[0x42];
    if (t == 0) {
        c = perm;
        i = 0;
        do {
            *c++ = i;
            i++;
        } while ((unsigned int)i <= 4);
        i = 0;
        do {
            x = (Random() * 5) >> 16;
            y = (Random() * 5) >> 16;
            n = perm[x];
            m = perm[y];
            i++;
            perm[x] = m;
            perm[y] = n;
        } while ((unsigned int)i <= 9);
        c = perm;
        for (i = 0; (unsigned int)i <= 4; i++) {
            j = *c;
            cnt = a[j];
            if (cnt > 0) {
                t = ids[j] + 8;
                n = a[j];
                do {
                    out[k] = t;
                    k++;
                    n--;
                } while (n != 0);
            }
            c++;
        }
    } else if (t == 1) {
        o = out + k;
        for (;;) {
            m = 0;
            q = a;
            c = perm;
            for (i = 0; (unsigned int)i <= 4; i++) {
                if (*q++ != 0) {
                    *c++ = i;
                    m++;
                }
            }
            if (m == 0)
                break;
            j = perm[(m * Random()) >> 16];
            *o = ids[j] + 8;
            k++;
            o++;
            a[j] -= 1;
        }
    } else {
        i = 0;
        while ((unsigned int)i <= 4) {
            cnt = a[i];
            if (cnt > 0) {
                t = ids[i] + 8;
                do {
                    out[k] = t;
                    k++;
                    cnt--;
                } while (cnt != 0);
            }
            i++;
        }
    }
    out[k] = 0;
    *(unsigned short *)(st + 0x3c) = 6;
    *(unsigned short *)(st + 0x3e) = 0;
    fp = Func_80008d4;
    u = 0x80;
    do {
        fp(_GetUnit(u), 0xa6 << 1);
        u++;
    } while ((unsigned int)u <= 0x85);
    i = 0;
    if (out[0] != 0) {
        do {
            n = Func_80c1df4(out[i], 1);
            if ((n & (0x80 << 8)) != 0)
                Func_80c1f50(out[i]);
            u = i + 0x80;
            _InitEnemyUnit(u, out[i], n & 0x7fff);
            _GetUnit(u);
            if (flag != 0)
                Func_80c1c54(u);
            i++;
            if (i > 5)
                break;
        } while (out[i] != 0);
    }
    return i;
}
