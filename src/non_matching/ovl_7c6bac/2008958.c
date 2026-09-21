/* OvlFunc_942_2008958 (0x02008958) -- NON-MATCHING, ours 165 of ref 163, with
 * -fno-strict-aliasing. Blocker class: A FLAG CONFLICT -- the fix for one residue breaks another.
 *
 * asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c_c.s (1 function, so landing needs NO split).
 *
 * THE WHOLE gState PROLOGUE AND BOTH COMPARISONS ARE BYTE-EXACT: `ldrsh` + `cmp #3`, then
 * `ldrh` + `lsl #16` + `cmp 1 << 16`. Both tests are plain `short`, and the named pointer
 * `g = gState;` is what keeps the offset out of the relocation -- the "name the OFFSET, not the
 * BASE" idiom from the same-stem neighbour ovl_30_c_c_a_c_c_c_c_c_b.c, which also correctly
 * predicted that this area has a constant-CSE problem.
 *
 * REMAINING: constant CSE of `0x95 << 4` (which costs an r7 push), `0x8c << 17`, `0xc0 << 8`,
 * and a `movs r0,#N` argument-fill position.
 *
 * ===== AND THERE IS A FLAG CONFLICT, WHICH IS THE REASON THIS IS PARKED RATHER THAN CLOSE =====
 *
 * -fno-rerun-cse-after-loop FIXES ALL THE CONSTANT CSE -- probed and confirmed as the
 * responsible pass -- but it ALSO re-collapses the two `gState` loads into one and breaks the
 * matched comparison pair that is currently exact. The two halves cannot both be had from the
 * flag set.
 *
 * THE NAMED-LOCALS CONSTANT LEVER IS HARMFUL HERE: 163 -> 181. That is a second measured
 * negative against the three-named-locals lever this batch (the other is OvlFunc_909_20086e0's
 * 59), and both have the same shape -- the lever wants a DOMINATING BRANCH, and naming
 * constants whose uses are not behind one creates live pseudos that widen the push list.
 *
 * ALSO CONFIRMED, so it is not chased again: tryc MASKS `ldrh <pool-label>` against
 * `ldr <pool-label>`, AND IT IS RIGHT TO. resolve_pools rewrites both to `ldr rN, =value`, and
 * gas assembles `ldrh r1, .L11` to the identical `4904` halfword because Thumb-1 has no
 * PC-relative `ldrh`. Verified with objdump on both objects. This is the third time that fact
 * has come up (const.sym's _CONST_1f objection, the common1_1608 landing, and here) -- do not
 * spend candidates on it.
 *
 * NEXT: a source route to the constant CSE that does not need the flag.
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9;
    unsigned char pad_a[0x15 - 0xa];
    unsigned char f15;
    unsigned char pad16[0x1e - 0x16];
    unsigned short f1e;
    unsigned char pad20[0x26 - 0x20];
    unsigned char f26;
};

struct Actor {
    unsigned char pad0[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sub *f50;
    unsigned char pad54[0x59 - 0x54];
    unsigned char f59;
};

extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern struct Actor *__MapActor_GetActor(int slot);
extern void OvlFunc_942_2008af8(void);

void OvlFunc_942_2008958(void)
{
    unsigned char *g;
    struct Actor *a;
    struct Actor *b;

    OvlFunc_942_2008af8();
    if (__GetFlag(0x95 << 4))
        __Func_8092950(0xc, 2);
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        __ClearFlag(0x12f);
    if (*(short *)(g + (0xe1 << 1)) == 1)
        __ClearFlag(0x8aa);
    if (__GetFlag(0x8aa)) {
        __MapActor_SetPos(8, 0xcc << 17, 0x94 << 17);
        __Func_8092adc(8, 0x80 << 8, 0);
    }
    if (__GetFlag(0x8ab)) {
        __MapActor_SetPos(0xd, 0x8c << 17, 0x94 << 17);
        __Func_8092adc(0xd, 0xc0 << 8, 0);
        __MapActor_SetPos(0x10, 0x90 << 17, 0x8c << 17);
        __Func_8092adc(0x10, 0xe0 << 8, 0);
        __MapActor_SetPos(0xa, 0xe8 << 16, 0x98 << 17);
        __Func_8092adc(0xa, 0x80 << 7, 0);
        __MapActor_SetPos(0xb, 0xf0 << 16, 0x9c << 17);
        __Func_8092adc(0xb, 0xc0 << 8, 0);
        a = __MapActor_GetActor(0xa);
        a->f59 = 0;
        a->f23 = 2;
        a->f50->f9 |= 0xc;
        a->f50->f26 = 0;
        a->f50->f1e = 0xc0 << 8;
        b = __MapActor_GetActor(0xb);
        b->f23 = 0;
        b->f50->f9 |= 0xc;
        b->f50->f15 |= 0xc;
    }
    if (__GetFlag(0x95 << 4)) {
        __Func_8010704(0x12, 0x12, 1, 1, 0xe, 0x12);
        __Func_8010704(0x12, 0x12, 1, 1, 0xf, 0x12);
    }
}
