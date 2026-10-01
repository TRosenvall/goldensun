/* Func_8025200 -- NON-MATCHING, 712 of 817 encodings differ.
 * (objcmp at PRODUCTION FLAGS, pre-split reference, --func Func_8025200.)
 *
 * SIZE IS NOT EXACT: ref 1836 bytes, ours 1812.
 * COUNT IS NOT EXACT: ref 817 encodings, ours 807.
 * So the 712 SATURATES and is not a true distance -- rank with the aligncmp
 * figure below, which is position-tolerant.
 * aligncmp: 518 aligned-equal = 63.4 percent of ref; 358 differing/ins/del in
 * 148 hunks.
 * PIN-FREE: tools/shimcount.py reports nothing at all -- no register asm, no
 * __asm__ (""), no .equ, no per-file flag row, no fakematch entry.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/8025200.c \
 *       asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s --func Func_8025200
 * AFTER THE SPLIT IS INSTALLED the reference path becomes
 *       asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_a.s   (and --func may be dropped)
 *
 * THE SPLIT SHAPE -- ONE SPLIT SERVES BOTH TARGETS, RUN IT FOR Func_802592c.
 * tools/split_s.py --dry-run asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s Func_802592c
 *   asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_a.s   1 function,  873 lines  <- Func_8025200
 *   asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_b.s   1 function,  892 lines  <- Func_802592c
 *   asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_c.s   2 functions, 1949 lines <- 8026080, 8026e80
 *   REMOVE asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s ; rewrite stage1.ld
 * Re-confirmed by dry-run in batch 309.  Running the Func_8025200 split instead
 * puts 802592c into a 3-function _c and forces a SECOND split later.
 *
 * ================ LEVERS THAT PAID, IN ORDER, WITH FIGURES ================
 *
 * (1) THE OBJ ELEMENT IS WALKED BY A POINTER, NOT SUBSCRIPTED.  Loop 1 written
 *     as objs[i].w[1] = ... gives gcc a giv whose base already carries the +4,
 *     so it emits str [r4,#0] / str [r4,#4] / ldrh [r4,#2] where the ROM has
 *     [r4,#4] / [r4,#8] / [r4,#6].  A declared union Ent *e advanced with e++
 *     makes the base register the pointer itself and every field offset lands.
 *     objcmp 768 -> 779 and aligned 56.9 -> 54.6 percent -- BOTH FIGURES WENT
 *     THE WRONG WAY while the addressing became exact, because moving a block
 *     re-seats the LCS.  Kept on the encoding evidence, not the figure.
 *
 * (2) THERE IS NO rowx2 VARIABLE; row * 2 IS WRITTEN AT BOTH USE SITES AND
 *     gcse/PRE MANUFACTURES THE COPIES.  The ROM has THREE mov/lsl/str blocks
 *     at .L258aa/.L258b2/.L258be plus one shared str, and one more in the loop
 *     preheader -- that is PRE inserting row * 2 on every edge that redefines
 *     row, and sp+0x14 being one of the LOWEST slots dates the pseudo to a pass,
 *     not to a declaration.  Writing the assignments by hand in each arm makes
 *     gcc cross-jump them away.  Deleting the variable: count 801 -> 807,
 *     size 1800 -> 1812, aligned 54.6 -> 55.2 percent.  Worth 6 instructions.
 *
 * (3) THE WHOLE VARIABLE PARTITION AT ONCE -- THE BIG ONE.  Nine reused locals
 *     split so that each loop and each region owns its own:
 *         i2  loop 2 counter          i3  Func_8003dec loop counter
 *         i4  cleanup counter         gq  item-loop gfx walker
 *         gr  cleanup gfx walker      k2/m2/lim2  the SECOND divide loop
 *         p2  the confirm block's b + top + row
 *     objcmp 781 -> 712, aligned 55.2 -> 63.4 percent, differing/ins/del
 *     459 -> 358, and the frame went 0x120 -> 0x128 against the ROM's 0x130,
 *     i.e. six of the eight missing spill slots appeared.
 *     THIS IS LEVER 2 OF THE BRIEF AND ITS NON-ADDITIVITY WARNING IS EXACT:
 *     three pure declaration-REORDER probes (boxB declared first; boxB first
 *     plus row before top; row before top alone) were ALL INERT at 807 and
 *     55.1-55.2 percent.  Reordering never changes the allocno COUNT; splitting
 *     does, and only the whole partition moved anything.
 *
 * ================ MEASURED INERT OR WORSE -- DO NOT RE-TRY ================
 *
 * MANUAL INT READ-MODIFY-WRITE INSTEAD OF BITFIELDS -- WORSE, and instructive.
 *   *(u16 *)(e + 6) = (*(u16 *)(e + 6) & 0xfffffe00) | (((boxB->x << 3) + 8) & 0x1ff)
 *   reads count 810 (CLOSER than 807) but aligned 44.1 percent, because combine
 *   NARROWS the old-value mask into HImode -- it emits mov r4,#254 / lsl r4,#8
 *   for 0xfe00 where the ROM has a pooled 0xfffffe00 -- and it also collapses
 *   loop 2's base-plus-index into a single walking pointer, destroying the ROM's
 *   ldrh r3,[r5,r6].  This is 8022a7c's recorded "manual RMW makes the mask
 *   become mov #0xfe / lsl #8" confirmed from a second function.
 *   THE BITFIELD IS THE SPELLING.  It is what gives the ROM's SImode pooled
 *   0xfffffe00 / 0xfffffc00 and its value-masked-first order.
 *
 * A BITFIELD STORE PLUS AN EXPLICIT & 0x1ff / & 0x3ff ON THE VALUE -- 805
 *   instructions, 45.2 percent.  WORSE.  (Note the OPPOSITE sign on the sibling
 *   Func_802592c, where an explicit mask on a value that comes from MEMORY is
 *   right -- see that park.)
 *
 * unsigned int BITFIELD CONTAINERS INSTEAD OF u16 -- EXACTLY INERT: 807, 55.2
 *   percent, hunk-for-hunk identical to the u16 spelling.  get_best_mode picks
 *   HImode at byte 6 either way.  Confirms 8022a7c's "u32 vs u16 bitfield
 *   containers" inert note from a second function; stop testing this.
 *
 * DECLARATION ORDER of the register-resident locals -- three probes, all inert
 *   (807, 55.1-55.2 percent).  See lever 3 above for why.
 *
 * AN EXPLICIT WALKING POINTER IN THE Func_8003dec LOOP -- 63.0 percent, slightly
 *   worse; &objs[i3] already strength-reduces to the ROM's add r5,#0xc.
 * A NAMED struct Ctx * LOCAL FOR iwram_3001f34 -- exactly inert (712, 63.4).
 *
 * FOUR SPELLINGS AGAINST THE ldrb NARROWING -- all inert or worse.  The ROM
 *   reads ldrh r3,[r1,#0xe] for boxB->y feeding the BYTE store at +4; ours reads
 *   ldrb r3,[r1,#0xe], because combine legitimately does the whole
 *   ((i * 2 + y) << 3) + 4 in QImode and narrows the HImode MEM with it.
 *   Tried: a named int local for boxB->y (62.3 percent, worse); the addends
 *   reversed; an (unsigned short) cast on the sum; an (int) cast on the shift --
 *   the last three exactly inert at 712 / 63.4.  No source handle found.
 *
 * ================ THE BLOCKER ================
 *
 * REGISTER ALLOCATION -- global.c's allocno priority, an ALLOCNO COUNT problem,
 * not an ordering one.  The ROM's frame is 0x130 with 21 stack words; ours is
 * 0x128 with 19.  Concretely the ROM SPILLS cx (sp+0x24), cy (sp+0x28) and both
 * gfx walkers (sp+0x8, sp+0x4) across calls and therefore keeps boxB in r11 for
 * the entire function; ours has spare call-saved registers at those points, so
 * it keeps cx/cy in r5/r6 and spills boxB instead -- which then costs an extra
 * ldr at every one of boxB's ~30 uses and turns the ROM's one-instruction
 * sub r3,#1 (deriving the -1 argument from the 0 already in r3) into our
 * two-instruction mov #1 / neg.
 * WHAT RULES OUT THE ALTERNATIVES:
 *   - NOT scheduling.  sched1 does not run in this build at all, and the
 *     differences are in which quantity lives in memory, which sched2 cannot
 *     change.
 *   - NOT declaration order.  Three reorder probes inert (above).
 *   - NOT the struct or mask spelling.  The u32-container probe is byte-identical
 *     and the manual-RMW probe changes the masks without moving the frame.
 *   - It IS the count: the partition of lever 3 is the only thing that moved the
 *     frame at all, and it moved it 0x120 -> 0x128 of the needed 0x130.  Two
 *     allocnos are still missing and no further split or merge found them in
 *     six builds.
 * A SECOND, INDEPENDENT RESIDUE: gcc's constant-POOL PLACEMENT.  Ours dumps
 * 0x1ff / 0 / 0xfffffe00 INSIDE loop 1's block; the ROM dumps four words
 * (0x1ff, 0, 0xfffffe00, 0xfffffc00) together AFTER loop 1.  Per the brief that
 * is a size-and-count defect in its own right and it shifts every later
 * pc-relative offset, so part of the 358 is downstream of it.
 * Related one-instruction item: the ROM RELOADS ldr r3,=0x1ff each iteration of
 * loop 1 while ours hoists it into a register -- a LICM consequence of having
 * one fewer competing allocno in that loop, i.e. the same root cause.
 *
 * ================ FACTS ESTABLISHED (for docs/elevation.md) ================
 *
 * A. THE SPILL-SLOT RULE CROSS-CHECKS ACROSS A FUNCTION PAIR.  One declaration
 *    order satisfies BOTH slot maps:
 *        s, top, prev_top, row, prev_row, gfx, [unit], boxA, nitems, keep, cy, cx
 *    In Func_8025200 top and row stay in r9/r8, so the spilled run is
 *    s/prev_top/prev_row/gfx/boxA/nitems/keep at 0x44..0x2c.  In Func_802592c
 *    prev_top stays in r9, so the run is s/top/prev_row/gfx/unit/boxA/nitems/keep
 *    at 0x40..0x24.  A declaration that does NOT spill in one function is
 *    invisible in its slot map but still numbered, so reading the two maps
 *    TOGETHER pins an order neither map pins alone.
 * B. cy TAKES THE HIGHER SLOT THAN cx IN BOTH FUNCTIONS (0x28/0x24 here,
 *    0x20/0x1c there) even though cx is COMPUTED FIRST.  Descending-slot order
 *    is declaration order, so these two are declared cy-then-cx.  A scalar's
 *    slot dates its DECLARATION, never its first assignment.
 * C. THE TREE ALREADY HAS THIS RECORD.  union Ent from
 *    src/non_matching/rom_b5000/80bb7c0.c is exactly this 0xc-byte OBJ, down to
 *    e.w[1] = 0x80 << 23, e.w[2] = 0, e.f.tile = UploadSprite2(...) and
 *    Func_8003dec(&e, 0xf0).  Reuse it; do not invent a struct.
 * D. CreateUIBox RETURNS A POINTER, and the box record is the struct Win of
 *    src/rom_15000/rom_18cac_a_c.c (LANDED; was src/non_matching/rom_15000/8018efc.c): pad[8], w at +8, h at +0xa, x at +0xc,
 *    y at +0xe.  Reads at [r11,#8]/[r11,#0xc]/[r11,#0xe] confirm it.
 * E. iwram_3001f34 IS REACHED TWO WAYS IN THE SAME FUNCTION.  Early it is
 *    *(struct Ctx **)((char *)&iwram_3001e8c + 0xa8), which reuses the
 *    iwram_3001e8c pool word and needs add r5,#0xa8 because 0xa8 exceeds Thumb's
 *    5-bit scaled ldr offset; later it is the plain symbol with its own pool
 *    word.  gcc cannot relate two distinct externs, so BOTH spellings are in the
 *    source.
 * F. (c + 4) / 5 IS NOT HOISTED OUT OF EITHER divide LOOP because __divsi3 is a
 *    CALL and loop-invariant motion will not move one; c + 4 IS hoisted, into
 *    r10.  The first loop's body reuses the test's quotient through cse; the
 *    second loop's body re-calls it.  Write the division in the loop condition
 *    and read it again in the body -- do not hoist it into a local.
 * G. 0xf018 IS A LITERAL, NOT A SYMBOL.  Nothing in wram/label/const/size/area/
 *    message.sym carries the value, and src/rom_15000/rom_1de5c_c_a_a.c and
 *    rom_1de5c_c_c_c_c_a_a_a_c_c_b.c already spell 0xf018 as a bare literal.
 *    The add r5,#1 between the Func_80251d4 pairs still identifies 0xf018 and
 *    0xf019 as ONE named base plus one, so they are written as 0xf018 and
 *    0xf019 against a value gcc chains -- which it does here because the base
 *    is already live in r5.
 * H. THE 94-INSTRUCTION SHARED RUN IS GENUINELY ONE SOURCE.  Everything from the
 *    prologue through the four Func_80251d4 calls is identical between the two
 *    targets modulo the CreateUIBox arguments and the frame offsets, and it was
 *    written once and pasted.  Its loop forms are NOT interchangeable: loop 1 is
 *    an ascending i <= 4 whose body uses i, so check_dbra_loop cannot reverse it;
 *    loop 2 is also written ascending and gcc reverses it to the ROM's
 *    sub/cmp #0/bge because its index survives only in the compare.  Write BOTH
 *    ascending -- the countdown source form is not needed here.
 */
#include "gba/types.h"

struct Win {
    unsigned char pad[8];
    u16 w;
    u16 h;
    u16 x;
    u16 y;
};

union Ent {
    int w[3];
    struct {
        int f0;
        u8 f4;
        u8 f5;
        u16 x : 9;
        u16 f6hi : 7;
        u16 tile : 10;
        u16 f8hi : 6;
    } f;
};

struct Ctx {
    unsigned char pad0[0x30];
    int f30;
    int f34;
    int f38;
    unsigned char pad3c[0x10];
    int f4c;
};

extern unsigned char *iwram_3001e8c;
extern struct Ctx *iwram_3001f34;
extern volatile unsigned int iwram_3001e40;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern unsigned char Data_310a4[];

extern int AllocUploadSpriteGFX(int size);
extern int UploadSprite2(int slot, void *gfx);
extern struct Win *CreateUIBox(int a, int b, int c, int d, int e);
extern int CloseUIBox(void *box, int n);
extern void WaitFrames(int n);
extern void Func_80251d4(int a, int b);
extern void Func_8022768(int x, int y, int w, int h, int c);
extern void Func_8016738(void);
extern int Func_8025180(int a, int b);
extern void Func_801965c(int msg, u16 *out, u32 n);
extern void Func_8017aa4(u16 *buf, void *box, int c, int d);
extern void Func_8016498(void *box);
extern void _GetItemInfo(int id);
extern void SetTextColor(int c);
extern void Func_801e7c0(int id, void *box, int x, int y);
extern int Func_8021af0(int a, int b);
extern void Func_8019000(void *box, int id, int a, int b, int e);
extern void Func_800352c(void);
extern void Func_8003dec(union Ent *e, int n);
extern void Func_8003f3c(int n);
extern void _PlaySound(int id);
extern int __divsi3(int a, int b);

int Func_8025200(int a, u16 *b, int c)
{
    union Ent cur;
    u16 buf[0x40];
    int gfxs[5];
    union Ent objs[5];
    unsigned char *s;
    int prev_top;
    int prev_row;
    int gfx;
    struct Win *boxA;
    int nitems;
    int keep;
    int cy;
    int cx;
    struct Win *boxB;
    int top;
    int row;
    int ret;
    int i;
    int i2;
    int i3;
    int i4;
    int k;
    int k2;
    int n;
    int m;
    int m2;
    int g;
    int v;
    int r;
    int lim;
    int lim2;
    u16 *q;
    u16 *p;
    u16 *p2;
    int *gp;
    int *gq;
    int *gr;
    struct Ctx *ctx;
    union Ent *e;

    s = iwram_3001e8c;
    prev_top = -1;
    prev_row = -1;
    gfx = AllocUploadSpriteGFX(0x80);
    boxA = CreateUIBox(0, 5, 0x1e, 4, 0x2a);
    nitems = 0;
    ctx = *(struct Ctx **)((char *)&iwram_3001e8c + 0xa8);
    top = ctx->f34;
    row = ctx->f30;
    keep = ctx->f38;
    boxB = CreateUIBox(0xf, 9, 0xf, 0xb, 6);
    e = objs;
    for (i = 0; i <= 4; i++) {
        e->w[1] = 0x80 << 23;
        e->w[2] = 0;
        e->f.x = (boxB->x << 3) + 8;
        e->f.f4 = ((i * 2 + boxB->y) << 3) + 4;
        e++;
    }
    gp = gfxs;
    for (i2 = 0; i2 <= 4; i2++) {
        g = AllocUploadSpriteGFX(0x80);
        *gp++ = g;
        objs[i2].f.tile = UploadSprite2(g, (void *)-1);
    }
    Func_80251d4(0xf018, 0x80 << 2);
    Func_80251d4(0xf018, 0x201);
    Func_80251d4(0xf019, 0x84 << 2);
    Func_80251d4(0xf019, 0x211);
    while (1) {
        if (top != prev_top || row != prev_row) {
            s[0xea6] = 1;
            Func_8022768(boxB->x + 1, boxB->y + prev_row * 2 + 1,
                         boxB->w - 2, 1, 0xf);
            Func_8016738();
            if (c != 0) {
                p = b + top + row;
                if (Func_8025180(a, *p) == 2)
                    Func_801965c(0x8ee, buf, 0x34);
                else
                    Func_801965c((*p & 0x1ff) + 0x75, buf, 0x34);
            } else {
                Func_801965c(0x8e5, buf, 0x34);
            }
            Func_8017aa4(buf, boxA, 0, 4);
            prev_row = row;
            if (top != prev_top) {
                Func_8016498(boxB);
                q = b + top;
                v = *q;
                n = 0;
                if (v != 0) {
                    gq = gfxs;
                    do {
                        _GetItemInfo(v);
                        SetTextColor(0xf);
                        if (Func_8025180(a, v) != 0)
                            SetTextColor(4);
                        else if ((v & (0x80 << 3)) != 0)
                            SetTextColor(2);
                        Func_801e7c0((v & 0x1ff) + 0x182, boxB, 0x10, n << 4);
                        SetTextColor(0xf);
                        objs[n].f.tile = Func_8021af0(v, *gq++);
                        n++;
                        if (n > 4)
                            break;
                        q++;
                        v = *q;
                    } while (v != 0);
                }
                nitems = n;
                prev_top = top;
            }
            if (c > 5) {
                lim = c + 4;
                for (k = 0; k < lim / 5; k++) {
                    m = k + 0xf301;
                    if (k == top / 5)
                        m = k + 0xf30b;
                    Func_8019000(boxB, m, boxB->w - lim / 5 + k - 2, -1, 0);
                }
            }
            Func_8022768(boxB->x + 1, boxB->y + row * 2 + 1, boxB->w - 2, 1, 0xe);
            s[0xea3] = 1;
            s[0xea6] = 0;
        }
        if (c > 5) {
            lim2 = c + 4;
            for (k2 = 0; k2 < lim2 / 5; k2++) {
                m2 = k2 + 0xf301;
                if ((iwram_3001e40 & 0xf) <= 0xb && k2 == top / 5)
                    m2 = k2 + 0xf30b;
                Func_8019000(boxB, m2, boxB->w - lim2 / 5 + k2 - 2, -1, 0);
            }
            Func_8019000(boxB, 0xf334, boxB->w - lim2 / 5 - 3, -1, 0);
            Func_8019000(boxB, 0xf335, boxB->w - 2, -1, 0);
            s[0xea3] |= 2 << ((boxB->y - 1) >> 2);
        }
        if (nitems > 0) {
            for (i3 = 0; i3 < nitems; i3++)
                Func_8003dec(&objs[i3], 0xf0);
        }
        cx = (boxB->x << 3) - 2;
        cy = ((row * 2 + boxB->y) << 3) + 0x14;
        cur.w[1] = 0x80 << 23;
        cur.w[2] = 0;
        cur.f.tile = UploadSprite2(gfx, Data_310a4);
        cur.f.x = cx + ((iwram_3001e40 & 4) >> 1) + 0xfffc;
        cur.f.f4 = cy - ((iwram_3001e40 & 4) >> 2) + 0xf8;
        if (c != 0)
            Func_8003dec(&cur, 0xf2);
        iwram_3001f34->f34 = top;
        iwram_3001f34->f30 = row;
        iwram_3001f34->f38 = keep;
        if ((gKeyPress & 1) != 0) {
            if (c == 0) {
                ret = -1;
                break;
            }
            p2 = b + top + row;
            ret = top + row;
            _GetItemInfo(*p2);
            r = 0;
            if ((*p2 & (0x80 << 3)) == 0) {
                r = Func_8025180(a, *p2);
                if (r == 0)
                    break;
            }
            _PlaySound(0x72);
            if (r == 2)
                Func_801965c(0x8ee, buf, 0x34);
            else if ((*p2 & (0x80 << 3)) != 0)
                Func_801965c(0x8ec, buf, 0x34);
            else
                Func_801965c(0x8eb, buf, 0x34);
            Func_8016738();
            Func_8017aa4(buf, boxA, 0, 4);
        } else if (iwram_3001f34->f4c == 0 || (gKeyPress & 2) != 0) {
            _PlaySound(0x71);
            ret = -1;
            break;
        }
        if (c != 0) {
            if ((gKeyRepeat & 0x80) != 0) {
                _PlaySound(0x6f);
                row++;
                if (row == 5 || top + row == c)
                    row = 0;
                keep = row;
            } else if ((gKeyRepeat & 0x40) != 0) {
                _PlaySound(0x6f);
                row--;
                if (row < 0) {
                    if (top == (c - 1) / 5 * 5)
                        row = c - top - 1;
                    else
                        row = 4;
                }
                keep = row;
            } else if ((gKeyRepeat & 0x10) != 0) {
                _PlaySound(0x6f);
                Func_800352c();
                if (top + 5 < c) {
                    top += 5;
                    row = keep;
                    if (top == (c - 1) / 5 * 5) {
                        row = c - top - 1;
                        if (row > keep)
                            row = keep;
                    }
                } else if (top != 0) {
                    row = keep;
                    top = 0;
                }
            } else if ((gKeyRepeat & 0x20) != 0) {
                _PlaySound(0x6f);
                Func_800352c();
                if (top != 0) {
                    row = keep;
                    top -= 5;
                } else {
                    top = (c - 1) / 5 * 5;
                    row = keep;
                    if (top != 0) {
                        row = c - top - 1;
                        if (row > keep)
                            row = keep;
                    }
                }
            }
        }
        WaitFrames(1);
    }
    CloseUIBox(boxA, 1);
    CloseUIBox(boxB, 1);
    WaitFrames(1);
    Func_8003f3c(gfx);
    gr = gfxs;
    for (i4 = 4; i4 >= 0; i4--)
        Func_8003f3c(*gr++);
    WaitFrames(1);
    return ret;
}
