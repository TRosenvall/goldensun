/* Func_80f07f0 (RenderCreditLine)  --  0x080f07f0.
 * NON-MATCHING, 271 of 290 encodings differ.  Ref 620 bytes / 290 encodings
 * (277 instructions + 13 pool words); ours 584 bytes / 272 encodings
 * (259 instructions + 13 pool words).  THE COUNT IS NOT A DISTANCE -- we are 18
 * encodings SHORT, so objcmp's figure is saturated and cannot rank anything.
 * Position-tolerant, and this is the number to compare candidates on:
 * aligncmp 147 aligned-equal of 290 (50.7%), 189 differing in 30 hunks.
 * The relocation SEQUENCE is already identical -- Func_8004970, _GetFlag,
 * _call_via_r3, _SetFlag, _call_via_r3, _call_via_r3, free, then the four ABS32
 * pool words Func_80008d8 / Func_8001af8 / .Lf11bd / .Lf1770.  Only the offsets
 * move, which is the length shortfall, not a symbol error.
 *
 * ZERO SHIMS (tools/shimcount.py).  Measured under the Makefile's own
 * GCC296_CFLAGS with no flag added.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py src/non_matching/rom_f0000/80f07f0.c \
 *     asm/rom_f0000/rom_f0254_c_c_c.s --func Func_80f07f0
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/aligncmp.py src/non_matching/rom_f0000/80f07f0.c \
 *     asm/rom_f0000/rom_f0254_c_c_c.s Func_80f07f0
 * (`tryc.py --align` is NOT usable on this file: the reference keeps its literal
 * pool inside the function, as the file-mate park already records.)
 *
 * ============================== SPLIT SHAPE ==============================
 *
 * asm/rom_f0000/rom_f0254_c_c_c.s holds THREE functions (Func_80f0614,
 * Func_80f0678, Func_80f07f0) AND a `.rodata` section, so converting anything in
 * it needs a TEXT/DATA SPLIT with the data keeping its own object.
 *
 * `python3 tools/datacheck.py asm/rom_f0000/rom_f0254_c_c_c.s`, re-run after batch
 * 295's over-attribution fix, reports -- and this is CONFIRMED against the three
 * bodies, not taken on trust:
 *     Func_80f0614   reads .Lf1220
 *     Func_80f0678   reads .Lf1220
 *     Func_80f07f0   reads .Lf11bd, .Lf1770
 * The file today exports only `.Lf0a5c`.  The export list for a split that
 * serves THIS function is therefore
 *     .global .Lf11bd
 *     .global .Lf1770
 *
 * *** DO THE SPLIT ONCE FOR ALL THREE, NOT ONCE PER FUNCTION. ***  Two of the
 * three now have C beside them (this park and src/non_matching/rom_f0000/
 * 80f0678.c) and Func_80f0614 is 25 instructions of plainly reachable shape.  A
 * single text/data cut exporting all THREE labels -- `.Lf11bd .Lf1220 .Lf1770`
 * -- serves every function in the file, is byte-neutral by construction, and
 * touches stage1.ld once instead of three times.  The file-mate park argues for
 * exporting `.Lf1220` alone; that was right for ITS function and is not the
 * file's list.
 *
 * ================== WHAT LANDED, AND WHAT IT COST =====================
 *
 * The tail is already right.  The 0x18-iteration tile copy -- eight pool-loaded
 * VRAM addresses 0x6010000..0x601001c against eight mov/lsl source offsets
 * 0x100..0x700 -- comes out instruction for instruction, including the
 * interleaved `mov r4,#0x80 / lsl r4,#N` builds and the `mov r2,#1 / neg r2,r2 /
 * add r12,r2` high-register decrement.  So does the bit-blit nest: `ldr r3,=0x101`
 * reloaded at the top of every row, the shadow pointer as a SECOND register
 * incremented alongside the pen, the shadow store FIRST, `mov r5,#7` with a
 * signed `bge` over the 8 columns, `lsr` on the mask (so the mask is unsigned),
 * and `add r1,#0xf8` for the 0x100 row stride.  `unsigned int` for the glyph byte
 * and the mask, `int` for both counters, is what the branch flavours demand.
 *
 * Load-bearing, each measured by reverting it alone:
 *
 * 1. THE INDIRECT CALLS.  The ROM does not `bl Func_80008d8`; it does
 *    `ldr r3,=Func_80008d8 / bl _call_via_r3`, three times, so all three go
 *    through ONE function-pointer local (src/rom_c9000/rom_e0524.c's idiom).
 *    Written as direct calls the relocation sequence is wrong outright.
 * 2. THE DUPLICATE READ POINTER in the pack loop.  The ROM increments TWO
 *    registers by 2 per inner iteration -- r2, the one it loads through, and r4,
 *    the one the row correction is computed from.  A single pointer is one
 *    instruction short; `u8 *rp = q;` at the top of the outer body with both
 *    `rp += 2` and `q += 2` inside reproduces it.
 * 3. THE ROW CORRECTION WRITTEN ADD-FIRST (`pk += 0x100; pk -= 0x60;`).  Worth
 *    four instructions, and the reason is the blocker below: because 0x100
 *    exceeds Thumb's imm8 the add MUST take a register, and `combine` then cannot
 *    fold `(plus (plus p r6) -0x60)` because r6 is not a constant to it.  Written
 *    the ROM's way round -- subtract first -- gcc folds the pair into one
 *    `add #0xa0`.
 * 4. `unsigned int` LOOP COUNTERS everywhere the ROM branches `bls`.
 *
 * MEASURED AND REJECTED, each a single change from this candidate, all pin-free:
 *   - naming 0x60 / 0xc0 / 0x100 as locals, assigned at the top of the function
 *     (scratch_elev/b300j/s1.c, s4.c) or immediately before the pack loop
 *     (p2.c, q4.c, q5.c): 50.3% against 50.7%, and it does not stop the fold;
 *   - a temporary for the subtraction, `t = pk - n; pk = t + stride;` (r3.c,
 *     p1.c, p4.c): 48.3%;
 *   - `pk = pk - 0x60 + 0x100` as one expression (p3.c): 48.3%;
 *   - `(int)` casts around the pointer arithmetic (q4.c);
 *   - a `do/while` outer loop (q5.c);
 *   - two row pointers reset from row bases each iteration (q1.c) and an
 *     index-based inner loop (q2.c): both make the correction one
 *     `add lr,lr,r3`, which is the ROM's shape lost entirely.
 *
 * ================== THE NAMED BLOCKER: gcse's CPROP ==================
 *
 * The ROM's pack loop keeps 0x60, 0xc0, 0x100 and 0x18 in REGISTERS and cannot
 * fold them.  Three consequences, all visible in the reference:
 *   - the inner count is `mov r5, r8`, a copy from a held register, not
 *     `mov r5, #0x60`;
 *   - the inner loop keeps its ENTRY GUARD, `cmp r7,#0 / beq`, which gcc deletes
 *     the moment it knows the bound;
 *   - the row correction is `sub r3, r1, r7 / add r1, r3, r6` -- a THREE-address
 *     subtract, which on Thumb forces the constant into a register (Thumb's
 *     three-address sub takes imm3 only).
 *
 * gcse.c's `one_cprop_pass` is what we cannot get past.  It is GLOBAL -- it
 * crosses basic blocks, where cse stops at the multi-predecessor loop top -- and
 * it rewrites every use of a constant-assigned local back to a const_int, after
 * which combine folds the pair.  PROVED, not inferred: the temp-variable
 * candidate (scratch_elev/b300j/r3.c) compiled with `-fno-gcse` reproduces the
 * ROM's pack loop essentially verbatim --
 *     cmp r7,#0 / beq  .  mov r1,r5 / mov r0,r7  .  sub r3,r4,r7 / add r4,r3,r1 /
 *     sub r3,r5,r2 / add r5,r3,r1  .  mov r3,lr / cmp r3,#0 / beq
 * -- and scores 284 of 290 encodings with 54.8% aligned-equal, against 264 and
 * 48.3% for the same file under production flags.  NOTE THE SPELLING FLIPS WITH
 * THE FLAG: production wants the add-first correction (this candidate, 50.7%),
 * -fno-gcse wants the subtract-first temporary (r3.c, 54.8%).  Screening one
 * spelling under one flag would have concluded the opposite about both.
 *
 * `-fno-gcse` already exists in the Makefile as GCSE_CFLAGS with one user
 * (rom_8ba38_a_c_b.o), and the note there says it was swept across every park,
 * improves six and matches none.  This is a seventh, and the FIRST where the
 * mechanism is named rather than observed, so the sweep note can be sharpened:
 * the class is "the ROM holds a loop-invariant constant in a register and keeps a
 * loop guard gcc would fold".  It is still a flag, so it is an owner decision and
 * not something to land here.
 *
 * For the record, a four-operand `__asm__ ("" : "+r"(n), "+r"(wide), "+r"(stride),
 * "+r"(tiles))` DOES reach the ROM's pack loop under production flags
 * (scratch_elev/b300j/r1.c: 612 of 620 bytes, 286 of 290 encodings, 52.1%
 * aligned).  It is NOT shipped: one shim carrying four pins for 1.4 points on the
 * aligned scale, it is exactly the "launder a value so flow.c cannot see it"
 * scaffolding docs/elevation.md says not to ship, and it is the wrong shape of
 * fix for a pass-level defect.  It is recorded because it BOUNDS the residue --
 * with the constants held, everything except the allocation below is right.
 *
 * ============ THE SECOND RESIDUE: REGISTER ALLOCATION, AND A FRAME ============
 *
 * Even with the pack loop structurally right (r1.c) half the encodings differ,
 * and the reason is a THREE-WAY ROTATION in the callee-saved assignment that no
 * source change tried moves:
 *     ROM   r7 = align   r8 = width/pen accumulator   r10 = the string cursor
 *     ours  r7 = s       r8 = align                   r10 = pen
 * The consequences are mechanical and they are most of the hunk count.  With `s`
 * in a HIGH register the ROM pays `mov r3,r10 / cmp r3,#0` for the null test and
 * `mov r2,r10 / ldrb r0,[r2] / mov r3,#1 / add r10,r3` for every character
 * advance where we pay two instructions; with the accumulator in r8 it pays
 * `mov r8,r3` to seed it.  In the glyph nest the ROM's eight low registers are
 * all taken by arithmetic (c, pen ptr, shadow ptr, temp, glyph ptr, column, mask,
 * bits) so the string cursor is pushed to r10; ours pushes the GLYPH BYTE to r12
 * instead and keeps the cursor low.  That is `global.c`'s `allocno_compare`
 * priority, `floor_log2(n_refs) * n_refs / live_length` with n_refs weighted by
 * loop depth -- a tie-break between two values, not a shape.
 *
 * AND THERE IS A FRAME WE CANNOT EXPLAIN.  The ROM reserves `sub sp, #0x2c` and
 * addresses THREE words of it (sp+0 the left-edge offset, sp+4 the scratch
 * buffer, sp+8 the row argument; the largest offset anywhere in the body is 8).
 * Thirty-two bytes -- eight words -- are allocated and never touched.  We reserve
 * 12.  That is eight more pseudos given stack slots by reload than this
 * reconstruction produces, which is the allocation rotation told again in the
 * frame: the original translation unit carried materially more live state here
 * than any spelling tried.  It is the same SYMPTOM as
 * src/non_matching/rom_b5000/80bd3e4.c's "a frame the body does not need", but
 * NOT the same cause -- that one is a static chain (this batch proved it by
 * nesting), and here there is no r9 read, no `add rN, sp, #K / mov r9, rN`
 * caller, and no nested callee, so the static-chain explanation is EXCLUDED.
 *
 * ========================= WHAT IT IS =========================
 *
 * Rasterises one credit line.  The credits carry their own 1bpp 8x8 font in
 * .Lf1770 with per-glyph advance widths in .Lf11bd, both indexed by
 * `character - 0x20`; characters below 0x20 are skipped entirely, which is why
 * the credits are English-only.  It measures the string, works out the left edge
 * in a 0xc0-pixel field (alignment 2 = right, 1 = centred, anything else =
 * left), draws every glyph twice into a 0x900-byte 8bpp scratch -- colour 1 at
 * +0x101 for the drop shadow, then colour 0xf at the glyph position -- packs the
 * scratch to 4bpp in place, and copies 0x18 tiles to 0x6010000 + row * 0x20.
 * Save flag 0x200 latches "the scratch has been used once": the first call clears
 * the whole buffer and sets the flag, every later call scrolls the previous
 * contents up with Func_8001af8 and clears only the new row.
 */
#include "gba/types.h"

typedef void (*Fn3)(void *a, int b, int c);

extern void *Func_8004970(int size);
extern void Func_80008d8(void *dst, int len, int val);
extern void Func_8001af8(void *dst, void *src, int len);
extern int _GetFlag(int id);
extern void _SetFlag(int id);
extern void free(void *p);

extern const u8 Lf11bd[] __asm__(".Lf11bd");
extern const u8 Lf1770[] __asm__(".Lf1770");

int Func_80f07f0(u8 *s, int row, int align)
{
    u8 *buf;
    u8 *cur;
    u8 *pk;
    u8 *q;
    int x;
    int w;
    int fw;
    int pen;
    int i;
    int n;
    int wide;
    int stride;
    int tiles;
    unsigned int c;
    Fn3 f;

    buf = Func_8004970(0x90 << 4);
    x = 0;
    fw = 0xc0;
    if (s == 0)
        return -1;
    if (_GetFlag(0x80 << 2) == 0) {
        f = (Fn3)Func_80008d8;
        f(buf, 0x90 << 4, 0);
        _SetFlag(0x80 << 2);
    } else {
        f = (Fn3)Func_8001af8;
        f(buf, buf + (0x80 << 4), 0x80 << 1);
        f = (Fn3)Func_80008d8;
        f(buf + (0x80 << 1), 0x80 << 4, 0);
    }

    cur = s;
    w = 0;
    while ((c = *cur++) != 0) {
        if (c > 0x1f)
            w += Lf11bd[c - 0x20];
    }

    if (align == 2)
        x = fw - w;
    else if (align == 1)
        x = (fw - w) / 2;

    cur = s;
    pen = 0;
    while ((c = *cur++) != 0) {
        if (c > 0x1f) {
            int gi = c - 0x20;
            const u8 *g = Lf1770 + gi * 8;
            u8 *d = buf + x + pen;
            int r;
            for (r = 0; r <= 7; r++) {
                unsigned int bits = *g++;
                unsigned int mask = 0x80;
                u8 *sh = d + 0x101;
                int col;
                for (col = 7; col >= 0; col--) {
                    if (bits & mask) {
                        *sh = 1;
                        *d = 0xf;
                    }
                    sh++;
                    d++;
                    mask >>= 1;
                }
                d += 0xf8;
            }
            pen += (c > 0x1f) ? Lf11bd[gi] : 1;
        }
    }

    tiles = 0x18;
    pk = buf;
    q = buf;
    for (i = 7; i >= 0; i--) {
        u8 *rp = q;
        int k = 0x60;
        while (k != 0) {
            *pk = rp[0] | (rp[1] << 4);
            rp += 2;
            q += 2;
            pk++;
            k--;
        }
        pk += 0x100;
        pk -= 0x60;
        q += 0x100;
        q -= 0xc0;
    }

    if (tiles != 0) {
        u8 *sp = buf;
        int d = row << 5;
        do {
            *(u32 *)(0x6010000 + d) = *(u32 *)sp;
            *(u32 *)(0x6010004 + d) = *(u32 *)(sp + (0x80 << 1));
            *(u32 *)(0x6010008 + d) = *(u32 *)(sp + (0x80 << 2));
            *(u32 *)(0x601000c + d) = *(u32 *)(sp + (0xc0 << 2));
            *(u32 *)(0x6010010 + d) = *(u32 *)(sp + (0x80 << 3));
            *(u32 *)(0x6010014 + d) = *(u32 *)(sp + (0xa0 << 3));
            *(u32 *)(0x6010018 + d) = *(u32 *)(sp + (0xc0 << 3));
            *(u32 *)(0x601001c + d) = *(u32 *)(sp + (0xe0 << 3));
            d += 0x20;
            sp += 4;
            tiles--;
        } while (tiles != 0);
    }

    free(buf);
    return 0;
}
