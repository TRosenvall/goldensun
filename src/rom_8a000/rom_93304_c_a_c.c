/* Func_942e0 -- 0x080942e0.  *** MATCHING ***  (was parked at 2 of 52 WITH ONE
 * REGISTER PIN, 15 of 52 pin-free.)  THIS LANDING IS PIN-FREE.
 *
 * Batch 321, brief D.  Both of the park's open claims were wrong, and both were
 * wrong because its two negative lists were never crossed.
 *
 * VERIFIED (tools/objcmp.py, THE AUTHORITY, inside the container):
 *   OK Func_942e0 -- 116 bytes, 52 encodings and 5 relocations identical
 *   --whole: OK whole file -- 116 bytes, 52 encodings and 5 relocations identical
 * ZERO PINS, zero devices, no fakematch row, no per-file Makefile rule.
 *
 * Verify with (the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_93304_c_a_c.c \
 *     asm/rom_8a000/rom_93304_c_a_c.s --func Func_942e0
 *
 * SPLIT SHAPE: NONE NEEDED.  tools/split_s.py --dry-run says "holds only
 * Func_942e0 and no data; convert it directly, no split needed"; datacheck.py
 * reports no data sections.  No exports, no linker-script change.
 *
 * ================= WHAT THE PARK CLAIMED, AND WHAT SURVIVED =================
 *
 * Claim 1 (the residue): "THE LAST TWO ARE A BARE CONSTANT sched2 PLACES EARLY
 *   ... NEXT: the open question is what gives a bare `mov #K` a LOWER sched2
 *   priority than an adjacent independent store."
 *   -> REPRODUCED as an observation, REFUTED as a question.  It is the other way
 *      round: the bare `mov #K` has a *HIGHER* priority than the store, by
 *      exactly one.  See THE SCHED2 ARITHMETIC below.
 *
 * Claim 2 (the pin): "PINNING EITHER MEMBER TAKES IT 15 -> 2, and it really is
 *   either ... that is the evidence this is not a priority ordering that some
 *   spelling could shift."
 *   -> REFUTED.  It is exactly a priority ordering, the priority is
 *      local-alloc's, and the margin is ONE INSN of live length.
 *
 * Claim 3 (batch 268): "THE STRUCT-TYPING LEVER IS INERT HERE ... So the residue
 *   below is NOT a typing artefact."
 *   -> REFUTED, and this is the cross-the-lists law in its purest form.  Batch
 *      268 typed BOTH byte stores at once, which is self-cancelling.  Typing the
 *      LAYER store ALONE -- and deliberately leaving the PART store as a plain
 *      `u8 *` access -- is the whole of the sched2 fix.
 *
 * Claim 4: "-fno-schedule-insns2 IS RULED OUT ... Do not add a SCHED2 rule for
 *   this file."  -> REPRODUCED and still true.  No flag rule is used here.
 *
 * ===================== THE SCHED2 ARITHMETIC, FROM THE DUMP =====================
 *
 * Read off `-da -dS`'s .23.sched2 dependence lists for the parked body.  The
 * four insns are 41 (`mov r1,#0`, z), 49 (`strb r1,[r6]`, the part store), 52
 * (`mov r3,#15`) and 53 (`strb r3,[r0,#5]`, the layer store); 56 is the next
 * memory reference, `ldr r3,[r5,#8]`.  The dump shows
 *
 *     insn 49  (mem:QI (reg r6) 0)              <- ALIAS SET 0
 *     insn 53  (mem:QI (plus (reg r0) 5) 0)     <- ALIAS SET 0
 *     insn 56  (mem/s:SI (plus (reg r5) 8) 6)   <- ALIAS SET 6
 *     insn 56's LOG_LINKS: ... (insn_list 49 ...) (insn_list 53 ...)
 *
 * so BOTH byte stores have a TRUE (store-to-load) edge into insn 56, because a
 * `u8 *` dereference carries alias set 0 and set 0 conflicts with everything.
 * In gcc-2.96's scheduler an ANTI or OUTPUT edge costs 0 and a TRUE edge costs
 * the producer's latency, so:
 *
 *     pri(53) = 1 + pri(56)
 *     pri(49) = max( 0 + pri(53),  1 + pri(56) ) = 1 + pri(56)
 *     pri(52) = 1 + pri(53)                      = 2 + pri(56)
 *
 * The constant's `mov` sits ONE TRUE-DEPENDENCE HOP FURTHER FROM THE BLOCK END
 * than the store does -- through its own store -- while the store's own edge to
 * the next memory reference costs the same 1.  So `mov r3,#15` wins by exactly
 * one and is issued first.  Nothing about the constant is special; the park was
 * looking for a way to LOWER it when what was needed was to lower the insn it
 * feeds.
 *
 * THE FIX IS TO GIVE THE LAYER STORE A NON-CONFLICTING ALIAS SET, so that
 * pri(53) = 0 and therefore pri(52) = 1, while the part store keeps its alias-0
 * edge into insn 56 and so keeps a large priority.  Declaring the
 * _Sprite_AddLayer return as a real `struct SpriteLayer *` and writing
 * `layer->unk_05` does exactly that and nothing else; the ROM's
 * `strb r1,[r6] / mov r3,#0xf / strb r3,[r0,#5]` comes straight out.
 *
 * Measured: the park's body with ONLY the layer typed is 0 differing with the
 * pin (from 2), and `((struct SpriteLayer *)layer)->unk_05 = 0xf` with `layer`
 * left as `u8 *` is identical -- so it is the ACCESS's alias set that matters,
 * not the declaration's spelling.  Typing the PART store as well puts it back
 * to a non-conflicting set, kills ITS edge into insn 56 too, and the figure
 * returns -- which is precisely why batch 268 measured typing as inert.
 *
 * ================== THE DEPIN, AND IT IS ONE INSN OF LIVE LENGTH ==================
 *
 * `tools/objcmp.py` figures alone made this look like a wall: SIXTEEN separate
 * allocation spellings all measured 14 differing, dead flat.  The reason is
 * worth more than the landing: `-da`'s .17.lreg prints the allocator's ACTUAL
 * INPUTS, and for every one of those sixteen variants they were BIT-IDENTICAL:
 *
 *     Register 33 used 11 times across 26 insns ... crosses 1 call   <- the actor
 *     Register 35 used  3 times across  7 insns ... crosses 1 call   <- the part
 *     ;; Register 33 in 6.   ;; Register 35 in 5.
 *
 * Every one of those edits -- a named local for the 0x1b argument, a named local
 * for the gState load, chained assignments, `p += 0x26`, `p` as an int address,
 * declaration order in three permutations, a second layer pointer, `~0xfffff`
 * for the mask -- was folded away before flow.c ever counted anything.  A flat
 * figure sweep could not distinguish "the lever is inert" from "the edit never
 * happened", and this one was the second.
 *
 * With the real numbers in hand the contest is arithmetic.  Three qtys need a
 * call-saved register (the actor, the part, and the `anim` parameter) and only
 * r5 and r6 are cheap -- r7 is the frame pointer, so the loser goes to r8 and
 * pays the ROM's `mov r6,r8 / push {r6}`.  local-alloc assigns in order of
 * decreasing n_refs/live_length, and
 *
 *     actor  11/26 = 0.4231
 *     part    3/7  = 0.4286     <- wins r5 by 1.3%
 *
 * The ROM needs the actor to win.  ONE MORE INSN inside the part pointer's live
 * range is enough: 3/8 = 0.3750, and the actor takes r5 with the part in r6 and
 * `anim` in r8, exactly as the ROM has them.  `f = 0xf;` written as its own
 * statement BEFORE the part store is that insn -- it lies inside the part's
 * range, it survives to flow because its value is consumed by a later store,
 * and reload still emits it as the ROM's `mov r3,#0xf` in the ROM's slot
 * because the typed layer store gave it priority 1.
 *
 * Measured, pin-free (figure + the .17.lreg line for the part pointer):
 *
 *   parked body, pin removed                        14   3/7  a->r6 p->r5
 *   + layer typed only                              14   3/7  a->r6 p->r5
 *   + `f = 0xf` BETWEEN the two stores (the park's
 *     own spelling -- outside the part's range)     14   3/7  a->r6 p->r5
 *   + a second layer pointer before the part store  14   3/7  a->r6 p->r5
 *   + the two stores SWAPPED                         3   3/9  a->r5 p->r6
 *   + `f = 0xf` BEFORE the part store, `u8 f`        0   3/8  a->r5 p->r6  <-- here
 *   + the same with `int f`                          0   3/8  a->r5 p->r6
 *   + `f = 0xf` before `z = 0`                       0   3/8  a->r5 p->r6
 *   + mask hoisted into a local before the store    12   3/8  a->r5 p->r6
 *
 * The swapped-store row is the one the park threw away ("Reordering the two
 * stores is 13 at best"): it is NOT a dead end, it is the row that first showed
 * the allocation can move at all, and it moves for the same reason -- putting
 * the layer store first pushes the part pointer's death two insns later.
 *
 * ========================= WHAT TO TAKE AWAY =========================
 *
 * 1. SCREEN AN ALLOCATION EDIT BY .17.lreg, NOT BY THE FIGURE.  "Register N used
 *    X times across Y insns" is the allocator's input.  If an edit does not move
 *    it, the edit did not happen, and its inert figure says nothing about the
 *    lever.  Sixteen variants and three rounds were spent before this was read.
 * 2. A PIN CAN BE WORTH ONE INSN OF LIVE RANGE.  When two qtys contend for one
 *    call-saved register, print both ratios; a margin under a few percent is
 *    reachable by adding ONE statement inside the shorter range.
 * 3. ALIAS SETS ARE A SCHED2 LEVER, AND THEY ARE ASYMMETRIC.  Typing an access
 *    REMOVES its dependence edges, which lowers the priority of everything that
 *    feeds it.  Typing one of two adjacent byte stores is a lever; typing both
 *    is a no-op.  This is the brief's humanization Sec.3 pattern 4 with the
 *    mechanism named: the declaration mattered because of the ALIAS SET it
 *    carries, not because of the arithmetic it spells.
 */
#include "gba/types.h"

struct Actor {
    u8 pad00[8];
    int x;              /* 0x08 */
    u8 pad0c[4];
    int z;              /* 0x10 */
    u8 pad14[0x10];
    int f24;            /* 0x24 */
    u8 pad28[4];
    int f2c;            /* 0x2c */
    u8 pad30[8];
    int f38;            /* 0x38 */
    u8 pad3c[4];
    int f40;            /* 0x40 */
    u8 pad44[0xc];
    u8 *part;           /* 0x50 */
};

struct SpriteLayer {
    u8 pad00[5];
    u8 unk_05;          /* 0x05 */
};

extern unsigned char gState[];
extern struct Actor *GetFieldActor(int id);
extern struct SpriteLayer *_Sprite_AddLayer(u8 *part, int n);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void WaitFrames(int n);

void Func_942e0(int anim)
{
    struct Actor *a;
    u8 *gs;
    u8 *p;
    struct SpriteLayer *layer;
    int z;
    u8 f;

    gs = gState;
    gs += 0xfa << 1;
    a = GetFieldActor(*(int *)gs);
    p = a->part;
    layer = _Sprite_AddLayer(p, 0x1b);
    z = 0;
    f = 0xf;
    p[0x26] = z;
    layer->unk_05 = f;
    a->x = (a->x & 0xfff00000) + (0x80 << 12);
    a->z = (a->z & 0xfff00000) + (0x80 << 13);
    a->f24 = z;
    a->f2c = z;
    a->f38 = 0x80 << 24;
    a->f40 = 0x80 << 24;
    _Actor_SetAnim(a, anim);
    WaitFrames(0x12);
}
