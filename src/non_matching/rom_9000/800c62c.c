/* Func_800c62c (0x0800c62c) -- NON-MATCHING, 207 of 280 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800c62c.c asm/rom_9000/rom_c004_c_c_a.s --func Func_800c62c
 *
 * SIZE 592 == 592 AND INSTRUCTIONS 280 == 280, so the count IS A TRUE DISTANCE.
 * 207 against the reference as committed; 206 once the reference carries
 * `ldr r5, =_SIZE_8009bb8` in place of `=0x2c4` (see the size.sym section).
 * The RELOCATION SEQUENCE is already IDENTICAL -- same nine symbols in the same
 * order, only offsets differ.
 * tools/aligncmp.py: aligned-equal 166 of 280 (59.3%), 139 differing in 56 hunks.
 * SHIMS: 2 register pins, both inside the shared `call_via` helper below -- needs
 * a fakematch.txt row, exactly like its precedent src/rom_c9000/rom_e3958_c_c_c_a.c
 * (also 2 pins, also for `.call_via`).  NOTHING ELSE is pinned.
 *
 * SPLIT: none needed.  asm/rom_9000/rom_c004_c_c_a.s holds ONE function and
 * tools/datacheck.py reports NO required data exports.
 *
 * ================== A size.sym ROW IS REQUIRED: _SIZE_8009bb8 = 0x2c4 =========
 * The candidate will not assemble, and cannot reach 280 instructions, without it.
 * VERIFIED ARITHMETIC, the same method and the same FILE as the admitted
 * _SIZE_800a418: asm/rom_9000/rom_92b8.s:6 starts the ARM routine Func_8009bb8 at
 * 0x08009bb8 and line 168 starts the next ARM routine Func_8009e7c at 0x08009e7c,
 * so the length is 0x08009e7c - 0x08009bb8 = 0x2c4.  Re-derived independently here.
 *
 * CRITERION 1 IN ITS STRONG FORM: 0x2c4 is 0xb1 << 2, so thumb_shiftable_const
 * makes gcc BUILD it (`mov r1,#0xb1 / lsl r1,#2`) and the ROM POOLS it anyway --
 * `ldr r5, =0x2c4`.  A pool word holding such a value cannot have come from a
 * const_int.
 * CRITERION 2, the DMA word: the ROM shares ONE register between galloc_iwram's
 * size argument and the DMA3 count (`mov r1,r5` then `lsr r5,#2 / orr r2,r5`).
 * With a compile-time constant gcc folds `0x84000000 | (len/4)` to a single
 * pooled 0x840000b1 and the `orr` disappears entirely.
 * TWO INDEPENDENT USERS, which none of the existing rows has: this function and
 * Func_800c880 (src/non_matching/rom_9000/800c880.c), which allocates 0x2c4 and
 * DMAs the same Func_8009bb8 in.  That park proposed the row as `_LEN_2c4` and
 * left it to the owner; the value is identical, and `_SIZE_8009bb8` is the name
 * the file's own convention gives it.  Landing either function lands the row.
 *
 * ================== THE PROSE COMMENT IS WRONG IN THREE PLACES ================
 * The reference's `@` block is mostly right -- the cull window really is x in
 * [-32, 272) and y in [-32, 224), and bit 1 of +0x23 really shifts by -0x140.0000
 * and bit 2 by +0x140.0000.  But:
 *   - "Bits 16-17 set the OAM priority" and "bits 18-19 set the tile-type byte"
 *     are both wrong.  `lsl r4,#16 / lsr r0,#30` selects bits 14-15, and
 *     `lsl r4,#18 / lsr r1,#30` selects bits 12-13.  Shifting left by 16 moves
 *     bit k to k+16, so >>30 keeps the ORIGINAL bits 14-15.
 *   - the tile-attribute index is (z >> 20) * 128 + (x >> 20) using the RAW
 *     world coordinates, not camera-relative ones, and the layer stride is
 *     0x30 off a base of 0x130 built as `mov #0x98 / lsl #1`.
 * Treat the rest as a lead, per the batch brief.
 *
 * ================== WHAT CLOSED IT TO EXACT SIZE AND COUNT ====================
 * Four changes, each measured alone.  First draft was 276 instructions / 588
 * bytes; every step is a separate edit.
 *
 * 1. `zero` AS AN INT CARRIER for `*q = 0`.  276 -> ... ; a bare `0` stored
 *    through a `short *` POOLS (`ldr r3, pool` where the ROM has `mov r3,#0`).
 *    Transferred verbatim from src/non_matching/rom_9000/800c880.c lever 4, which
 *    is the same original file's other big draw pass.
 * 2. THE PHANTOM FRAME.  ROM `sub sp, #0x50` with its highest used slot at 0x28 --
 *    36 bytes of frame nothing touches, the same slack Func_800c880 (24 bytes) and
 *    ActorCmd_Player_Climb (68) have.  Reproduced by hanging a pad on the position
 *    quad:  `struct { int v[4]; unsigned char pad[36]; } P;`
 *    A bare unused array is DELETED (gcc-2.96 removes an unused stack array unless
 *    it is stored to), which is why it has to be a member of something live.
 *    pad[32] gives `sub sp,#76`; pad[36] gives the ROM's 80.
 * 3. DO NOT NAME x AND z FOR THE ACTIVITY TEST.  Written as
 *    `x = *(int*)f; z = *(int*)(f+8); if (x != 0 || z != 0)` gcc loads BOTH
 *    up front; the ROM loads x, tests it, and loads z only on the x == 0 path
 *    (into a register it then throws away), reloading z later in the body.  Write
 *    the test inline as `if (*(int *)f != 0 || *(int *)(f + 8) != 0)` and take the
 *    named copies inside the body.
 * 4. THE ONE THAT FIXED BOTH SIZE AND COUNT -- UN-MERGING THE FIRST RELEASE SITE.
 *    The function releases a tile allocation at THREE points and the ROM emits
 *    only TWO `bl Func_8003f78`: the two late ones are cross-jumped onto a shared
 *    tail (.Lc80e), the early one is not.  Written the same way at all three,
 *    gcc merges ALL THREE and the object carries ONE call relocation where the
 *    ref has two -- which is how the fault was found (read the relocation
 *    SEQUENCE, not just the set).  The early site differs in the ROM by computing
 *    `spr + 0x25` BEFORE the call (`add r5,#0x25 / bl / strb r6,[r5]`) where the
 *    merged tail computes it after (`bl / mov r3,r5 / add r3,#0x25 / strb`).  So:
 *        c = spr[0x1c];  spr = spr + 0x25;  Func_8003f78(c);  *spr = kind;
 *    at the early site, and the plain `Func_8003f78(spr[0x1c]); spr[0x25] = ...`
 *    at the other two.  276/588 -> 280/592, both exact.
 * 5. DECLARATION ORDER SETS THE SPILL SLOTS, and gcc assigns them in REVERSE
 *    declaration order.  The ROM's five spills are q=0, cy=4, cx=8, map=0xc,
 *    n=0x10, so the declarations must run n, map, cx, cy, q.  ALL FIVE then land
 *    on the ROM's slots, and so do the two arrays (scale at 0x14, P.v at 0x1c).
 *    208 -> 206 and aligned-equal 58.6% -> 59.6%.  This is a cheap, mechanical
 *    check worth running on any function with more than two spills.
 *
 * Also load-bearing, and NOT to be simplified away:
 *  - `b = &iwram_3001e70; map = b[0]; q = *(b - 2); e = *(b - 3);` -- the
 *    adjacent-globals lever (800c880.c lever 3).  ONE pool word; the ROM reaches
 *    iwram_3001e68 with `sub r3,#8` and iwram_3001e64 with `sub r6,#0xc`.
 *  - three INDEPENDENT walking pointers `e`, `f = e + 8`, `g = e + 0x54`, each
 *    stepped `+= 0x70` in its own statement.  Folding them into one base with
 *    offsets loses the ROM's `mov r1, sl` / `mov r2, r8` shape.
 *  - `f = e; f = f + 8;` as TWO statements (800c880.c lever 2): one expression
 *    lets cse fold the offset into a second pooled constant.
 *  - `spr[9] = (spr[9] & ~0xc) | (prio << 2)` reproduces the ROM's `mov r2,#0xd /
 *    neg r2,r2` mask WITHOUT a bitfield.  Worth recording against the brief's
 *    "a neg-built mask means a bitfield": here the complement -13 comes out of a
 *    plain `& ~0xc` on an int-promoted unsigned char, because the value is
 *    immediately OR-ed and stored back at TWO offsets, so gcc keeps one SImode
 *    constant.  The FORM of the complement was the right tell; the CONCLUSION
 *    that it must be a bitfield was not.
 *  - the y cull is TWO separate signed compares (`> -0x200000`, `< 0xe00000`)
 *    because the value is used afterwards, while the x cull is gcc's merged
 *    unsigned range test -- the same asymmetry Func_800bfa4 documents.
 *
 * ================== THE RESIDUE ==================
 * With size, instruction count, both arrays, all five spill slots, the frame, the
 * relocation SEQUENCE and the branch structure all exact, what is left is which
 * register holds which value -- the wall docs/elevation.md and
 * src/non_matching/rom_9000/rom_bfa4.c both stop at.  The ROM is carrying more
 * live values than gcc gives us, and parks three of them in high registers we
 * leave in low ones:
 *      ROM   sl = e   r7 = f   r8 = g   fp = Func_8000888
 *            ip = e + 0x22 (later sp+0x14)      lr = e + 0x23
 *      ours  sl = e   r7 = f   r8 = g   Func_8000888 rematerialised per call,
 *            e + 0x22 and e + 0x23 in low registers
 *
 * MEASURED against that, none of it reachable:
 *   `register int (*fp)(int,int) __asm__("r11")`  280/592, 226 -- WORSE, and it
 *        costs a third pin.  fp does land in r11; nothing else moves.
 *   `"h" (f)` as the asm constraint instead of `"r" (f)` -- the principled
 *        pin-free way to demand a high register -- ICEs gcc-2.96:
 *        "Internal compiler error in reload_cse_simplify_operands, at
 *        reload1.c:8104".  Worth knowing: the Thumb `h` constraint is not usable
 *        in an asm that also has an output operand.
 *   DROPPING `"lr"` FROM THE call_via CLOBBER LIST.  This one is not a trick and
 *        should be considered on its merits: `.call_via` expands to
 *        `mov r12, pc / bx reg`, which does NOT write lr, so the tree's helper
 *        over-declares the clobber -- and that is exactly what stops gcc parking
 *        e + 0x23 in lr across the two calls the way the ROM does.  Removing it
 *        drops the hunk count 55 -> 50 but leaves aligned-equal flat (59.6% ->
 *        59.3%) and the index count worse (206 -> 221), so it is NOT in this
 *        draft.  Re-test it on the OTHER `.call_via` users before changing the
 *        shared helper; if it helps any of them, the clobber list is simply wrong.
 *   `cam = (int *)(map + 0xe4); cx = cam[0] & mask; cy = cam[1] & mask;` -- the
 *        Func_800bfa4 camera-pointer idiom.  It gets the ROM's PROLOGUE exactly
 *        (`adds r2,r0,#0 / adds r2,#228 / ldr r1,[r2] / ldr r2,[r2,#4]`, four
 *        instructions where ours has six) and then comes out TWO instructions
 *        SHORT overall -- 278/588.  So the prologue shape is right and something
 *        else in the body is two instructions light; finding that is the next
 *        move, and it would give both.  k3 in the screening set is this variant.
 *   `0x22 + (int)e` and `0x23 + (int)e` to force the ROM's constant-first
 *        `mov r0,#0x23 / add r0,sl` -- folds to the same thing, no change.
 *   building h and k late, at their point of use -- 270/572, much worse.
 *
 * NEXT: take the cam variant (278) and find its missing two instructions; the
 * ROM's `mov lr, r0` and `add r0, sl` for e + 0x23 are the two the plain variant
 * is also missing, so the two faults are probably the same one.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void Func_8009bb8(void);
extern int Func_8000888(int, int);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}
extern void Func_8003f78(int slot);
extern void UpdateSprite(unsigned char *spr, int *pos, int *scale, u16 rot);
extern unsigned int iwram_3001e70;
extern unsigned char _SIZE_8009bb8[];

void Func_800c62c(void)
{
    unsigned int *b;
    int n;
    unsigned char *map;
    int cx;
    int cy;
    short *q;
    unsigned char *e;
    unsigned char *f;
    unsigned char *g;
    unsigned char *h;
    unsigned char *k;
    unsigned char *spr;
    int *p;
    int *tbl;
    void *buf;
    int size;
    int kind;
    int prio;
    int tt;
    int c;
    int x;
    int z;
    int dx;
    int dz;
    int w;
    int y;
    int u;
    int zero;
    int scale[2];
    struct { int v[4]; unsigned char pad[36]; } P;
    int (*fp)(int, int);

    b = &iwram_3001e70;
    map = (unsigned char *)b[0];
    cx = *(int *)(map + 0xe4) & 0xffff0000;
    cy = *(int *)(map + 0xe8) & 0xffff0000;
    q = (short *)*(b - 2);
    size = (int)_SIZE_8009bb8;
    buf = galloc_iwram(0x34, size);
    DMA3_COPY(Func_8009bb8, buf, size);
    e = (unsigned char *)*(b - 3);
    zero = 0;
    *q = zero;
    n = 0x3f;
    fp = Func_8000888;
    g = e + 0x54;
    f = e;
    f = f + 8;
    do {
        if (*(int *)e != 0) {
            if (*(int *)f != 0 || *(int *)(f + 8) != 0) {
                kind = *g & 0xf;
                if (kind != 0) {
                    if (kind == 1) {
                        if (q[2] != 0 && g[8] == 0) {
                            spr = *(unsigned char **)(f + 0x48);
                            c = spr[0x1c];
                            spr = spr + 0x25;
                            Func_8003f78(c);
                            *spr = kind;
                        } else {
                            z = *(int *)(f + 8);
                            x = *(int *)f;
                            dz = z - cy;
                            dx = x - cx;
                            w = dz - *(int *)(f + 4);
                            spr = *(unsigned char **)(f + 0x48);
                            if (dx > -0x200000 && dx < 0x1100000
                                && w > -0x200000 && w < 0xe00000) {
                                h = e + 0x22;
                                k = e + 0x23;
                                tbl = *(int **)(map + 0x130 + f[0x1a] * 0x30);
                                p = &tbl[(z >> 20) * 128 + (x >> 20)];
                                if ((f[0x1b] & 1) != 0) {
                                    prio = (unsigned int)(*p << 16) >> 30;
                                    if (prio != 0) {
                                        spr[9] = (spr[9] & ~0xc) | (prio << 2);
                                        spr[0x15] = (spr[0x15] & ~0xc) | (prio << 2);
                                    }
                                }
                                tt = (unsigned int)(*p << 18) >> 30;
                                if (tt != 0)
                                    *h = tt - 1;
                                scale[0] = call_via(fp, *(int *)(f + 0x10),
                                                    *(int *)(spr + 0x18));
                                scale[1] = call_via(fp, *(int *)(f + 0x14),
                                                    *(int *)(spr + 0x18));
                                y = *(int *)(f + 4);
                                u = *(int *)(f + 0xc);
                                P.v[0] = dx;
                                P.v[1] = y;
                                P.v[2] = dz;
                                P.v[3] = u;
                                if ((*k & 2) != 0) {
                                    P.v[1] = y - 0x1400000;
                                    P.v[2] = dz - 0x1400000;
                                    P.v[3] = u - 0x1400000;
                                }
                                if ((*k & 4) != 0) {
                                    P.v[1] += 0x1400000;
                                    P.v[2] += 0x1400000;
                                    P.v[3] += 0x1400000;
                                }
                                UpdateSprite(spr, P.v, scale,
                                             *(unsigned short *)(e + 6));
                            } else if (g[8] == 0 && (1 & spr[0x1d]) == 0) {
                                Func_8003f78(spr[0x1c]);
                                spr[0x25] = 1;
                            }
                        }
                    }
                }
            } else {
                kind = *g & 0xf;
                if (kind == 1) {
                    spr = *(unsigned char **)(f + 0x48);
                    if (g[8] == 0 && (kind & spr[0x1d]) == 0) {
                        Func_8003f78(spr[0x1c]);
                        spr[0x25] = kind;
                    }
                }
            }
        }
        n--;
        g += 0x70;
        f += 0x70;
        e += 0x70;
    } while (n >= 0);
    gfree(0x34);
}
