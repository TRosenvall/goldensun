/* OvlFunc_970_2008194 -- asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_c.s, the whole
 * TU (one function, and tools/datacheck.py reports no `.section .data`).
 *
 * The map's per-frame ambience hook: ramp the blend weight from a sine table,
 * sway the four party actors and the two scroll planes off a second sine, and on
 * every other frame spawn one drifting sparkle at a randomised offset.
 *
 * FIVE LEVERS, and the frame-layout one is the one that generalises.
 *
 * 1. THE BLEND WEIGHT ROUND-TRIPS THROUGH A `volatile unsigned short` LOCAL, and
 *    that local has to be DECLARED BEFORE the vec3.  The ROM writes the computed
 *    halfword to sp+0x12, reads it straight back with `ldrh`, and stores that to
 *    REG_BLDALPHA -- a store/reload pair no ordinary local produces:
 *      * a plain `unsigned short`, a `struct HalfWord` carrier, a one- or
 *        two-element `unsigned short` array, and a single frame struct holding
 *        both the vec3 and the halfword ALL get the load folded away by cse (the
 *        store and the load are both HImode at the same address), and one of them
 *        loses the frame slot entirely -- 34 to 216 differing;
 *      * `volatile` is what keeps the slot AND forces the reload, but only if the
 *        reload is a SEPARATE STATEMENT into an int (`t = hw;`): written
 *        `REG_BLDALPHA = hw;` gcc reuses the just-stored register and the `ldrh`
 *        never appears.
 *    **AND THE DECLARATION ORDER IS THE FRAME LAYOUT.** The frame is 20 bytes:
 *    reload slot at sp+0, vec3 at sp+4, halfword at sp+0x12.  gcc's stack slots
 *    run FIRST-ASSIGNED-HIGHEST here, and a `volatile` scalar gets its slot at
 *    `expand_decl` like an array does -- so declaring `hw` BEFORE `int v[3]` puts
 *    it at 0x12 and declaring it after puts it at 6.  A non-volatile scalar gets
 *    its slot later (at `put_var_into_stack`, when its address is taken) and
 *    lands at 6 whichever way it is declared, which is why decl order LOOKS inert
 *    until the `volatile` is there.
 *
 * 2. THE BARE `__asm__ volatile ("");` CLOSED THE LAST TWO ENCODINGS, exactly the
 *    use the sibling ovl_30_c_c_c_a_c_c_c_c_a_b.c records: a two-insn sched2
 *    transposition, here `ldrh r2,[r2]` against the `ldr r3` of REG_BLDALPHA's
 *    address.  `do { } while (0);` in the same slot is equally exact.
 *
 * 3. THE STORE BLOCK IS PINNED r2/r3, with the shift SPLIT OUT of the value
 *    expression.  Unpinned it is 8 differing: gcc computes the value with a
 *    three-operand `add r2, r3, r0` into the address's register and puts the
 *    address in r3, where the ROM has the destructive `add r3, r0` and the address
 *    in r2.  `u = call + 8;` then `u <<= 8;` INSIDE the pinned block is what puts
 *    `lsl r0, #8` between `mov r2, sp` and `add r2, #18` -- the same
 *    split-the-pair rule as an argument fill, applied to an address computation.
 *
 * 4. __CreateActor's THREE VEC READS WANT NAMES (p1/p2/p3).  Inline, the pooled
 *    actor id loads before the last vec read; named, r0 fills last as the ROM has
 *    it.  Worth 2, and neither an `int` return type nor a full PIN4 reached it.
 *
 * 5. THE POOLED ZERO NEEDED NO CARRIER, WHICH REFINES THE RECORDED RULE.  The ROM
 *    reaches `a->f55 = 0` with a POOL load (`ldr r1, .L35c` / `.word 0`), and
 *    src/non_matching/ovl_787e04/2008578.c's rule says a pooled zero reaching a
 *    `strb` is the `struct HalfWord` case because a QImode constant can never
 *    pool.  Here the BARE LITERAL 0 assigned to an `unsigned char` STRUCT FIELD
 *    already emits `ldrh r1, .L14` -- gcc picked the HImode pool path by itself,
 *    with the `struct HalfWord` carrier and the bare literal byte-identical.  So
 *    try the bare literal FIRST at a `strb` site; the carrier is only needed when
 *    it does not pool on its own.
 *
 * The `.call_via r3` helper is the tree's existing template, unchanged, with `f`
 * left to the allocator rather than pinned (the ROM has it in r3, which is what
 * gcc picks).  The clobber list keeps "memory","r12" and does NOT list lr -- `mov
 * r12, pc` puts the return address in r12, so lr survives.
 *
 * Read off the ROM: `sub r3, #0xf` derives -13 from the `2` just stored to f23, so
 * the bitfield write is the batch-71 two-bit field at offset 2 set to 2; and
 * `strh r7, [r3]` shares the `1` with the `iwram_3001e40 & 1` mask, so the mask
 * and the stored 1 must both be plain literals in the same block.
 *
 * VERIFIES: 556 bytes, 247 encodings and 27 relocations identical.
 */
#include "gba/types.h"
#include "gba/io.h"

struct Part {
    unsigned char pad00[9];
    unsigned char b0 : 1,
                  b1 : 1,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x14 - 0x10];
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Part *part;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned short f66;
    unsigned char pad68[0x6c - 0x68];
    void (*f6c)(void);
};

extern int L17ec __asm__(".L17ec");
extern int L17f0 __asm__(".L17f0");
extern int L17f8 __asm__(".L17f8");
extern int L17fc __asm__(".L17fc");
extern int L1800 __asm__(".L1800");
extern int L1804 __asm__(".L1804");
extern int L1808 __asm__(".L1808");
extern int L180c __asm__(".L180c");
extern int L1810 __asm__(".L1810");
extern int L1818 __asm__(".L1818");
extern int gOvl_020097e8;
extern int gOvl_02009814;
extern unsigned char *iwram_3001e70[];
extern unsigned int iwram_3001e40;

extern int Func_8000888(int a, int b);
extern int __sin(int a);
extern struct Actor *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_970_2008100(void);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}

void OvlFunc_970_2008194(void)
{
    unsigned char *base;
    struct Actor *a;
    volatile unsigned short hw;
    int v[3];
    int d;
    int x, y;
    int t;
    int u;

    base = iwram_3001e70[0];
    if (gOvl_020097e8 != 0) {
        u = call_via(Func_8000888, __sin(L17ec << 9), 3) + 8;
        {
            register int q2 __asm__("r2");
            register int q3 __asm__("r3");
            q3 = L17f0;
            q2 = (int)&hw;
            u <<= 8;
            q3 += u;
            *(volatile unsigned short *)q2 = q3;
            q2 = *(volatile unsigned short *)q2;
            __asm__ volatile ("");
            REG_BLDALPHA = q2;
        }
        L17ec++;
    }
    if (L17fc != 0) {
        d = call_via(Func_8000888, __sin(L1800 << 9), 2) << 16;
        *(int *)(base + 0x140) = L1804 + d;
        *(int *)(base + 0x170) = L1808 + d;
        if (L180c != 0xffff0000) {
            a = __MapActor_GetActor(0);
            a->fc = L180c + d;
            a->f14 = L180c + d;
            a->f55 = 0;
        }
        if (L1810 != 0xffff0000) {
            a = __MapActor_GetActor(1);
            a->fc = L1810 + d;
            a->f14 = L180c + d;
            a->f55 = 0;
        }
        if (gOvl_02009814 != 0xffff0000) {
            a = __MapActor_GetActor(3);
            a->fc = gOvl_02009814 + d;
            a->f14 = L180c + d;
            a->f55 = 0;
        }
        if (L1818 != 0xffff0000) {
            a = __MapActor_GetActor(2);
            a->fc = L1818 + d;
            a->f14 = L180c + d;
            a->f55 = 0;
        }
        L1800++;
    }
    if (L17f8 != 0 && (iwram_3001e40 & 1) != 0) {
        x = *(int *)(base + 0xe4) & 0xffff0000;
        y = *(int *)(base + 0xe8) & 0xffff0000;
        x += __Random() * 240;
        v[0] = x;
        v[1] = 0;
        y += __Random() * 160;
        y += 0xf0 << 13;
        v[2] = y;
        {
            int p1 = v[0];
            int p2 = v[1];
            int p3 = v[2];
            a = __CreateActor(0x1f7, p1, p2, p3);
        }
        if (a != 0) {
            a->f6c = OvlFunc_970_2008100;
            a->f64 = 0x3c;
            a->f66 = 1;
            a->f55 = 0;
            a->f23 = 2;
            a->part->b2 = 2;
            __Actor_SetSpriteFlags(a, 0);
            __Actor_SetAnim(a, 0);
        }
    }
}
