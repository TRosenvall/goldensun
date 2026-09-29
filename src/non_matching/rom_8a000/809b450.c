/* Func_809b450 (0x0809b450) -- NON-MATCHING, 2 encodings of 145 differ.
 * Blocker class: sched2 tie.  UNREACHABLE FROM SOURCE -- see PROOF below.
 * Fresh in batch 287; mechanism closed in batch 295.
 *
 * asm/rom_8a000/rom_9ad70_c_a_c_c_c.s holds this function alone (pools only),
 * so it would be a whole-file conversion, no split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809b450.c asm/rom_8a000/rom_9ad70_c_a_c_c_c.s --func Func_809b450
 *   -> "ENCODINGS differ in 2 place(s) (ref 145, ours 145)", first at index 85.
 *
 * THE RESIDUE: ROM `mov r1,#0x21 / neg r1,r1 / strh r3,[r5,#8]`, ours
 * `mov r1,#0x21 / strh r3,[r5,#8] / neg r1,r1`.  Same size, same 145
 * instructions, same relocations, one adjacent pair swapped.
 *
 * PROOF THAT NO SOURCE SPELLING REACHES IT.  From the -dR -fsched-verbose=6
 * dump of basic block 3 (the loop body), the two candidates at the tie are
 * insn 234 = `(set (mem:HI (plus (reg r5) 8)) (reg:HI r3))` (the f08 bitfield
 * store) and insn 478 = `(set (reg r1) (neg (reg r1)))`, the second half of the
 * reload-rematerialised -0x21 mask.  rank_for_schedule (haifa-sched.c:4029) has
 * exactly five tests after priority, and ALL FIVE ARE TIED OR SKIPPED:
 *
 *   1. INSN_PRIORITY   12 == 12.  And the equality is STRUCTURAL, not luck:
 *        prio(234) = 0 + prio(237) = 0 + (2 + prio(242))
 *        prio(478) = 1 + prio(239) = 1 + (1 + prio(242))
 *      237 is the `ldrb r3,[r5,#5]` that starts byte 5's read-modify-write and
 *      234's dependence on it is an ANTI dep on r3, which insn_cost charges 0;
 *      the ldrb's own latency is 2.  On the other side the neg's latency is 1
 *      and it reaches the same `and` (242) through one copy insn 239
 *      (`mov r2,r1`) of latency 1.  0+2 == 1+1.  Both paths bottom out on the
 *      SAME insn 242, so any change that lengthens or shortens the byte-5 chain
 *      moves both priorities together.
 *   2. INSN_REG_WEIGHT -- gated on `!reload_completed`, so sched2 never runs it.
 *   3. the interblock tests -- same bb.
 *   4. the last_scheduled_insn class test.  last_scheduled_insn at the tie is
 *      insn 477 (`mov r1,#0x21`); 478 is data-dependent on it with cost 1, and
 *      the code treats cost==1 as class 3, the same class 234 gets for having no
 *      dependence at all.  3 == 3.
 *   5. depend_count.  234 -> {285, 237} = 2.  478 -> {467, 239} = 2.
 *
 * so it falls to `INSN_LUID (tmp) - INSN_LUID (tmp2)` and the strh wins because
 * the mask is rematerialised at its USE, which is inside byte 5's RMW, which is
 * after the f08 statement.  Getting LUID(478) < LUID(234) requires the byte-5
 * RMW to start before the f08 store, and that is the statement reorder measured
 * at 68 and worse.  THE FUNCTION SHOULD COME OFF THE REACHABLE LIST.
 *
 * LEVERS THAT GOT IT HERE (37 -> 30 -> 2):
 *  - `*((u8 *)s + 0x1d) |= 1` (or a 1-bit bitfield), NOT `s->f1d |= 1`: the
 *    QImode struct-member OR leaves an `ior ... (subreg (reg:QI 0))` that CSE
 *    feeds from the a->f55 store's QI zero; combine then kills the use but
 *    leaves the QI zero SET (REG_UNUSED) alive through local-alloc, which puts
 *    it in r3 and pushes the `a+0x55` address pseudo off r3. With it gone, the
 *    two plain `a->f55 = 0; a->f64 = 0;` stores get the same register and
 *    postreload move2add gives the ROM's `add r3,#0xf` from the 0x55 address.
 *  - the pooled zero for `s[0x26]` / `f28[0x16]` is the struct HalfWord case.
 *
 * INERT OR WORSE (all measured): all 120 orders of the five tail statements
 * (best = source order, 2); all 40 placements of `z.v = 0` / `s = a->f50`
 * (37 in the old base); the f08 value as temp / `/ 32` / SpriteSlot pointer /
 * a `tile:10` bitfield in SpriteSlot (2 each); f08 through a separate
 * `struct SprBits` or a char union (2); byte 5/7 through separate B5/B7
 * structs (92); `m = -0x21` byte-AND spellings before or after f08 (21);
 * `s->c6 = 1` before `s->c5 = 0` (12); `do{}while(0)` / `asm("")` between the
 * statements (size changes); an explicit `q = a + 0x55; ...; q += 0xf` pointer
 * with an int zero (30: the pointer pseudo takes r1 and the pool zero r3).
 *
 * ADDED IN BATCH 295 (delta, all measured against this file's 2):
 *  - THE COUPLED PAIR the park's own list invited -- "f08 value as temp" (inert
 *    at 2 alone, confirmed at four different declaration placements AND as a
 *    block-scoped `{ int v = ...; s->f08 = v; }`) TOGETHER WITH the c5 statement
 *    moved between the temp and the store: 68, `first at index 32`, relocations
 *    reordered.  With c5 AND c6 moved in: 110, first at index 7.  With c5, c6
 *    and e6 moved in: 136.  With the store after `f28[0x16]`: 120 and the
 *    instruction count drops to 143.  `unsigned short` temp, `vramOffset` named
 *    before the shift, e6 hoisted above c5: 68, 68, 67.  The pair does not pay
 *    here, and the reason is test 1 above: the reorder changes which chain is
 *    longer rather than breaking the tie.
 *  - `__asm__ ("" : "+r" (s))` barriers: before the f08 statement 4, between f08
 *    and c5 6, after c5 67 (size 312/320).  A `"+r"` on the f08 value itself 71.
 *    The 4-scoring one keeps the 85/86 residue exactly and ADDS a swap at 50/51,
 *    so the barrier does not reach the tie at all.
 *  - EVERY FLAG GROUP AND EVERY PAIR: the same 17 flags as the sibling park
 *    (see src/rom_8a000/rom_8ba38_a_c_a_a_b.c (landed in batch 295)), alone and in all 136 pairs.
 *    Best is 2; nothing reaches 0.  -fno-schedule-insns2 is WORSE, which is the
 *    positive control that sched2 ran on the ROM too.
 *
 * SHIMS: none.  No `register ... __asm__` declarations and no
 * `__asm__(".equ ...")` lines.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Sprite {
    unsigned char pad00[5];
    unsigned char c0 : 5;
    unsigned char c5 : 1;
    unsigned char c6 : 2;
    unsigned char pad06;
    unsigned char e0 : 6;
    unsigned char e6 : 2;
    unsigned short f08 : 10;
    unsigned short b10 : 2;
    unsigned short b12 : 4;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[0x26 - 0x1e];
    unsigned char f26;
    unsigned char f27;
    unsigned char *f28;
};

struct Sprite9 {
    unsigned char pad00[9];
    unsigned char lo : 2;
    unsigned char prio : 2;
    unsigned char hi : 4;
};

struct Actor {
    void *f00;
    unsigned char pad04[4];
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[0x50 - 0x20];
    struct Sprite *f50;
    unsigned char pad54[0x55 - 0x54];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[0x68 - 0x66];
    struct Actor *f68;
    void (*f6c)(void);
};

struct HalfWord { unsigned short v; };

extern unsigned char *iwram_3001f30;
extern struct SpriteSlot gSpriteSlots[];
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Sprite_SetAnim(struct Sprite *s, int n);
extern void Func_8003f3c(int n);
extern void Func_809b3d8(void);
extern void Func_809b364(void);

void Func_809b450(struct Actor *e)
{
    unsigned char *g;
    struct Actor *leader;
    struct Actor *arr[2];
    struct Actor *a;
    struct Sprite *s;
    struct HalfWord z;
    int i;

    g = iwram_3001f30;
    leader = *(struct Actor **)(g + 0x10);
    for (i = 0; i <= 1; i++) {
        a = _CreateActor(0x1a, e->f08, e->f0c + (0x80 << 15), e->f10);
        arr[i] = a;
        if (a == 0)
            continue;
        a->f14 = e->f14;
        s = a->f50;
        a->f55 = 0;
        a->f64 = 0;
        z.v = 0;
        a->f68 = e;
        a->f1c = 0x6666;
        a->f18 = 0x6666;
        if (s == 0)
            continue;
        _Sprite_SetAnim(s, 0);
        s->f26 = z.v;
        Func_8003f3c(s->f1c);
        s->f1c = *(unsigned short *)(g + 0x71a);
        *((unsigned char *)s + 0x1d) |= 1;
        s->f08 = gSpriteSlots[s->f1c].vramOffset >> 5;
        s->c5 = 0;
        s->c6 = 1;
        s->e6 = 2;
        s->f28[0x16] = z.v;
    }
    arr[0]->f6c = Func_809b3d8;
    ((struct Sprite9 *)arr[0]->f50)->prio = 0;
    arr[1]->f6c = Func_809b364;
    ((struct Sprite9 *)arr[1]->f50)->prio = ((struct Sprite9 *)leader->f50)->prio;
}
