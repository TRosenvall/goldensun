/* Func_80a5788 -- the party-member equip/select loop, 0x080a5788,
 * 464 ROM instructions.
 * PARKED.
 * NON-MATCHING, 172 of 476 encodings differ.
 *
 * SIZE IS 1040 AGAINST 1036 and the instruction count 478 against 476, so 172 is
 * NOT a true distance -- objcmp's index-by-index count saturates.  The honest
 * figure is tools/aligncmp.py: 454 of 476 aligned-equal (95.4%), 25 differing in
 * 20 hunks, and 14 of those 20 hunks are pool/branch offsets behind the two
 * defects below.  The relocation SYMBOL SEQUENCE is IDENTICAL (36 entries); 16
 * offsets differ, all by the same 4-byte shift.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a5788.c \
 *     asm/rom_a1000/rom_a5534_c_a_a.s --func Func_80a5788
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a5788.c \
 *     asm/rom_a1000/rom_a5534_c_a_a.s Func_80a5788 -v
 *
 * THE SPLIT.  asm/rom_a1000/rom_a5534_c_a_a.s holds SIX functions -- Func_80a5788
 * (FIRST), Func_80a5b94, Func_80a5cc0, Func_80a5fe0, Func_80a602c, Func_80a60d4 --
 * and no .section .data; tools/datacheck.py is silent, so the split is pure text
 * and needs no `.global`.  Three of the six are already parked from this file
 * (src/non_matching/rom_a1000/80a5b94.c, 80a5cc0.c, 80a60d4.c).
 *
 * SHIMS: ZERO (tools/shimcount.py reports none).
 *
 * THE FINDING, AND IT TRANSFERS: THE OUTER LOOP IS A PLAIN `while (A && B)`, NOT
 * `while (1) { if (A && B) {...} else break; }`.  This was worth 201 -> 453 of 476
 * in one step, and it is a BLOCK-ORDER fact you can read straight off objcmp's
 * relocation SEQUENCE without compiling anything:
 *
 *     rom   Func_80a10d0, _PlaySound, _PlaySound, <iwram pool>, Func_80a1a40,
 *           ... body ... , __modsi3, _GetUnit, Func_80a3ddc, <key pools>,
 *           _GetFlag, _GetFlag, _GetUnit, Func_80a3ddc, Func_80a3e28,
 *           Func_80a5578, Func_80a1a40, _Func_80164d4, ...
 *     ours  Func_80a10d0, _GetFlag, _GetUnit, Func_80a3ddc, Func_80a3e28,
 *           Func_80a5578, Func_80a1a40, <iwram pool>, Func_80a1a40, ... (break form)
 *
 * The ROM emits the OUTER TEST AND THE WHOLE SETUP BLOCK LAST, after the inner
 * loop and its test, entering with a `b` to it -- that is stmt.c's `while` loop
 * rotation.  The `while (1) { ... else break; }` spelling (which the near-sibling
 * src/non_matching/rom_a1000/80a96d8.c does need) puts the setup first and moves
 * every block in the function.  SO: READ THE RELOCATION SEQUENCE FIRST ON ANY
 * NESTED-LOOP FUNCTION; it dates the loop form before a single instruction is
 * compared.
 *
 * WHAT ELSE CLOSED 454 OF 476:
 *
 * 1. THE SIBLING SHAPE.  src/non_matching/rom_a1000/80a96d8.c and 80a90bc.c are
 *    the same menu skeleton -- Func_80a10d0 to open the box, `int ctx[7]` with
 *    Func_80a1fd4(0, ctx[5], 5, &ctx[4], &ctx[2]) as the cursor mover, a redraw
 *    flag and a full-redraw flag, `ret`/`done`, and the gKeyPress 1/2 and
 *    gKeyRepeat 0x100/0x200 arms.  Starting from that skeleton rather than from
 *    the disassembly is most of this file.
 *
 * 2. EVERY LOOP INDEX IS A `unsigned char`.  The three counted loops (the two
 *    _Sprite_SetAnim sweeps over st->f219 and the four-entry 0x144 fill) all show
 *    `add r3,rN,#1 / lsl #24 / lsr rN,#24` and an UNSIGNED branch (`bcc`/`bls`).
 *    An `int i` gives `blt` and no truncation.  Worth 169 -> 201.
 *
 * 3. THE FRAME PUBLISHES THE DECLARATION ORDER.  0x58 = 8 bytes of outgoing args
 *    (Func_80a10d0 takes six) + three compiler temps at 0x08/0x0c/0x10 + five
 *    scalars + `int ctx[7]` at 0x28 + a 20-byte buffer at 0x44.  Highest slot
 *    first: buf, ctx, box(0x24), ret(0x20), full(0x1c), done(0x18), prev(0x14).
 *    Any other order, or any extra long-lived local, spills `redraw` and rotates
 *    `st` out of r7 into r8 -- which costs THREE extra slots and a 0x64 frame,
 *    because almost every `st` reference uses the Thumb register-offset form and
 *    so needs `st` in a LOW register.  Getting the loop form right (the finding
 *    above) is what fixed the frame.
 *
 * 4. THE TWO Func_80a1a40 CALLS READ DIFFERENT ctx SLOTS: ctx[6] in the setup
 *    block, ctx[4] at the top of the inner loop body.
 *
 * 5. `ret` IS SPILLED AND RE-READ NARROWLY.  `st->f25c = ret;` compiles to
 *    `add r4,sp,#0x20 / ldrb r4,[r4] / strb` and `st->h178[which] = ret;` to
 *    `add r2,sp,#0x20 / ldrh r2,[r2] / strh` -- gcc narrows the load of its own
 *    spill slot to the width of the store.  That falls out of a plain `int ret;`.
 *
 * 6. THE `-1` IS SHARED.  `mov r5,#1 / neg r5,r5` built for the `r == -1` test is
 *    reused for `ret = -1` in the gKeyPress&2 arm, exactly as in 80a96d8.
 *
 * 7. `st->arr21a[which]` IS ADDRESSED AS `(st+2) + (0x218 + which)`.  gcc commons
 *    the 0x218 (= 0x86<<2) it already needs for `st->f218` and pushes the
 *    remaining +2 into the base, which it keeps in a spill slot for the whole
 *    function.  Nothing had to be written for it; it is why [sp,#0xc] exists.
 *
 * 8. THE 0x1e AND 0x1a OF THE 0x144 FILL ARE POOLED, not `mov`-built, because the
 *    stores are HImode -- the shiftable/HImode rule from
 *    src/non_matching/rom_a1000/80a5cc0.c's blocker 2 predicts this exactly.
 *
 * 9. gKeyPress AND gKeyRepeat ARE `volatile`: the ROM keeps the ADDRESS in r1 and
 *    reloads the word for the second test of each pair.
 *
 * ============================================================
 * THE BLOCKERS.  Two, both local-alloc ties in the same four instructions.
 *
 * BLOCKER 1, 2 encodings: WHICH REGISTER THE `st->arr21a[which]` BYTE LANDS IN
 * BEFORE Func_80a3ef0.  Pass: local-alloc / reload coalescing.
 *
 *     rom   ldrb r3,[r2,r3] / ldr r1,[r1,#0x18] / mov r0,r3 / mov r2,#0 / bl
 *     ours  ldrb r0,[r2,r3] / mov r3,sl / ldr r1,[r3,#0x18] / ... / bl
 *
 * Both bases agree (r2 = st+2, r3 = 0x218+which); the ROM loads into r3 and copies,
 * ours loads straight into the argument register and is ONE INSTRUCTION SHORTER.
 * THIS FILE buys the copy by giving the value the local `n` -- the same `n` that
 * later holds `st->f219` for the `% n` -- which is the only spelling found that
 * keeps the copy: a local used ONLY here is deleted again (measured, byte
 * identical to the no-local version).  But sharing `n` costs a NEW copy at the
 * __modsi3 site (`adds r1,r3,#0`), so the trade is +2 net:
 *     a5788_v6.c  (no local)   474 instructions, 1032 bytes, 352 of 476, 453 aligned
 *     a5788_a1.c  (THIS FILE)  478 instructions, 1040 bytes, 172 of 476, 454 aligned
 * Neither count matches.  Whoever picks this up needs the copy at the Func_80a3ef0
 * site WITHOUT the one at __modsi3 -- i.e. a register-allocation input, not a
 * spelling.  Measured inert or worse: a dedicated `int id` (deleted); `sel`
 * reused for it (409 aligned); writing the divisor inline so `n` has one use
 * (deleted again, back to v6 exactly).
 *
 * BLOCKER 2, 5 encodings: THE OBJECT-POINTER ROLE IN THE `st->list[ctx[6]] != 0`
 * BLOCK.  Pass: local-alloc.
 *
 *     rom   ldr r0,[r7,r3] / movs r2,#0 / strb r3,[r0,#5] / strh r2,[r0,#0xc] /
 *           strb r3,[r0,#0xf]
 *     ours  ldr r2,[r7,r3] / movs r1,#0 / strb r3,[r2,#5] / strh r1,[r2,#0xc] /
 *           strb r3,[r2,#0xf]
 *
 * A straight r0<->r2 / r2<->r1 pair swap with the same instruction count and the
 * same statement order (f5 = 9, then fc = 0, then ff = 0xfa -- the ROM's `mov
 * r3,#9 / mov r2,#0 / ... / mov r3,#0xfa` interleave is sched1).  Naming the
 * pointer in a local and writing the three stores through `st->objs[ctx[6]]`
 * directly both measure identically.
 *
 * ALSO INERT, RECORD SO NOBODY REPEATS IT: `short buf[10]` instead of `int buf[5]`;
 * `int box` instead of `unsigned int box`; declaring `st` first; dropping the
 * object-pointer local; writing the modulo without the `n` local.
 *
 * STRUCT NOTES.  iwram_3001f2c as this function sees it: 0x08 u32 (the last
 * selection), 0x1c + which an s8 cursor per column, 0x34 the box handle that
 * Func_80a10d0 fills, 0x44 a sprite the tail releases, 0x48 + 4*i the object
 * table, 0x114 + 4*i the party sprite table, 0x144 + 2*i four u16 highlight
 * codes (0x1e idle / 0x1a selected), 0x174 + 2*which and 0x178 + 2*which two u16
 * results, 0x1c8 the u16 item list, 0x208 the u16 party table, 0x218 u8 (the
 * Func_80a3ddc result), 0x219 u8 the party size, 0x21a + which u8 the unit id,
 * 0x21c an object pointer whose +5 byte is set to 0xd, 0x25c u8 = ret, 0x260 +
 * unit u8 the remembered slot.  0x218/0x219/0x21a and the 0x1c8/0x208 pair agree
 * with src/non_matching/rom_a1000/80a96d8.c's view of the same block.
 */
struct Obj {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned char pad06[6];
    unsigned short fc;
    unsigned char fe;
    unsigned char ff;
};

struct Unit {
    unsigned char pad00[0xd8];
    unsigned short items[15];
};

struct State {
    unsigned char pad00[8];
    unsigned int f08;
    unsigned char pad0c[0x1c - 0x0c];
    signed char s1c[8];
    unsigned char pad24[0x34 - 0x24];
    unsigned int f34;
    unsigned char pad38[0x44 - 0x38];
    struct Obj *f44;
    struct Obj *objs[32];
    unsigned char padc8[0x114 - 0xc8];
    struct Obj *spr114[12];
    unsigned short h144[24];
    unsigned short h174[2];
    unsigned short h178[40];
    unsigned short list[32];
    unsigned short arr[8];
    unsigned char f218;
    unsigned char f219;
    unsigned char arr21a[2];
    struct Obj *f21c;
    unsigned char pad220[0x25c - 0x220];
    unsigned char f25c;
    unsigned char pad25d[3];
    unsigned char f260[4];
};

extern struct State *iwram_3001f2c;
extern unsigned int iwram_3001e40;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern struct Unit *_GetUnit(int id);
extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
extern int Func_80a3ddc(struct Unit *unit, unsigned short *p, int f);
extern void Func_80a3e28(unsigned short *p, int f);
extern void Func_80a5578(int *ctx, int which);
extern void Func_80a1a40(int a, int b);
extern void Func_80a17c4(struct Obj *o);
extern void WaitFrames(int n);
extern void Func_80a56c8(unsigned int win, int a, int *ctx);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void Func_80a5614(unsigned int win, int *buf, int *ctx);
extern void Func_80a3ef0(int a, int b, int c);
extern void _Sprite_SetAnim(struct Obj *s, int n);
extern int _CanEquipItem(int unit, int item);
extern int Func_80a1fd4(int swap, int total, int cols, int *col, int *row);
extern void _PlaySound(int id);
extern int _GetFlag(int id);
extern void _Func_80164d4(unsigned int win, int a, int b, int c, int d);

int Func_80a5788(int which)
{
    int buf[5];
    int ctx[7];
    unsigned int box;
    int ret;
    int full;
    int done;
    int prev;
    struct State *st;
    struct Unit *u;
    struct Obj *o;
    int redraw;
    int r;
    unsigned char i;
    int sel;
    int n;

    st = iwram_3001f2c;
    ret = 0;
    prev = 0;
    st->f25c = ret;
    Func_80a10d0(&st->f34, 0xd, 3, 0x11, 0xe, 2);
    box = st->f34;
    done = 0;
    while (done == 0 && _GetFlag(0x150) == 0) {
        u = _GetUnit(st->arr21a[which]);
        st->f218 = Func_80a3ddc(u, st->list, 0);
        Func_80a3e28(st->list, 0);
        st->f21c->f5 = 0xd;
        Func_80a5578(ctx, which);
        Func_80a1a40(0x62, ctx[6] * 16 + 0x24);
        redraw = 1;
        full = 1;
        while (_GetFlag(0x150) == 0) {
            Func_80a1a40(0x62, ctx[4] * 16 + 0x24);
            if (redraw != 0) {
                redraw = 0;
                if (u->items[prev] != 0)
                    Func_80a17c4(st->objs[prev]);
                if (full != 0) {
                    WaitFrames(1);
                    Func_80a56c8(box, 0, ctx);
                    if (which == 0)
                        _Func_801e7c0(0xb89, box, 0, 0x58);
                    full = 0;
                }
                Func_80a5614(box, buf, ctx);
                st->h178[which] = st->list[ctx[6]];
                n = st->arr21a[which];
                Func_80a3ef0(n, ctx[6], 0);
                if (st->list[ctx[6]] != 0) {
                    o = st->objs[ctx[6]];
                    o->f5 = 9;
                    o->fc = 0;
                    o->ff = 0xfa;
                }
                for (i = 0; i < st->f219; i++)
                    _Sprite_SetAnim(st->spr114[i], 1);
            }
            if ((iwram_3001e40 & 0x1f) == 0) {
                for (i = 0; i < st->f219; i++) {
                    if (_CanEquipItem(st->arr[i], st->list[ctx[6]] & 0x1ff))
                        _Sprite_SetAnim(st->spr114[i], 3);
                }
            }
            WaitFrames(1);
            prev = ctx[6];
            r = Func_80a1fd4(0, ctx[5], 5, &ctx[4], &ctx[2]);
            if (r == 1) {
                full = 1;
                redraw = 1;
            }
            if (r == 0)
                redraw = 1;
            if (r == -1)
                redraw = 0;
            if ((gKeyPress & 1) != 0 && st->list[ctx[6]] != 0) {
                _PlaySound(0xad);
                ret = st->list[ctx[6]];
                done = 1;
                break;
            }
            if ((gKeyPress & 2) != 0) {
                _PlaySound(0x71);
                ret = -1;
                done = 1;
                break;
            }
            if ((gKeyRepeat & 0x100) != 0 || (gKeyRepeat & 0x200) != 0) {
                if (which == 1) {
                    _PlaySound(0x72);
                    WaitFrames(1);
                } else {
                    _PlaySound(0x6f);
                    st->f260[st->arr21a[which]] = ctx[6];
                    sel = st->s1c[which];
                    do {
                        if ((gKeyRepeat & 0x100) != 0)
                            sel++;
                        else
                            sel--;
                        n = st->f219;
                        sel = (sel + n) % n;
                        st->f08 = st->arr[sel];
                        st->arr21a[which] = st->arr[sel];
                        st->s1c[which] = sel;
                        st->f218 = Func_80a3ddc(_GetUnit(st->arr21a[which]), st->list, 0);
                    } while (st->f218 == 0);
                    for (i = 0; i <= 3; i++)
                        st->h144[i] = 0x1e;
                    st->h144[sel] = 0x1a;
                    break;
                }
            }
        }
    }
    _Func_80164d4(box, 0, 0x58, 0x78, 0x60);
    Func_80a17c4(st->f44);
    st->h174[which] = ctx[6];
    st->f260[st->arr21a[which]] = ctx[6];
    st->h178[which] = ret;
    if (_GetFlag(0x150) != 0)
        ret = -1;
    WaitFrames(1);
    return ret;
}
