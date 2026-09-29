/* Func_801b664 (PagePartyListForward) -- 0x0801b664,
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s
 *
 * NON-MATCHING: 2 encodings of 200 differ (objcmp).
 *
 * SIZE EXACT (428 bytes both).  INSTRUCTION COUNT EXACT (200 both).  So the 7
 * IS a true distance, not a saturated count.  All NINE relocations are the
 * ROM's nine call targets in the ROM's order, at the ROM's byte offsets.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801b664.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s --func Func_801b664
 *
 * NO SHIMS (tools/shimcount.py: 0), NO PINS, NO asm, no fakematch row needed.
 * tools/datacheck.py on the reference prints nothing and exits 0 -- the file
 * carries NO data section, so no split export is required.  But the file holds
 * FOUR functions (Func_801b664, Func_801b810, Func_801b9a8, Func_801b9ec), so
 * it cannot convert whole until the other three land, and tryc.py SKIPS its
 * size check on a multi-function reference -- objcmp was mandatory throughout.
 *
 * DIRECT SIBLINGS: Func_801b4ec (src/rom_15000/rom_1aeec_a_a_c_a_c_c_a_b.c) and
 * Func_801b5c0 (.../rom_1aeec_a_a_c_a_c_c_b.c), both EXACT, same five callees,
 * same tail.  Func_801b810 in this same file is the backward twin and is parked
 * at 161 of 190.  Its struct, its callee signatures and two of its levers were
 * reused here rather than re-derived.
 *
 * THE TWIN'S BLOCKER DOES NOT APPLY HERE, and that is why this one got close.
 * Func_801b810's park names "one allocno too many -- the candidate buys r8 and
 * the ROM does not".  Func_801b664's ROM prologue is
 * `push {r5,r6,r7,lr} / mov r7,r10 / mov r6,r8 / push {r6,r7}` -- it buys BOTH
 * r8 and r10 itself, so there is no register to win back.  Read a twin's park
 * for its levers, but re-derive its blocker: this one was absent.
 *
 * FOUR LEVERS LANDED, each measured.
 *
 * 1. DO NOT NAME p->f394 IN A LOCAL -- cse's PATH-FOLLOWING wants two loads.
 *    Worth 59 (195 -> 136).  The ROM loads p->f394 twice before any call: once
 *    in the entry block (r2) and again at the `k > 5` arm (r1).  The ELSE arm
 *    reuses the entry block's r2.  That asymmetry is `cse_end_of_basic_block`
 *    following the UNCONDITIONAL `b .L1b7b2` into the else arm -- so the else
 *    arm is on the entry block's cse path and the `bhi` target is not.  With
 *    `k = p->f394` named, one pseudo serves all three uses and the second load
 *    disappears.  Reading the member fresh at each of the three sites
 *    reproduces the ROM exactly.  The `> 5` test then needs
 *    `(unsigned int)p->f394` to keep `bhi`: a bare member read promotes to int
 *    and emits `bgt`.  This is the twin's lever 1 reached without its local.
 *
 * 2. THE TWO LOOP-INVARIANT LOADS ARE volatile, AND NOTHING ELSE REACHED THEM.
 *    Worth 145 (156 -> 11).  The ROM reloads p->f396 and p->f398 on EVERY
 *    iteration of the .L1b6d2 loop; gcc hoists both into the preheader because
 *    strict aliasing proves a store to `struct Node`'s field cannot alias a
 *    `struct Party` field.  `loop_invariant_p` (loop.c) walks loop_store_mems
 *    with `true_dependence`, and DIFFERENT_ALIAS_SETS_P lets the load float.
 *    `volatile unsigned short` on the two fields blocks the hoist and gives
 *    exactly one load per source read -- which is what the ROM has, in both the
 *    loop and the peeled tail.
 *
 *    THE DOCUMENTED UNION LEVER DOES NOT REACH THIS PASS.  Five forms of the
 *    "one-member union is a per-MEM alias escape" lever were measured and ALL
 *    FIVE were byte-identical to no change at all:
 *      embedded one-member union at 0x396            (layout broke, see below)
 *      ((union PW *)&p->f396)->v  pointer-cast form  156  (inert)
 *      packed embedded union at 0x396                156  (inert)
 *      4-short union at 0x394 covering f394/6/8      156  (inert)
 *      4-short union at Node 0x14 (the STORE side)   156  (inert)
 *    That entry is written up as a SCHEDULING device (arm_adjust_cost, a missing
 *    1-point priority edge) and it is correct as such -- but sched2 and
 *    loop-invariant motion consult alias sets through different code, and the
 *    union reaches only the former.  NEW, and worth recording: the union is not
 *    a general substitute for alias set 0 at a MEM; it does not block LICM.
 *
 *    AN EMBEDDED UNION AT AN ODD-HALFWORD OFFSET SILENTLY BREAKS THE LAYOUT.
 *    ARM's STRUCTURE_SIZE_BOUNDARY is 32, so a union/struct MEMBER is 4-aligned
 *    and a one-short union is padded to 4 bytes.  `union U16 { unsigned short
 *    v; }` at 0x396 pushed every later field +4 (f39e landed at 0x3a2, f3a2 at
 *    0x3a6) and READ 154 of 199 WITH SIZE MATCHING -- a BETTER number than the
 *    correct layout's 156.  The pool words are the tell: they carried 0x3a2 and
 *    0x3a6 where the ROM has 0x39e and 0x3a2.  Check pool CONSTANTS against the
 *    reference before believing an improvement; a union member is only safe at
 *    a 4-aligned offset (0x394 works, 0x396 does not).
 *
 *    -fno-strict-aliasing IS NOT THE BETTER ROUTE HERE.  The tree has a
 *    sanctioned ALIAS_CFLAGS group (Makefile:235) with 20+ per-file rows and
 *    this reference is not in it.  Measured at the instruction level, the flag
 *    without volatile leaves residue in BOTH the loop and the peeled tail;
 *    volatile leaves it only in the tail.  So volatile is strictly better AND
 *    stays on production flags.  Flagged for the owner: volatile on two window
 *    fields is a MODELLING claim, not just a codegen device, and wants a look.
 *
 * 3. THE THREE NODE STORES ARE IN THE SAME ORDER IN BOTH PLACES -- ty, step,
 *    f1a.  Worth 4 (11 -> 7).  The loop body and the peeled tail write the same
 *    three fields; writing the tail in the ROM's EMITTED tail order
 *    (ty, f1a, step) is 11, and writing both in one order is 7.  Measured:
 *      tail ty,step,f1a (ships)                              7
 *      tail ty,f1a,step                                     11
 *      tail step,ty,f1a                                     12
 *      tail f1a,ty,step                                     13
 *      tail with a local for p->f396                     13, 14
 *      tail stores after a do{}while(0) boundary              9
 *
 * 4. b - 1 ON A HALFWORD MEMBER, NOT VIA AN int LOCAL.  The twin's lever 2.
 *    Inert here as a COUNT (both spellings gave 136 at the time) but kept
 *    because the ROM's `ldr r1,=0xffff / add r3,r2,r1` is the HImode form and
 *    the int-local spelling is what emits `sub r3,#1`.
 *
 * ALSO MEASURED AND WORSE / INERT:
 *   naming a = p->f39c to force the ROM's f39c-then-f39e load order  -2 insns
 *   f396/f398 declared short instead of unsigned short               inert
 *   f1a declared unsigned short (the alias hypothesis)               inert
 *   volatile on Node.step (pins its order)             204 insns, +4, 154
 *   the wait loop written as an explicit if + do/while              inert
 *   a 4-short union on Node 0x14 on top of the volatile fix         inert (7)
 *
 * 3b. THE sched2 BARRIER GOES BETWEEN THE ty STORE AND THE f398 LOAD.
 *    Worth 5 (7 -> 2).  A bare `do { } while (0);` there -- and region 2 then
 *    ordered f1a BEFORE step -- is the whole remaining lever.  Measured, all at
 *    exact length (200 of 200):
 *      ty / barrier / f1a / step   (SHIPS)                   2
 *      ty / barrier / step / f1a                             8
 *      ty / step / barrier / f1a                             7
 *      ty / f1a / barrier / step                             9
 *      barrier at both positions                             9
 *      `__asm__ volatile ("")` in place of the do/while      2  (identical)
 *      a pointer local for &p->f398 hoisted above it      7, 9  (pools 0x398
 *                                                as `ldr` for `mov`/`lsl`)
 *      p->f398 read into a named local first             7, 9, 11
 *
 * BLOCKER: ONE ADJACENT TRANSPOSITION, 2 ENCODINGS.  PASS .19.sched2.
 * Every other one of the 200 encodings is byte-identical, size is exact and the
 * nine relocations are the ROM's nine in order.  The residue is a single swap:
 *
 *   rom  ldrh r2,[r3] / mov r1,#0xe6 / strh r2,[r5,#0x18] / lsl r1,#2 / ...
 *   ours ldrh r2,[r3] / strh r2,[r5,#0x18] / mov r1,#230  / lsl r1,#2 / ...
 *
 * The ROM spends the f396 load-latency slot on the FIRST instruction of the
 * f398 address build and issues the ty store after it.  The barrier that buys
 * the other 5 is also what forbids this: it ends region 1 at the ty store, so
 * `mov r1,#0xe6` is region 2's first instruction and cannot move up.
 *
 * WHY IT IS A GENUINE VICE, not an untried spelling.  Without the barrier the
 * whole tail is ONE region and the address build lands in the ROM's slot
 * correctly -- but then the (pool-load, step-store) pair issues early and
 * delays the ty store instead (7).  With the barrier the pair is pinned late
 * and the address build is pinned later (2).  The two are the same lever pulled
 * in opposite directions and the full cross was measured: three source orders
 * of the three stores x four barrier positions, plus the asm-barrier form.
 *
 * The mechanism is the PRIORITY of the f396 load.  In one region its chain runs
 * on through `lsl r2,#16 / asr / cmp / beq` (the wait-loop guard), so it
 * outranks the address build and issues first, giving the ROM's fill.  The
 * barrier truncates that chain to `ldrh -> strh ty`, and with the guard in the
 * next region the load no longer outranks anything.  So the open route is a
 * separation that pins the step store WITHOUT shortening the f396 load's chain
 * -- i.e. something that re-regions only the step store while leaving the wait
 * -loop guard in the f396 load's region.  Placing the barrier after the f1a
 * store does exactly that and reads 9, because the address build then floats
 * ABOVE the f396 load rather than into the slot after it.  Nothing in C
 * measured here separates those three effects.
 *
 * FLAGS: tree default GCC296_CFLAGS, production.  No per-file Makefile rule; the
 * landing path is built by the generic `asm/%.o: src/%.c` cross-dir rule, the
 * same flag group tryc.py and objcmp give a scratch path.  No figure above is
 * flag-conditional except the one line that says so.
 */
struct Node {
    unsigned char pad0[4];
    struct Node *next;                  /* 0x04 */
    unsigned char pad1[0xa - 8];
    unsigned short id;                  /* 0x0a */
    unsigned char pad2[0x10 - 0xc];
    short y;                            /* 0x10 */
    unsigned char pad3[2];
    short step;                         /* 0x14 */
    unsigned char pad4[2];
    short ty;                           /* 0x18 */
    short f1a;                          /* 0x1a */
};
struct Party {
    unsigned char pad0[8];
    unsigned short f8;                  /* 0x008 */
    unsigned short fa;                   /* 0x00a */
    unsigned char pad1[0x3c - 0xc];
    unsigned short f3c;                 /* 0x03c */
    unsigned short f3e;                 /* 0x03e */
    unsigned char pad2[0x348 - 0x40];
    struct Node *head;                  /* 0x348 */
    unsigned char pad3[0x354 - 0x34c];
    unsigned short xs[0x10];            /* 0x354 */
    unsigned short ys[0x10];            /* 0x374 */
    unsigned short f394;                /* 0x394 */
    volatile unsigned short f396;       /* 0x396 */
    volatile unsigned short f398;       /* 0x398 */
    unsigned char pad4[0x39c - 0x39a];
    unsigned short f39c;                /* 0x39c */
    unsigned short f39e;                /* 0x39e */
    unsigned char pad5[2];
    unsigned short f3a2;                /* 0x3a2 */
};

extern void Func_801b9a8(struct Party *p, int a);
extern void Func_801b9ec(struct Party *p, int a);
extern void Func_801ba68(struct Party *p, int a);
extern void Func_801bd98(int x, int y, struct Node *n, int m);
extern void Func_801b010(int id, int a);
extern void WaitFrames(int n);

void Func_801b664(struct Party *p)
{
    struct Node *n;
    unsigned int m;
    int v;

    Func_801b9a8(p, p->f39e);
    p->f3a2 = 0x21;
    WaitFrames(1);
    p->f39e = p->f39e + 1;
    if ((unsigned int)p->f394 > 5) {
        m = p->f39c + p->f39e;
        if (m == p->f394) {
            n = p->head;
            p->fa = 0;
            if (n->next != 0) {
                do {
                    n->ty = p->f396;
                    n->step = -0xc;
                    n->f1a = p->f398;
                    n = n->next;
                } while (n->next != 0);
            }
            n->ty = p->f396;
            do { } while (0);
            n->f1a = p->f398;
            n->step = -0xc;
            while (n->ty != n->y)
                WaitFrames(1);
            n = p->head;
            if (n != 0) {
                unsigned short *q = &p->xs[0];
                do {
                    Func_801bd98(q[0], q[0x10], n, 1);
                    n = n->next;
                    q++;
                } while (n != 0);
            }
            p->f39e = 0;
            p->f39c = 0;
            n = p->head;
            v = n->y + 0x10;
            n = n->next;
            if (n != 0) {
                do {
                    n->ty = v;
                    n->step = 0xc;
                    n = n->next;
                    v += 0x10;
                } while (n != 0);
            }
            p->f3e = 1;
        } else if (p->f39e == 4 && m + 1 < p->f394) {
            p->f39e = p->f39e - 1;
            p->f3c = 8;
            p->f39c = p->f39c + 1;
            Func_801ba68(p, 1);
            if (p->f39c + p->f39e + 2 == p->f394)
                p->f3e = 0;
            p->fa = 1;
        }
    } else {
        if (p->f39e == p->f394)
            p->f39e = 0;
    }
    p->f3a2 = 1;
    Func_801b9ec(p, p->f39e);
    WaitFrames(1);
    Func_801b010(p->head->id, 0);
    WaitFrames(1);
}
