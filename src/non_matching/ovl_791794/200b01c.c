/* OvlFunc_897_200b01c -- asm/overlays/rom_791794/ovl_30_c_c_c_c_c_a.s
 * NON-MATCHING, 299 encodings of 360.  NOT a distance (ref 752 bytes / 360 encodings against ours 744 / 356, four short).
 * READ `--align`: 223 of 377.  All 15 relocations exact and in order.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_791794/200b01c.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_c_c_c_a.s --func OvlFunc_897_200b01c
 * Distance while iterating:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/ovl_791794/200b01c.c --ref asm/overlays/rom_791794/ovl_30_c_c_c_c_c_a.s --align
 * 354 instructions, ONE function, no data section (grep -ci func_start = 1):
 * whole-file conversion.  batch 294, brief E, target 2.
 *
 * PARKED.  --align 223 of 377.  objcmp: 299 of 360 differ (ours 356), SIZE ref
 * 752 / ours 744 -- FOUR ENCODINGS SHORT, so the 299 is not a true distance.
 * All 15 RELOCATIONS are present, in the same order, with the same symbols, and
 * the five pool words (.L3a0c .L3a2a .L3a68 .L3a90 .L3a48) are in the ROM's own
 * pool ORDER including the gap before .L3a48.  The residue is entirely register
 * allocation and spill-slot numbering.
 * NO SHIMS: zero `register ... __asm__` declarations, zero `__asm__(".equ ...")`
 * lines, no volatile, no barriers, default flags.
 * `_udivsi3_RAM` against our `__udivsi3` is NOT a difference:
 * overlays/rom_791794/overlay.ld:71 already carries `__udivsi3 = _udivsi3_RAM;`
 * and objcmp says so itself.
 *
 * BLOCKER: two allocation facts, both named below -- (i) the order in which
 * loop.c's strength reduction creates its five induction-variable pseudos, which
 * decides four spill-slot offsets via reload1.c:801, and (ii) one adjacent
 * register-pair swap, r9 <-> r11.
 *
 * ================= THE READING =================
 *
 * A particle/oscillator updater.  `galloc_ewram(0x21, 0x194)` returns a block
 * laid out as TEN 0x28-byte elements followed by a u16 live count at +0x190:
 * 10 * 0x28 = 0x190, + 2 = 0x192, rounded to the 0x194 the ROM asks for.  The
 * allocation size IS the layout evidence.  The count is RE-READ from memory at
 * the bottom of every iteration (`ldr r4,[sp,#0x40] / add r3,r4,#0x190 /
 * ldrh r3,[r3]`), so the loop bound is a genuine memory operand and the loop is
 * `while (i != w->n)` -- `beq` on the bottom test fixes the `!=`; a `<` would
 * emit `bge`.  The entry `cmp r3,#0 / bne` is jump.c's duplicate_loop_exit_test
 * copy with cse folding `0 != n`.
 *
 * ELEMENT (0x28): +0x00 actor pointer, +0x04/08/0c an accumulated position
 * triple, +0x10/14/18 a phase triple, +0x1c a magnitude with +0x20 its delta,
 * +0x24 a stagger countdown byte, +0x25 a burst countdown byte, +0x26 pad.
 *
 * FIVE TABLES, spacing fixes the shapes: .L3a2a - .L3a0c = .L3a48 - .L3a2a =
 * 0x1e = 10 * 3, so those three are three-per-element (u8, u8, and s8 -- the
 * third is read with `ldrsb`); .L3a90 - .L3a68 = 0x28 = 10 * 4, so those two are
 * int[10] indexed by the element number.
 *
 * FRAME 0x44 = seventeen spill slots, no arrays, nothing address-taken.
 * Reload assigns slots in ASCENDING PSEUDO NUMBER order (reload1.c:801,
 * `for (i = LAST_VIRTUAL_REGISTER + 1; i < max_regno; i++) alter_reg (i, -1);`)
 * and ARM's frame grows downward, so the FIRST-declared scalar gets the HIGHEST
 * offset.  Declaring w, c, i, a, b, cc, d, e, L, f, g, h in that order puts all
 * twelve source slots on the ROM's offsets 0x40 down to 0x14 -- verified, every
 * one.  (Same lever as the Field_Move_Target park in this batch, where it was
 * worth 211 -> 203; it is recorded as inert in docs/elevation.md and it is not.)
 *
 * THREE ROM SHAPES THAT ARE NOT SOURCE CONSTRUCTS:
 *  - `push {r5,r6,r7,lr}` with r4 unsaved: Makefile:131 passes -fcall-used-r4
 *    globally.  Not a per-file flag to go looking for.
 *  - `str r1, [r5,#8] / [0xc] / [0x10]` inside the burst-start block stores ZERO
 *    out of the register that held the burst counter: cse's record_jump_cond
 *    knows it is 0 on the `bne`'s fall-through.  Plain `= 0` in the C.
 *  - the stagger byte is stored to its slot TWICE at the top of the body (`str
 *    r3` with the loaded value, then again with the decremented one).  That is
 *    two source assignments to one spilled `unsigned char`; reload does not do
 *    dead-store elimination on its own slots.  `L = c->f24; L = L - 1;`.
 *
 * ============ FOUR LOAD-BEARING CONSTRUCTS, each singly measured ============
 *
 * 1. THE THREE DIVISIONS ASSIGN TO NEW VARIABLES, NOT BACK TO THE MULTIPLIER.
 *    [359 -> 252 aligned, and it took the length from 368 to the ROM's 377]
 *    The ROM spells each as
 *        cmp rX,#0 ; beq L ; ...udiv... ; mov rX,r0 ; b L' ; L: mov rX,#0 ; L':
 *    -- an explicit `else 0` arm, six instructions across the three sites that
 *    do not exist if the destination is the SAME variable being tested (then the
 *    else arm is a provable no-op and gcc drops the `b` and the `mov`).  So the
 *    source is `p = tbl[i][j] * Random() >> 16;` and then `if (p) u = (p << 16)
 *    / 1000; else u = 0;` with u distinct from p.  gcc then coalesces u onto p's
 *    register anyway, which is why the ROM looks like one variable.
 *    THIS WAS THE WHOLE 9-INSTRUCTION SHORTFALL of the first candidate.
 *
 * 2. THE BURST COUNTER IS AN `unsigned char`, NOT AN int.  [247 -> 223]  The
 *    ROM's decrement is `mov r3,r11 / add r3,#0xff / lsl r3,#0x18 / lsr r3,#0x18
 *    / mov r11,r3` -- the lsl/lsr pair is the QImode truncation, and an `int`
 *    gives `mov r3,#1 / neg r3,r3 / add r11,r3` instead (three instructions, no
 *    truncation, and it cannot be recovered by casting the result).  Note the
 *    direction: for QImode ARM's PROMOTE_MODE promotes UNSIGNED, so `unsigned
 *    char` is right here -- the opposite of the HImode case in batch 293's
 *    finding 5, where `unsigned short` still gets `ldrsh`.
 *
 * 3. THE SIN/COS MULTIPLIES PUT THE ACCUMULATOR ON THE LEFT.  [252 -> 247]
 *    `__sin(a * L3a2a[i][0])` reaches the ROM's `mov r0,r3 / mul r0,r1`;
 *    `__sin(L3a2a[i][0] * a)` does not.  Measured both ways at all three sites.
 *    The DIRECTION IS OPPOSITE to the Random multiplies in the same function,
 *    where `tbl[i][j] * Random()` is right and the swap is inert -- so this is
 *    the batch-293 finding again: the mul lever is a measurement, and its
 *    procedure does not even hold within one function.
 *
 * 4. THE COUNT IS RE-READ, so `w` and `c` must be separate variables.  Folding
 *    them (using `w->e[i]` for everything) loses the `str r0,[sp,#0x3c]` at the
 *    top and the whole slot layout shifts by one.
 *
 * ============ THE RESIDUE, ITEMISED ============
 *
 * (a) FOUR SPILL SLOTS IN THE WRONG ORDER -- 50 of the ~223 differing lines
 *     mention one of them.  The five loop-created induction pseudos are
 *         q1 = &c->f24, q2 = &c->f25, k = 3*i, t1 = &.L3a0c[3i+1],
 *         t2 = &.L3a2a[3i]
 *     and the ROM gives them 0x10, 0xc, 8, 4, 0 while we give them 8, 4, 0x10,
 *     0xc, 0.  Since reload1.c:801 hands out slots in pseudo order, this is
 *     purely the ORDER loop.c created them.  loop.c:5494 PREPENDS each new giv
 *     (`v->next_iv = bl->giv; bl->giv = v;`) and the emission loop at loop.c:4790
 *     walks that list, so the LAST-discovered giv gets the LOWEST pseudo number
 *     and therefore the HIGHEST slot.  The ROM's order implies q1 was discovered
 *     last and t2 first, which does not follow from any statement order I could
 *     construct -- and note the scan does not start at the body's first insn
 *     (loop.c sets loop->scan_start to the entry jump's target and defers the
 *     loop->top..scan_start range), so source order is not the whole story.
 *     MEASURED INERT for this: writing the three-wide tables as `tbl[][3]` with
 *     `tbl[i][0..2]` versus 1-D with `tbl[i*3 + 0..2]` (byte-identical output,
 *     both 223); spelling the two counter bytes as a `cnt[2]` member array
 *     (byte-identical); moving the burst counter's declaration to three
 *     different points in the list (byte-identical).
 *
 * (b) ONE ADJACENT REGISTER-PAIR SWAP.  Every other register agrees: r5 the
 *     actor pointer, r6 and r7 the first two divided values, r8 the third
 *     multiplier, r10 the element+8 induction pointer.  The ROM puts the second
 *     divided value in r9 and the burst counter in r11; we have them the other
 *     way round.  The call-crossing part of REG_ALLOC_ORDER (arm.h:989) is
 *     5, 6, 7, 8, 10, 9, 11, so these two are ranks 6 and 7 -- adjacent in
 *     allocno_compare, and the flip is one comparison of refs/live_length.
 *     Read those counts from a -dl dump, never from the source: flow.c:4948
 *     weights every reference by loop_depth + 1 and this whole body is inside
 *     the loop.
 *
 * (c) TWO RELOAD-INHERITANCE INSTRUCTIONS we do not emit, both cases of the ROM
 *     reloading a spilled value that we still have in a register:
 *       - `ldr r4, =0x1999` before `str r4,[sp,#0x28]`, where we reuse the r3
 *         that the immediately preceding `cmp` loaded with the same constant;
 *       - `ldr r1, [sp,#0x38]` before the bottom `cmp r1,r3`, where we still
 *         have the just-incremented counter in r4.
 *     Both are decided after reload and neither has a source spelling.  They are
 *     exactly the four-encoding shortfall objcmp reports.
 *
 * -- worked in scratch_elev/b294/E
 */

struct Part {
    int *s;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    unsigned char f24;
    unsigned char f25;
    unsigned char pad[2];
};

struct Sys {
    struct Part e[10];
    unsigned short n;
};

extern void *__galloc_ewram(int tag, int size);
extern unsigned int __Random(void);
extern int __sin(int a);
extern int __cos(int a);
extern unsigned char L3a0c[][3] __asm__(".L3a0c");
extern unsigned char L3a2a[][3] __asm__(".L3a2a");
extern signed char L3a48[][3] __asm__(".L3a48");
extern int L3a68[] __asm__(".L3a68");
extern int L3a90[] __asm__(".L3a90");

void OvlFunc_897_200b01c(void)
{
    struct Sys *w;
    struct Part *c;
    int i;
    int a;
    int b;
    int cc;
    int d;
    int e;
    unsigned char L;
    int f;
    int g;
    int h;
    int *s;
    unsigned char m;
    unsigned int p1;
    unsigned int p2;
    unsigned int p3;
    unsigned int u;
    unsigned int v;
    unsigned int t;
    int sx;
    int sy;
    int cz;

    w = __galloc_ewram(0x21, 0x194);
    c = w->e;
    i = 0;
    while (i != w->n) {
        s = c->s;
        a = c->f10;
        b = c->f14;
        cc = c->f18;
        d = c->f1c;
        e = c->f20;
        m = c->f25;
        f = c->f04;
        g = c->f08;
        h = c->f0c;
        L = c->f24;
        L = L - 1;
        if (L != 0)
            goto out;
        L = 3;
        if (m == 0) {
            d += e;
            if (d >= L3a68[i]) {
                e = -L3a90[i];
            } else if (d <= 0x1999) {
                d = 0x1999;
                e = L3a90[i];
                f = s[2];
                g = s[3];
                h = s[4];
                s[2] = 0;
                s[3] = 0;
                s[4] = 0;
                m = 0x18;
            }
            s[6] = d;
            s[7] = d;
        }
        p1 = L3a0c[i][0] * __Random() >> 16;
        p2 = L3a0c[i][1] * __Random() >> 16;
        p3 = L3a0c[i][2] * __Random() >> 16;
        if (p1 != 0)
            u = (p1 << 16) / 1000;
        else
            u = 0;
        if (p2 != 0)
            v = (p2 << 16) / 1000;
        else
            v = 0;
        if (p3 != 0)
            t = (p3 << 16) / 1000;
        else
            t = 0;
        if (L3a48[i][0] == 1) {
            a += u;
        } else {
            a -= u;
            if (L3a48[i][0] != -1)
                a = 0;
        }
        if (L3a48[i][1] == 1) {
            b += v;
        } else {
            b -= v;
            if (L3a48[i][1] != -1)
                b = 0;
        }
        if (L3a48[i][2] == 1) {
            cc += t;
        } else {
            cc -= t;
            if (L3a48[i][2] != -1)
                cc = 0;
        }
        sx = __sin(a * L3a2a[i][0]) * 2;
        sy = __sin(b * L3a2a[i][1]) * 2;
        cz = __cos(cc * L3a2a[i][2]) * 2;
        if (m != 0) {
            f += sx;
            g += sy;
            h += cz;
            m = m - 1;
            if (m == 0) {
                s[2] = f;
                s[14] = f;
                if (v != 0) {
                    s[3] = g;
                    s[15] = g;
                }
                s[4] = h;
                s[16] = h;
            }
        } else {
            s[2] += sx;
            s[14] = s[2];
            if (v != 0) {
                s[3] += sy;
                s[15] = s[3];
            }
            s[4] += cz;
            s[16] = s[4];
        }
      out:
        c->f10 = a;
        c->f14 = b;
        c->f18 = cc;
        c->f1c = d;
        c->f20 = e;
        c->f25 = m;
        c->f04 = f;
        c->f08 = g;
        c->f0c = h;
        c->f24 = L;
        c++;
        i++;
    }
}
