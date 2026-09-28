/* Func_801e41c -- 0x0801e41c, asm/rom_15000/rom_1de5c_c_a_a.s.
 *
 * ONE function (grep -ci func_start = 1) and NO data section (datacheck.py exit 0,
 * no EXPORTS line), so this is a plain WHOLE-FILE conversion: no split, no export,
 * no linker-script change.  The six `.word` blocks in the reference are gcc's own
 * inline thumb jump tables, not a data section.
 *
 * EXACT: objcmp --whole reports
 *   OK whole file -- 768 bytes, 280 encodings and 92 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_15000/rom_1de5c_c_a_a.s --whole
 *
 * ============================== THE SHAPE ==============================
 * One straight run of border tiles written into the 32-wide tilemap at
 * iwram_3001e8c, each cell stitched against the glyph already there.
 *   b == d  ->  VERTICAL run over rows c..e, pointer stride 32 halfwords (0x40)
 *   c == e  ->  HORIZONTAL run over columns b..d, stride 1 halfword (2)
 * Each run has THREE switches -- first cell, last cell, middle cells -- hence the
 * six jump tables.  Each table's span is read off its own range check: the ROM does
 * `add r3, v, #-0xf009` then `cmp r3, #N / bhi <default>`, so the table covers
 * 0xf009 .. 0xf009+N and every entry pointing at the default label is a value NOT
 * listed as a case.  `case X: break;` with an empty body is a real case here,
 * because the default is not empty -- it assigns 0xf00f (vertical) or 0xf00e
 * (horizontal).
 *
 * The redundant `if (b == d) return;` inside the `else if (c == e)` arm is in the
 * ROM (`cmp r7, r10 / bne .L1e57e / b .L1e6c2`, provably dead on that path) and is
 * kept: removing it changes the instruction stream.
 *
 * ==================== THE FOUR LEVERS, WITH NUMBERS ====================
 * First candidate: 294 instructions against 280, 796 bytes against 768.
 *
 * 1. THE CELL VALUE MUST BE AN SImode LOCAL -- `u32 v`, NOT `u16 v`.
 *    294 -> 280 instructions and 256 differing -> 22.  THE SINGLE BIGGEST LEVER
 *    IN THE FUNCTION.  With `u16 v` the pseudo is HImode, so the load goes through
 *    `*thumb_movhi_insn`'s "m" alternative and prints `ldrsh r1, [r0, r3]` (with a
 *    `mov r3, #0` for the register offset it needs), and the unsigned switch then
 *    costs a `lsl #16 / lsr #16` pair to undo the sign extension.  Declaring `v`
 *    SImode makes the load `zero_extendhisi2` -- the ROM's single `ldrh r1, [r0]`
 *    -- and the switch subtraction reads the loaded register directly.  Three
 *    instructions per loop, in two loops, plus everything downstream.
 *
 * 2. CASE BODIES ARE EMITTED IN SOURCE ORDER WHILE THE DISPATCH IS SORTED BY
 *    VALUE, so the HIGHER-VALUED case of each pair must be written FIRST.
 *    22 -> 6.  Four of the six switches have two distinct non-empty bodies, and
 *    in every one the ROM lays out the higher case's body first:
 *      vertical first cell : 0xf011 -> 0xf018  before  0xf00e -> 0xf009
 *      vertical last cell  : 0xf014 -> 0xf019  before  0xf00e -> 0xf00a
 *      horizontal first    : 0xf016 -> 0xf01a  before  0xf00f -> 0xf00b
 *      horizontal last     : 0xf017 -> 0xf01b  before  0xf00f -> 0xf00c
 *    expand_case sorts the TESTS (so the `cmp`/table order is unchanged) while
 *    emit_case_nodes walks the case list in SOURCE order for the bodies.  Same
 *    mechanism as Func_801b010's tail switch in this batch -- two functions, one
 *    bank, independent confirmation.
 *
 * 3. THE SWAP TEMPORARY IS THE LOOP COUNTER.  6 -> 2.  The ROM's swap is
 *    `mov r4, r8 / mov r8, r6 / mov r6, r4` and r4 is the SAME register the loop
 *    index lives in (`mov r4, r8` again at the loop entry).  A separate `u32 t`
 *    gets ip: `mov ip, r8 / mov r8, r6 / mov r6, ip`.  Writing both swaps through
 *    `i` gives local-alloc one quantity instead of two and it lands in r4.
 *    Block-local `u32 t1` / `u32 t2` per swap is INERT at 6 -- the scope lever does
 *    not reach this; it has to be the same variable.
 *
 * 4. THE POINTER BASE MUST BE ADDED AS AN INTEGER, NOT AS A POINTER.  2 -> 0.
 *    The ROM has `add r0, r3, r2` with r3 the scaled index and r2 the map base;
 *    `map + index` gives `add r0, r2, r3` -- same registers, opposite operand
 *    order, one bit of encoding, at the two sites that compute the run's start.
 *    Commuting it in the SOURCE IS INERT: `index + map`, `&map[index]`,
 *    `(u16 *)((u8 *)map + index * 2)`, `p = map; p += index;` and moving `map`'s
 *    declaration all measure exactly 2.  What works is taking the scaling out of
 *    pointer arithmetic entirely and adding `(int)map`:
 *        p = (u16 *)(((((a->y + c) << 5) + a->x + d) << 1) + (int)map);
 *    `* 2` in place of `<< 1` is BYTE-IDENTICAL; `(int)map` on the LEFT is back to
 *    2.  Pointer arithmetic expands as `(plus (reg ptr) (reg scaled))`, while an
 *    integer add lets combine keep the shift as the first operand, which is the
 *    canonical order the ROM has.  This is the commutative-operand lever from the
 *    brief, and it confirms the brief's framing: it is a PROCEDURE, read per site
 *    off the ROM's own `mov`, and the fix was not in the operand order I wrote but
 *    in the TYPE of the expression that produced it.
 *
 * NO SHIMS: no `register ... __asm__`, no `__asm__ ("")`, no .equ, no .sym entry,
 * no per-file flag, no fakematch row.
 */
#include "gba/types.h"

struct Win {
    u8  pad_00[0xc];
    u16 x;        /* 0x0c */
    u16 y;        /* 0x0e */
};

extern u16 *iwram_3001e8c;

extern void Func_801e260(int x, int y, int w, int h);

void Func_801e41c(struct Win *a, u32 b, u32 c, u32 d, u32 e)
{
    u16 *map;
    u16 *p;
    u32 i;
    u32 v;

    map = iwram_3001e8c;
    if (b == d) {
        if (c == e)
            return;
        if (c > e) {
            i = c;
            c = e;
            e = i;
        }
        Func_801e260(a->x + d, a->y + c, 1, e - c);
        p = (u16 *)(((((a->y + c) << 5) + a->x + d) << 1) + (int)map);
        for (i = c; i <= e; i++) {
            v = *p;
            if (i == c) {
                switch (v) {
                case 0xf009:
                    break;
                case 0xf00a:
                    v = 0xf00d;
                    break;
                case 0xf00b:
                    break;
                case 0xf00c:
                    break;
                case 0xf00d:
                    break;
                case 0xf011:
                    v = 0xf018;
                    break;
                case 0xf00e:
                    v = 0xf009;
                    break;
                case 0xf018:
                    break;
                default:
                    v = 0xf00f;
                    break;
                }
            } else if (i == e) {
                switch (v) {
                case 0xf009:
                    v = 0xf00d;
                    break;
                case 0xf00a:
                    break;
                case 0xf00b:
                    break;
                case 0xf00c:
                    break;
                case 0xf00d:
                    break;
                case 0xf014:
                    v = 0xf019;
                    break;
                case 0xf00e:
                    v = 0xf00a;
                    break;
                case 0xf019:
                    break;
                default:
                    v = 0xf00f;
                    break;
                }
            } else {
                switch (v) {
                case 0xf009:
                    v = 0xf00d;
                    break;
                case 0xf00a:
                    v = 0xf00d;
                    break;
                case 0xf00b:
                    break;
                case 0xf00c:
                    break;
                case 0xf00d:
                    break;
                case 0xf00e:
                    v = 0xf00d;
                    break;
                default:
                    v = 0xf00f;
                    break;
                }
            }
            *p = v;
            p += 32;
        }
    } else if (c == e) {
        if (b == d)
            return;
        if (b > d) {
            i = b;
            b = d;
            d = i;
        }
        Func_801e260(a->x + b, a->y + c, d - b, 1);
        p = (u16 *)(((((a->y + c) << 5) + a->x + b) << 1) + (int)map);
        for (i = b; i <= d; i++) {
            v = *p;
            if (i == b) {
                switch (v) {
                case 0xf009:
                    break;
                case 0xf00a:
                    break;
                case 0xf00b:
                    break;
                case 0xf00c:
                    v = 0xf00d;
                    break;
                case 0xf00d:
                    break;
                case 0xf016:
                    v = 0xf01a;
                    break;
                case 0xf00f:
                    v = 0xf00b;
                    break;
                case 0xf01a:
                    break;
                default:
                    v = 0xf00e;
                    break;
                }
            } else if (i == d) {
                switch (v) {
                case 0xf009:
                    break;
                case 0xf00a:
                    break;
                case 0xf00b:
                    v = 0xf00d;
                    break;
                case 0xf00c:
                    break;
                case 0xf00d:
                    break;
                case 0xf017:
                    v = 0xf01b;
                    break;
                case 0xf00f:
                    v = 0xf00c;
                    break;
                case 0xf01b:
                    break;
                default:
                    v = 0xf00e;
                    break;
                }
            } else {
                switch (v) {
                case 0xf009:
                    break;
                case 0xf00a:
                    break;
                case 0xf00b:
                    v = 0xf00d;
                    break;
                case 0xf00c:
                    v = 0xf00d;
                    break;
                case 0xf00d:
                    break;
                case 0xf00f:
                    v = 0xf00d;
                    break;
                default:
                    v = 0xf00e;
                    break;
                }
            }
            *p = v;
            p += 1;
        }
    }
}
