/* Func_801be80 (0x0801be80) -- NON-MATCHING, 251 of 274 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/801be80.c asm/rom_15000/rom_1aeec_a_a_c_c.s \
 *       --func Func_801be80
 *
 * SIZE 580 == 580 AND INSTRUCTIONS 274 == 274, so 251 IS A TRUE DISTANCE.
 * tools/aligncmp.py: aligned-equal 147 of 274 (53.6%), 155 differing in 60 hunks.
 * ZERO SHIMS -- no pins, no barriers, no volatile (tools/shimcount.py: 0).
 * The PROLOGUE IS BYTE-EXACT, including all three high-register saves.
 *
 * SPLIT REQUIRED.  asm/rom_15000/rom_1aeec_a_a_c_c.s holds FIVE functions --
 * Func_801ba68, UploadIcon, Func_801bcd4, Func_801bd98 and this one, which is the
 * LAST.  So it is a two-part text-only split:
 *     python3 tools/split_s.py asm/rom_15000/rom_1aeec_a_a_c_c.s Func_801be80
 * leaves the first four in rom_1aeec_a_a_c_c_a.s and this one in
 * rom_1aeec_a_a_c_c_b.s, with no _c part.  tools/datacheck.py reports NO required
 * data exports, so nothing has to be made `.global`.  NOTE that landing
 * src/non_matching/rom_15000/801ba68.c (the same file's first function) needs the
 * SAME split from the other end -- coordinate the two, or the second one to land
 * will find the linker script already rewritten.
 *
 * ================== WHERE THE STRUCTS CAME FROM -- READ THIS FIRST ============
 * `struct P` and `struct W` are NOT invented here.  They are lifted from
 * src/non_matching/rom_15000/801ba68.c, three functions up in the same original
 * file, and extended.  That park's own headline finding carried over intact:
 * typing the parameter as `struct W *` with MEMBER ARRAYS rather than
 * `unsigned char *` plus casts is what produces the ROM's
 * `add r3, r2, r1 / ldrh r3, [base, r3]` -- base in the register, whole
 * displacement in the index.  Do not flatten these back to byte pointers.
 *
 * WHAT THIS PARK ADDS to those two definitions, all of it evidenced by the ROM's
 * pool constants (see the layout check below):
 *   struct W  + f2e2 (0x2e2), f2fa (0x2fa), head2 (0x34c), f39e (0x39e),
 *               f3a2 (0x3a2), a3a4[5] (0x3a4), a3ae[16] (0x3ae)
 *   struct P  + f8, fe, f16, f1c, f1e, f20 (they were pad bytes there), and
 *               `struct O o28` at 0x28
 *
 * THE LAYOUT CHECK, which docs/elevation.md insists on after any struct change
 * ("read the pool constants and check they still name the ROM's offsets", the
 * Func_801b664 trap).  Our object's pool holds 738, 762, 922, 926, 930 =
 * 0x2e2, 0x2fa, 0x39a, 0x39e, 0x3a2 -- every one of them a displacement the ROM
 * pools too, and not one shifted by 4.  The layout is confirmed, not assumed.
 * Note `a3ae` is reached as `(w + 2)[0x3ac + 2*i]`: 0x3ac is 0xeb << 2 and so
 * buildable, 0x3ae is not, so gcc splits the 2 onto the base.  That is gcc being
 * clever, not a wrong offset, and the ROM does the same thing.
 *
 * ================== THE BITFIELD BLOCK AT +0x28, WHICH WAS THE BLOCKER ========
 * Batch 298 characterised this function and did not attempt it, on the honest
 * ground that the `+0x28` block is bitfield assignments with no struct in the tree
 * to borrow.  That was right about the diagnosis and the struct IS the work, but
 * it is a ten-line struct and it lands:
 *
 *   struct O {  ... unsigned char e0:2, e2:2, e4:1, e5:1, e6:2;  (byte 5)
 *                   unsigned char pad6;
 *                   unsigned char g0:6, g6:2;                    (byte 7)
 *                   unsigned short tile:10, prio:2, pal:4;       (halfword at 8) }
 *
 * and the seven assignments are `o->e2 = 0; o->e5 = 0; o->e4 = 0; o->e6 = 0;
 * o->g6 = 1; o->pal = 0; o->tile = q->fe;`.
 *
 * WHAT CONFIRMS IT, and it is the reusable part:
 *  - THE FOUR ands ON ONE BYTE.  The ROM has ONE `ldrb [r0,#5]`, four `and`s, and
 *    ONE `strb` -- gcc's combine merges four consecutive store_bit_field
 *    read-modify-writes on the same byte into a single one.  The mask sequence is
 *    ~0xc, ~0x20, ~0x10, then 0x3f, in that order, which fixes the SOURCE ORDER of
 *    the four assignments: bits 2-3, bit 5, bit 4, bits 6-7.
 *  - THE FORM OF THE COMPLEMENTS, per the brief's refinement.  Three come out
 *    `mov r2,#0xd / neg r2,r2` (-13) and `mov r3,#0x21 / neg r3,r3 / add r3,#0x10`
 *    (-33 then -17, CHAINED), while the fourth is a plain `mov r4,#0x3f`.  Same
 *    struct, same statement group, one positive and three negative masks twenty
 *    bytes apart -- exactly the "the same function can want one of each" case.
 *    Nothing had to be spelled to get this; it falls out of the bitfield widths.
 *  - `o->pal = 0` narrows to a BYTE operation on byte 9 (`mov r3,#0xf / and /
 *    strb [r0,#9]`) because bits 12-15 sit wholly inside that byte, while
 *    `o->tile = q->fe` on the 10-bit field spanning bytes 8-9 stays HImode.  Two
 *    different widths on one halfword, both the ROM's.
 *  - `o->tile = q->fe` is what POOLS 0xfffffc00.  store_bit_field builds the
 *    inverse mask in SImode, where -1024 needs a pool word; written by hand as
 *    `*(u16 *)(s + 8) = (*(u16 *)(s + 8) & 0xfffffc00) | (q->fe & 0x3ff)` gcc
 *    narrows to HImode and emits `mov r2,#0xfc / lsl r2,#8` instead, which is one
 *    instruction longer and has no pool word.  The POOLED MASK IS THE TELL FOR A
 *    BITFIELD, and it is a cleaner tell than the `neg` form because a plain
 *    `& ~0xc` on a byte ALSO produces `neg` (measured separately on Func_800c62c
 *    this batch).
 *
 * ================== THE OTHER TWO THINGS THAT CLOSED IT ======================
 * First draft with plain byte masks: 267 instructions / 564 bytes, 45.3% aligned.
 *
 * 1. NAME `&w->f39e` AS A POINTER -- worth 5 aligned-equal points and the entire
 *    prologue.  The ROM keeps it in r10 for the whole function and reads through
 *    it at THREE sites (the f39c + f39e sum, Func_801b9a8's argument, and the
 *    re-read inside the node walk) while REBUILDING it from the pool for the two
 *    later ones (`w->a3ae[i] = w->f39e` and `w->f39e |= 0x80`).  Naming it for all
 *    five is 276/584; naming it for the right three is 274/580.
 *    This is the reachable half of the src/non_matching/rom_c0/8006408.c rule --
 *    "a local only earns a register when it holds something gcc cannot recompute
 *    for free".  `&w->f39e` is COMPUTED (w + 0x39e), so it earns r10; the same
 *    park's unreachable case was `&global`, a compile-time constant, which does
 *    not.  It also lifts the function from two saved high registers to the ROM's
 *    three, which is what makes the prologue exact.
 * 2. INT CARRIERS FOR THE TWO FIELDS COPIED TWICE -- this is what fixed both the
 *    size and the count.  `q->f10 = p->f10; q->f12 = p->f12; q->f18 = p->f10;`
 *    RELOADS p->f10, because the store to `q->f12` may alias it (both are
 *    `struct P *`).  The ROM does not reload.  Written as
 *        a = p->f10; q->f10 = a; q->f18 = a;
 *        c = p->f12; q->f12 = c; q->f1a = c;
 *    the reload goes away.  276/584 -> 274/580, both exact.
 *    Merely reordering so the two f10 stores are adjacent does NOT work (276/584):
 *    the intervening store to `q->f10` itself is enough to force the reload.  The
 *    carrier has to take the load out of the aliasing window.
 *
 * Also load-bearing:
 *  - `ret` is computed FIRST, before any call, and returned from r9 at the end.
 *  - the node walk re-reads `*n39e` every iteration (`while (*n39e != i)`), which
 *    is why it cannot be hoisted into a local.
 *  - `((short)p->f10 - (short)q->f10) >> 1` with explicit `short` casts -- the ROM
 *    uses `ldrsh` with a REGISTER offset at both sites, which is the only way Thumb
 *    can do a signed halfword load.
 *  - the three-way tail: empty list -> `w->head2 = q; q->f0 = 0;`, otherwise walk
 *    to the last node and link; `q->f4 = 0` is the shared merge, and the ROM
 *    reuses the same zero register for `q->f0 = 0` and `q->f4 = 0`.
 *
 * ================== THE RESIDUE: TWO REGISTER NAMES ==========================
 * Everything structural is exact -- size, instruction count, the prologue and
 * epilogue, the branch layout, every pool constant, the struct layout.  What is
 * left is a tie-break between two pseudos:
 *      ROM   r5 = w (the parameter)   r6 = p   r7 = q   r8 = i   r9 = ret   r10 = &f39e
 *      ours  r5 = q                   r6 = p   r7 = w   r8 = i   sl = ret   r9  = &f39e
 * `w` and `q` are SWAPPED, and that one swap accounts for most of the 251.  With
 * only r5/r6/r7 callee-saved here (r4 is call-used via the Makefile's
 * -fcall-used-r4), gcc ranks `q` above the parameter and the ROM ranks the
 * parameter above `q`.
 *
 * MEASURED, all three at 274/580 and 251 differing, none of them moving it:
 *   reading w->f39c before building the n39e pointer (the ROM's order)   52.2%
 *   SPLITTING `q` into the list walker and the Func_801a910 result, so
 *     its live range is two short ones instead of one long one           53.6%
 *   both together                                                       52.2%
 * This is the REG_ALLOC_ORDER class (HANDOFF batch 295) in its purest form: not a
 * different ordering but a different PRIORITY tie-break between a parameter and a
 * local of similar use count.
 *
 * NEXT: this is the closest of the five in batch 299 and the only one with no
 * shims at all.  If anything is going to fall to the REG_ALLOC_ORDER rebuild
 * experiment, it is this one -- re-screen it first.
 */
struct O {
    unsigned char pad0[5];
    unsigned char e0 : 2;
    unsigned char e2 : 2;
    unsigned char e4 : 1;
    unsigned char e5 : 1;
    unsigned char e6 : 2;
    unsigned char pad6;
    unsigned char g0 : 6;
    unsigned char g6 : 2;
    unsigned short tile : 10;
    unsigned short prio : 2;
    unsigned short pal : 4;
};

struct P {
    struct P *f0;
    struct P *f4;
    unsigned short f8;
    unsigned short fa;
    unsigned short fc;
    unsigned short fe;
    unsigned short f10;
    unsigned short f12;
    unsigned short f14;
    unsigned short f16;
    unsigned short f18;
    unsigned short f1a;
    unsigned short f1c;
    unsigned short f1e;
    unsigned short f20;
    unsigned short f22;
    unsigned short f24;
    unsigned short f26;
    struct O o28;
};

struct W {
    unsigned char pad000[0xa];
    unsigned short fa;
    unsigned char pad00c[0x3e - 0xc];
    unsigned short f3e;
    unsigned char pad040[0x2e2 - 0x40];
    unsigned short f2e2;
    unsigned char pad2e4[0x2fa - 0x2e4];
    unsigned short f2fa;
    unsigned char pad2fc[0x348 - 0x2fc];
    struct P *head;
    struct P *head2;
    unsigned char pad350[0x39a - 0x350];
    unsigned short f39a;
    unsigned short f39c;
    unsigned short f39e;
    unsigned char pad3a0[2];
    unsigned short f3a2;
    unsigned short a3a4[5];
    unsigned short a3ae[16];
};

extern void Func_801ba34(struct W *w);
extern void Func_801b9a8(struct W *w, int n);
extern void Func_801c21c(void);
extern void WaitFrames(int n);
extern void Func_8003f3c(int id);
extern struct P *Func_801a910(int a);

int Func_801be80(struct W *w)
{
    struct P *p;
    struct P *q;
    struct P *t;
    struct O *o;
    unsigned short *n39e;
    int a;
    int c;
    int i;
    int ret;

    n39e = &w->f39e;
    ret = w->f39c + *n39e;
    i = 0;
    Func_801ba34(w);
    Func_801b9a8(w, *n39e);
    w->f3a2 = 0x21;
    WaitFrames(1);
    w->fa = 0;
    w->f3e = 0;
    w->f2e2 = 0;
    w->f2fa = 0;
    Func_801c21c();
    p = w->head;
    if (p != 0 && *n39e != 0) {
        do {
            p = p->f4;
            i++;
            if (p == 0)
                break;
        } while (*n39e != i);
    }
    p->f1c = p->f10;
    p->f1e = p->f12;
    q = w->head;
    while (q != 0) {
        if (q != p) {
            q->f18 = p->f10;
            q->f14 = ((short)p->f10 - (short)q->f10) >> 1;
        }
        q = q->f4;
    }
    WaitFrames(2);
    q = w->head;
    if (q != 0) {
        i = 0;
        do {
            if (q != p) {
                Func_8003f3c(q->fc);
                q->fa = i;
            }
            q = q->f4;
        } while (q != 0);
    }
    w->head = p;
    p->f0 = 0;
    p->f4 = 0;
    p->f18 = 4;
    q = w->head2;
    i = 0;
    while (q != 0) {
        p->f18 = p->f18 + 0x10;
        q = q->f4;
        i++;
    }
    w->a3a4[i] = w->f39c;
    w->a3ae[i] = *n39e;
    p->f14 = ((short)p->f18 - (short)p->f10) >> 1;
    i = 0;
    w->f39a = i;
    w->f39e = w->f39e | 0x80;
    WaitFrames(2);
    q = Func_801a910(1);
    q->fa = p->fa;
    q->f20 = p->f20;
    q->f8 = p->f8;
    q->fc = p->fc;
    q->fe = p->fe;
    a = p->f10;
    q->f10 = a;
    q->f18 = a;
    c = p->f12;
    q->f12 = c;
    q->f1a = c;
    q->f1c = p->f1c;
    q->f1e = p->f1e;
    q->f14 = i;
    q->f16 = i;
    q->f22 = 0x100;
    q->f26 = 0x100;
    o = &q->o28;
    o->e2 = 0;
    o->e5 = 0;
    o->e4 = 0;
    o->e6 = 0;
    o->g6 = 1;
    o->pal = 0;
    o->tile = q->fe;
    p->fa = i;
    w->head = 0;
    t = w->head2;
    if (t != 0) {
        while (t->f4 != 0)
            t = t->f4;
        t->f4 = q;
        q->f0 = t;
    } else {
        w->head2 = q;
        q->f0 = 0;
    }
    q->f4 = 0;
    return ret;
}
