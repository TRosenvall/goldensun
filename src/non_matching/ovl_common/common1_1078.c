/* OvlFunc_common1_1078 -- NON-MATCHING, 7 encodings of 217.  217 instructions against 217,
 * 476 bytes against 476, ZERO relocation differences.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common1_1078.c \
 *     asm/overlays/common/common1_a_a_a_a_c_c_a_c.s --func OvlFunc_common1_1078
 *
 * Its .s also holds OvlFunc_common1_fac (84 instructions, unattempted), so it would not
 * convert whole even at zero.
 *
 * TWO GENERALISING FINDS, BOTH SINGLE DROPS:
 *
 * `int mask = -13;` AS A SHARED NAMED LOCAL, NOT THE LITERAL -- 135 differing.
 * `p[5] & -13` on an `unsigned char` lets gcc fold to a QImode `mov r2,#0xf3`, ONE
 * instruction; the ROM has `mov r2,#0xd / neg r2,r2` (SImode -13) and REUSES that register
 * for the second field.  **A `mov`+`neg` PAIR WHERE A BYTE MASK WOULD FIT IN ONE `mov` IS
 * THE TELL FOR A NAMED `int` MASK** -- and without it the function comes out two
 * instructions SHORT, so the count disagreeing is itself the signal.
 *
 * THE QUEUE POINTER IS THE LAST PARAMETER OF THE INLINE DMA HELPER -- 15 -> 7.  With it
 * first (the landed rom_7795e8 spelling) the third push puts the queue in r0 and REG_IME in
 * r1; the ROM has them swapped, in a block whose instruction ORDER was already right.
 * Writing that push out by hand reaches the same 7, so it is ALLOCATION, not inlining.
 *
 * BLOCKER: THREE ADJACENT TRANSPOSITIONS, ALL ONE SHAPE -- this candidate's cheap producer
 * (`mov r7,r0`; `ldr r2,=.L49`; `mov r2,#13`) issued one slot EARLIER than the ROM's.  Ten
 * statement-order and operand-order spellings moved none of them, and the reason is worth
 * carrying: THEY MOVE INSNS AND NEVER TOUCH A REFERENCE COUNT.
 *
 * BATCH 286 SCHED2 READOUT of the first two (-da -fsched-verbose=5, .23.sched2,
 * insn numbers from that dump; body unchanged, still 7 of 217):
 *   t=41, ready {41 strb [r6,#6], 24 mov r7,r0}: BOTH prio 68, both class 3
 *     (41's dep on the just-issued `mov r3,#1` costs 1), and 24 wins on the
 *     dependent-count step, 11 against 4.  p(24) = 1 + p(ldr r3,[r7,#8]) and
 *     p(mov r3,#4) = 1 + p(strb [r6,#7]) = 1 + p(ldr r3,[r7,#8]) through the r3
 *     ANTI edge, so the two are tied STRUCTURALLY by the reuse of r3.
 *   t=47, ready {63 ldr r3,[r7,#8], 61 ldr r2,=.L49}: both prio 67; 63 is ANTI-
 *     dependent on the just-issued strb [r6,#7] and anti costs 0 in
 *     arm_adjust_cost, so insn_cost != 1 puts it in CLASS 2 against 61's 3.
 *   So the ROM needs a real priority edge, not a tie flip.  Measured this batch:
 *   `struct Ent ent;` appended to struct Actor (alias-subset, hoping to turn
 *   the byte stores into true producers for the act loads) INERT at 7; the f6
 *   and/or f7 store through `(unsigned char *)e` 480 bytes / 178 (the byte
 *   constant CSEs with the later `|= 1` / `| 4`); `__asm__ volatile("")`
 *   barriers splitting the block with the call result copied after them
 *   (act = a) 22-33, because the extra pseudos move the allocation.
 */
/* PARK -- OvlFunc_common1_1078, asm/overlays/common/common1_a_a_a_a_c_c_a_c.s.
 *
 * 7 of 217 encodings, 476 bytes against 476, 217 instructions against 217, every
 * relocation identical.  The .s also holds OvlFunc_common1_fac (84 instructions,
 * unattempted), so this .s does NOT convert whole even when this is finished.
 *
 * The three DMA pushes are the ALREADY LANDED idiom from
 * src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c (struct DmaQueue / LOCK_IME /
 * SetRegAnimDest), and the struct layouts come from the INVERSE function in the
 * same overlay, src/overlays/common/common1_a_a_a_a_c_c_b.c -- that one READS
 * .L49/.L20/.L31 into the actor, this one writes the actor's fields out to them.
 *
 * LOAD-BEARING, each a single drop against this file:
 *
 *   `int mask = -13;` A SHARED NAMED LOCAL, not the literal   135 differing
 *     This is the whole difference between 134 and 15.  `p[5] & -13` on an
 *     unsigned char lets gcc fold the mask to a QImode `mov r2, #0xf3`, ONE
 *     instruction; the ROM has `mov r2, #0xd / neg r2, r2`, which is the SImode
 *     -13, and it REUSES that register for the second field.  So the tell for a
 *     named int mask is a `mov`+`neg` pair where a byte mask would fit in one
 *     `mov` -- and the function is two instructions SHORT without it.
 *
 *   THE QUEUE POINTER IS THE *LAST* PARAMETER of both inline helpers  15 -> 7
 *     With it first (the landed rom_7795e8 spelling) the third push puts the queue
 *     in r0 and REG_IME in r1; the ROM has them the other way round, and moving
 *     the parameter to the end is what swaps them.  Eight encodings, in a block
 *     whose instruction ORDER was already right.  Writing that third push out by
 *     hand instead of calling the helper reaches the same 7, so this is the
 *     allocation, not the inlining.
 *
 *   `(u32)queue + count * 12 + 4` in the LOOP helper                     8 -> 7
 *     The landed helper writes `count * 12 + (u32)queue + 4` and that gives
 *     `add r1, r1, r4` where the ROM has `add r1, r4, r1` -- a different Thumb
 *     encoding for the same add.  The other two pushes want the landed order.
 *
 * MEASURED INERT, so not claimed: a named `four` for the OR constant, a named `m`
 * for 0x80 << 7, swapping `e->f6`/`e->f7`, moving `p = act->f50` earlier, the
 * declaration order inside the helpers, `mask & p[5]` instead of `p[5] & mask`
 * (both AND sites, separately and together), `extern int L49[]` with `L49[0] =`,
 * and hoisting `count = queue->count` out of LOCK_IME (that one is 23, harmful).
 *
 * THE RESIDUE IS THREE ADJACENT TRANSPOSITIONS, ALL THE SAME SHAPE: in each case
 * OUR cheap producer is issued ONE SLOT EARLIER than the ROM's.
 *
 *     rom  mov r3,#1 / strb r3,[r6,#6] / mov r3,#4 / mov r7,r0 / strb r3,[r6,#7]
 *     ours mov r3,#1 / mov r7,r0 / strb r3,[r6,#6] / mov r3,#4 / strb r3,[r6,#7]
 *
 *     rom  ldr r3,[r7,#8]  / ldr r2,=.L49 / str r3,[r2]
 *     ours ldr r2,=.L49    / ldr r3,[r7,#8] / str r3,[r2]
 *
 *     rom  mov r0,r9 / mov r2,#13 / ldrb r1,[r0,#5] / neg r2,r2
 *     ours mov r2,#13 / mov r0,r9 / ldrb r1,[r0,#5] / neg r2,r2
 *
 * All three are sched2 priority ties between a `mov`/pool-load and a memory load
 * that the same insn consumes, and ten statement-order and operand-order
 * spellings moved none of them -- consistent with docs/elevation.md's "When the
 * PRIORITY FORMULA decides, statement order is the wrong knob": these spellings
 * move INSNS and never touch a REFERENCE COUNT.
 */
#include "gba/types.h"
#include "gba/io.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char iwram_3001e68;

extern int L49 __asm__(".L49");
extern int L20 __asm__(".L20");
extern int L31 __asm__(".L31");

struct Ent {
    unsigned char pad0[6];
    unsigned char f6;
    unsigned char f7;
};

struct Actor {
    unsigned char pad0[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    unsigned char *f50;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int slot, int a);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_809280c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int anim);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void SetRegAnimDest(u32 dest, u32 src, struct DmaQueue *queue)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = src;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void SetBldAlphaStep(int t, struct DmaQueue *queue)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)((u32)queue + count * 12 + 4);
        *(u16 *)queue = count + 1;
        *task++ = ((0xf - t) << 8) | (t + 1);
        *task++ = REG_ADDR_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void OvlFunc_common1_1078(int slot, int x, int y)
{
    struct Ent *e;
    struct Actor *act;
    unsigned char *p;
    int i;

    e = *(struct Ent **)&iwram_3001e68;
    act = __MapActor_GetActor(slot);
    e->f6 = 1;
    e->f7 = 4;
    L49 = act->f8;
    L20 = act->f10;
    p = act->f50;
    L31 = act->f6;
    __Func_8092b08(slot, 2);
    act->f23 |= 1;
    act->f6 = 0x80 << 7;
    __Actor_SetSpriteFlags(act, 3);
    __Actor_SetAnim(act, 0);
    __Actor_SetAnim(act, 1);
    __MapActor_SetPos(slot, x << 16, y << 16);
    __Func_809280c(0, 0x80 << 7, 0);
    SetRegAnimDest(REG_ADDR_BLDCNT, 0xf0 << 4, &gDMATaskCount);
    {
        int mask = -13;
        p[5] = (p[5] & mask) | 4;
        p[0x11] = (p[0x11] & mask) | 4;
    }
    __PlaySound(0xfc);
    for (i = 0; i <= 0xf; i += 2) {
        act->f18 = (i << 12) + (0x80 << 5);
        act->f1c = (0xf8 << 9) - (i << 12);
        SetBldAlphaStep(i, &gDMATaskCount);
        __WaitFrames(1);
    }
    SetRegAnimDest(REG_ADDR_BLDALPHA, 0x10, &gDMATaskCount);
    act->f18 = 0x88 << 9;
    act->f1c = 0xf0 << 8;
    __CutsceneWait(1);
    act->f18 = 0x80 << 9;
    act->f1c = 0x80 << 9;
    __CutsceneWait(0xd);
    {
        int mask = -13;
        p[5] = p[5] & mask;
        p[0x11] = p[0x11] & mask;
    }
    __MapActor_DoAnim(slot, 3);
    __CutsceneWait(0x14);
}
