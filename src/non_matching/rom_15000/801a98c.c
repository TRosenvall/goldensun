/* Func_801a98c (0x0801a98c) -- NON-MATCHING: 575 encodings of 645 differ (objcmp).
 * THAT NUMBER IS NOT A DISTANCE: reference 645 instructions / 1392 bytes, ours 635 /
 * 1356, so the streams misalign at index 9 and the tail is reported wholesale.  The
 * honest measure, from a register/immediate-normalised instruction-stream diff:
 * 617 reference instructions, 364 of them identical, ~309 differing.  This is a
 * FIRST PASS -- the whole function is transcribed and every block is in the right
 * place; what is left is register allocation and a handful of constant-mode details.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801a98c.c \
 *     asm/rom_15000/rom_1a66c_c_c.s --whole
 *
 * ===================== THE SPLIT THIS NEEDS =====================
 * asm/rom_15000/rom_1a66c_c_c.s holds ONE function and a `.section .rodata` tail
 * with a single label, `.L36740` (`.incrom 0x36740, 0x36750`, 16 bytes).  The file
 * carries NO `.global` at all, which is why tools/datacheck.py printed no EXPORTS
 * line for it -- treat any earlier note calling this file "clean" as wrong.
 *   1. add `.global .L36740` IMMEDIATELY BEFORE the `.L36740:` label (not in the
 *      preamble -- split_s.py refuses a preamble holding more than includes);
 *   2. `python3 tools/split_s.py asm/rom_15000/rom_1a66c_c_c.s Func_801a98c`,
 *      giving `_a` (the code) and `_b` (the .rodata); build + compare must be
 *      byte-neutral before anything else happens;
 *   3. stage1.ld already lists this object twice -- line 411 for `(.text)` and
 *      line 624 for `(.rodata)` -- so the two pieces slot into those two lines;
 *   4. the C then references the table as
 *        extern u8 L36740[] __asm__(".L36740");
 *      which is the tree's convention (see src/rom_c9000/rom_dd2ac_c_c_b.c).
 *      L36740 is read BOTH ways: `*(signed char *)(L36740 + i) >> 1` in the second
 *      section (the ROM's `lsl #24 / asr #25`) and plain `L36740[i]` in the fourth
 *      (`ldrb`), so it must be declared unsigned and cast at the signed site.
 *
 * ===================== WHAT IS ESTABLISHED =====================
 * FOUR SECTIONS, in this order:
 *   1. a guarded do-while over the list at *(void **)(base + (0xd2 << 2)), counting
 *      with i, animating each entry's OBJ sub-object at +0x28;
 *   2. the single entry at base + (0xc3 << 2) (q2), gated on q2->fa != 0;
 *   3. two DisplayMenuArrowCursor calls, then a second guarded do-while over the
 *      list at *(void **)(base + (0xd3 << 2));
 *   4. the single entry at base + (0xb6 << 2) (q1), gated on q1->fa != 0, which is
 *      the only place UploadSpriteGFX and the p = base + (0xc0 << 2) sprite record
 *      are touched; then `*(u16 *)(base + 0x3a2) += 1` and return.
 *
 * THE OBJ SUB-OBJECT IS THE SAME STRUCT AS Func_80191cc'S, one bank over, and the
 * bitfield masks are read straight off the ROM:
 *     +0x05 byte : c0:2 (0x03) c2:2 (0x0c) c4:1 (0x10) c5:1 (0x20) c6:2 (0xc0)
 *     +0x06 half : x:9 (0x1ff / 0xfffffe00), pri:5 via BYTE ops on +7 (0x3e),
 *                  mode:2 (0xc0)
 *     +0x08 half : tile:10 (0x3ff / 0xfffffc00), then d10:2, d12:2, d14:2
 * The ROM's `and` with ~0xc on BYTE +0x09 is `d10 = 0` -- bits 10-11 of the +8
 * halfword, i.e. bits 2-3 of byte 9.  Writing `d12 = 0` instead costs a
 * `mov #0x31 / neg` where the ROM reuses its ~0xc; that was the bug in the first
 * pass and is fixed here.
 * BITFIELDS ARE THE RIGHT SPELLING, NOT EXPLICIT MASKS, and this is now proved
 * rather than assumed (scratch_elev/b292/H/probe3.c):
 *   - the ROM READS the 9-bit field with `lsl #23 / lsr #23`, which is what
 *     extract_bit_field gives; an explicit `& 0x1ff` reaches the same shape only
 *     through combine;
 *   - the ROM's read-modify `o->x = o->x - 16` uses `add r3, 0xfff0` from the pool,
 *     i.e. HImode arithmetic, which the bitfield spelling reproduces exactly and
 *     an explicit `((x & 0x1ff) - 16)` does NOT (that is int, giving `sub #16`);
 *   - written as explicit masks, gcc builds ~0x1ff as `mov r3,#254 / lsl r3,#8`
 *     instead of pooling it, which the ROM never does.
 *
 * s16 FIELDS ARE READ BOTH WAYS FROM ONE DECLARATION.  `mov r1,#0x10 /
 * ldrsh r2,[r7,r1]` is a SIGNED read of a `short` member (Thumb has no
 * immediate-offset ldrsh, hence the constant in a register), and `ldrh r1,[r7,#0x10]`
 * beside it is the same member read unsigned because the value is about to be
 * truncated back into 16 bits.  So `n->x != n->xt` and `n->x = n->x + n->xs;` on
 * ONE `short x;` member produce both forms -- there is no second member and no cast
 * in the source.  Section 1's four-way clamp is
 *     if (n->x != n->xt) {
 *         if (n->xs > 0) { if (n->x + n->xs >  n->xt) n->x = n->xt; else n->x += n->xs; }
 *         else           { if (n->x + n->xs <  n->xt) n->x = n->xt; else n->x += n->xs; }
 *         o->x = n->x;
 *     }
 * and it comes out instruction-for-instruction right, branch senses included.
 *
 * Func_8003d28 takes a pointer to the SAME three-halfword request record as in
 * Func_80191cc, but here it lives at a fixed address, base + (0xd0 << 2); the three
 * stores are written with three DIFFERENT address spellings in the ROM
 * (`0xd0 << 2`, a pooled `0x342`, `0xd1 << 2`), so they are separate raw-cast
 * expressions and not one struct pointer.
 *
 * ===================== WHAT IS LEFT, NAMED =====================
 * (a) REGISTER ALLOCATION, the same class as Func_80191cc's park.  The ROM keeps
 *     the list cursor in r7, the OBJ pointer in r5, i in r11 (a HIGH register --
 *     only `add r11,r3` and `cmp r11,r3` touch it, both high-capable) and SPILLS
 *     the p = base + (0xc0 << 2) sprite pointer to sp+8, reloading it five times in
 *     section 4.  Ours does the opposite: p takes r11 and i is spilled.  p's class
 *     is LO_REGS (it is a load base), i's is GENERAL_REGS, so the ROM's assignment
 *     is the one that respects the classes; ours is a global.c priority inversion,
 *     priority being floor_log2(n_refs)*n_refs/live_length*10000*size.  p has ~5
 *     refs over the whole function, i ~4 refs over section 1 only.  This is the
 *     single biggest remaining block of differences and it is where the next
 *     attempt should start.  Frame size follows from it: ROM 0x18, ours 0x14.
 * (b) The two mask constants of every 9-bit and 10-bit bitfield write load as
 *     `ldr` (SImode) in the ROM and `ldrh` (HImode) here -- about two encodings per
 *     site.  The pool WORD is identical (`.word 511` / `.word -512`); only the load
 *     width differs, so this is the RTL mode of the constant, which store_bit_field
 *     takes from get_best_mode(bitsize, bitpos, align, ...) (stor-layout.c).  At
 *     bitpos 48 with align reduced to 16 by `bitpos & -bitpos`, get_best_mode's
 *     SLOW_BYTE_ACCESS branch can only reach HImode.  Probe evidence
 *     (scratch_elev/b292/H/probe2.c, probe3.c): NO C spelling of the field tried --
 *     u16 container, u32 container, explicit masks -- moves it, and when gcc has no
 *     0x1ff in hand at all it uses `lsl/lsr` instead.  The ROM's SImode pair
 *     therefore looks like it comes from the constants being CSE'd out of an SImode
 *     use elsewhere in the same block, which is a shape question, not a type one.
 *     DO NOT spend another session re-testing the type spellings; they are closed.
 * (c) THE SHARED-MASK ORDERING LEVER IS APPLIED HERE AND IT PAID.  In section 2 the
 *     ROM shares ONE `mov r0,#0x3f` between the byte-5 `c6` write and the byte-7
 *     `mode` write.  Writing `o->mode = 2;` BEFORE `o->c6 = 0;` (rather than after)
 *     makes gcc share it too: 351 -> 364 identical instructions, 329 -> 309
 *     differing.  Moving `o->pri = 0;` down as well gave 360, so only the
 *     mode/c6 pair matters.  This is the same mechanism that paid 337 -> 342 in
 *     Func_80191cc's park, and it is the one reusable finding of this pass: WHEN
 *     TWO BITFIELD WRITES IN DIFFERENT BYTES SHARE A MASK CONSTANT, THEIR SOURCE
 *     ORDER DECIDES WHETHER cse2 COPIES THE CONSTANT OR CONSUMES IT, and the copy
 *     is what the ROM has.
 */

#include "gba/types.h"

struct Spr {
    u32 f0;
    u8  f4;
    u8  c0:2, c2:2, c4:1, c5:1, c6:2;
    u16 x:9, pri:5, mode:2;
    u16 tile:10, d10:2, d12:2, d14:2;
};

struct Ent {
    u32 f0;
    struct Ent *next;
    u16 f8;
    u16 fa;
    u16 fc;
    u16 fe;
    short x;
    short y;
    short xs;
    short ys;
    short xt;
    short yt;
    u16 f1c;
    u16 f1e;
    u16 f20;
    short f22;
    u16 f24;
    short f26;
    struct Spr spr;
};

extern u8 *iwram_3001e98;
extern u32 iwram_3001800;
extern u8 Data_346f8[];
extern u8 L36740[] __asm__(".L36740");

extern int UploadSpriteGFX(int id, int n, void *src);
extern int Func_8003d28(void *r);
extern void Func_8003dec(struct Spr *o, int n);
extern int Func_801b36c(void *base);
extern void DisplayMenuArrowCursor(void *base, int which);
extern int _GetFlag(int id);
extern int _Func_80b845c(int id, int *out);

void Func_801a98c(void)
{
    u8 *base;
    struct Spr *p;
    struct Ent *q1;
    struct Ent *q2;
    struct Ent *n;
    struct Spr *o;
    int i;
    int arg;
    int d;
    int y;
    int t;
    u16 yv;
    int tmp[2];

    base = iwram_3001e98;
    p = (struct Spr *)(base + (0xc0 << 2));
    i = 0;
    q1 = (struct Ent *)(base + (0xb6 << 2));
    q2 = (struct Ent *)(base + (0xc3 << 2));
    n = *(struct Ent **)(base + (0xd2 << 2));
    if (n != 0) {
        do {
            o = &n->spr;
            o->c0 = 0;
            o->pri = 0;
            o->x = n->x;
            yv = n->y;
            o->f4 = yv;
            arg = 0xf0;
            if (*(u16 *)(base + (0xe8 << 2)) != 0) {
                o->f4 = *(u16 *)(base + (0xe8 << 2)) + yv;
            } else if (n->x != n->xt) {
                if (n->xs > 0) {
                    if (n->x + n->xs > n->xt)
                        n->x = n->xt;
                    else
                        n->x = n->x + n->xs;
                } else {
                    if (n->x + n->xs < n->xt)
                        n->x = n->xt;
                    else
                        n->x = n->x + n->xs;
                }
                o->x = n->x;
            } else if (i == *(u16 *)(base + 0x39e)) {
                arg = 0xf1;
                if (q1->fa != 0 && _Func_80b845c(n->f8, tmp) != -1) {
                    q1->xt = tmp[0];
                    q1->yt = tmp[1];
                    if (q1->f22 == 0) {
                        q1->x = tmp[0];
                        q1->y = tmp[1];
                        q1->f22 = 1;
                    }
                }
            }
            if (n->f22 != 0) {
                if (_GetFlag(0x103) != 0) {
                    if (*(u16 *)(base + 0x2e2) == 1)
                        o->c2 = 1;
                    else
                        o->c2 = 0;
                    if (n->fa == 1)
                        o->c2 = 1;
                }
                Func_8003dec(o, arg);
            }
            n = n->next;
            i++;
        } while (n != 0);
    }
    if (q2->fa != 0) {
        n = (struct Ent *)Func_801b36c(base);
        o = &q2->spr;
        o->c2 = 0;
        o->c0 = 0;
        o->pri = 0;
        o->c4 = 0;
        o->c5 = 1;
        o->mode = 2;
        o->c6 = 0;
        o->d10 = 0;
        o->tile = q2->fe;
        o->x = n->x - 4;
        o->f4 = n->y + (*(signed char *)(L36740 + ((iwram_3001800 >> 1) & 0xf)) >> 1) - 4;
        if (q2->f22 != q2->f26) {
            *(u16 *)(base + (0xd0 << 2)) = q2->f22;
            *(u16 *)(base + 0x342) = q2->f22;
            *(u16 *)(base + (0xd1 << 2)) = 0;
            o->pri = Func_8003d28(base + (0xd0 << 2));
            o->c0 = 3;
            o->x = o->x - 16;
            o->f4 = o->f4 - 16;
            q2->f22 = q2->f22 + q2->f24;
        }
        if (_GetFlag(0x103) != 0)
            o->c2 = 1;
        Func_8003dec(o, 0xf8);
    }
    DisplayMenuArrowCursor(base, 0);
    DisplayMenuArrowCursor(base, 1);
    n = *(struct Ent **)(base + (0xd3 << 2));
    if (n != 0) {
        do {
            o = &n->spr;
            if (n->x != n->xt)
                n->x = n->x + n->xs;
            if (n->y != n->yt)
                n->y = n->y + n->ys;
            o->x = n->x;
            o->f4 = n->y;
            if (n->f22 != n->f26) {
                n->f22 = n->f22 + n->f24;
                *(u16 *)(base + (0xd0 << 2)) = n->f22;
                *(u16 *)(base + 0x342) = n->f22;
                *(u16 *)(base + (0xd1 << 2)) = 0;
                o->pri = Func_8003d28(base + (0xd0 << 2));
                o->c0 = 3;
                o->x = o->x - 8;
                o->f4 = o->f4 - 8;
            } else {
                o->c0 = 0;
                o->pri = 0;
            }
            if (_GetFlag(0x103) != 0) {
                if (*(u16 *)(base + 0x2e2) == 1)
                    o->c2 = 1;
                else
                    o->c2 = 0;
                if (n->fa == 1)
                    o->c2 = 1;
            }
            Func_8003dec(o, 0xf0);
            n = n->next;
        } while (n != 0);
    }
    if (q1->fa != 0) {
        t = UploadSpriteGFX(q1->fc, 0x100,
                            &Data_346f8[((iwram_3001800 >> 2) & 0xf) << 8]);
        p->tile = t;
        if (q1->xt != q1->x) {
            d = (q1->xt - q1->x) >> 1;
            if (d != 0)
                q1->x = q1->x + d;
            else
                q1->x = q1->xt;
        }
        y = q1->y;
        if (q1->yt != q1->y) {
            d = (q1->yt - q1->y) >> 1;
            if (d != 0) {
                y = y + d;
                q1->y = y;
            } else {
                y = q1->yt;
                q1->y = y;
            }
        }
        p->f4 = y + L36740[(iwram_3001800 >> 2) & 0xf] - 0x20;
        p->x = q1->x - 4;
        if (_GetFlag(0x103) != 0) {
            if (*(u16 *)(base + 0x2e2) == 1)
                p->c2 = 1;
            else
                p->c2 = 0;
        }
        Func_8003dec(p, 0xf8);
    }
    *(u16 *)(base + 0x3a2) = *(u16 *)(base + 0x3a2) + 1;
}
