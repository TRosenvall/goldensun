/*
 * Func_8003e58 (FindObjTileRun) -- asm/rom_c0/rom_3e58_a.s
 *
 * BLOCKER: the ROM carries one address in TWO registers and we carry it in one.
 * 63 lines against 65.
 *
 * The alloc-table address lives in r12 (skip and fill) with a low copy in r7
 * (loop head and scan). We keep a single register, so `mov r7, r12` is absent
 * and three addressing forms differ downstream:
 *
 *      rom   ldrb r3,[r7,r4]           ours  add r2,r4,r6 / ldrb r3,[r2]
 *      rom   add r2,r0,r7              ours  (absent -- gcc reuses)
 *      rom   mov r2,r12 / ldrb r3,[r2,r4]   ours  ldrb r3,[r6,r4]
 *
 * TRIED AND REJECTED for the second register, all measured: two locals `t` and
 * `u = t` (CSE merges them, 61 lines); a second local assigned from the global
 * (61); that assignment before and after the first (61 both); and ALL SIXTEEN
 * combinations of local-versus-bare-global at the four use sites (59-63 lines,
 * 47-59 differing). The local/global mix is the only thing that yields two
 * pseudos at all, and gcc then splits them across the wrong sites.
 *
 * A BOUNDARY WORTH RECORDING: four spellings of the fill store through the bare
 * global -- `G[r+j]`, a named offset, `*(G + (r+j))`, `*(G + o)` -- are
 * byte-identical to each other. THE BASE-FIRST/INDEX-FIRST LEVER DOES NOT REACH
 * A BARE GLOBAL ARRAY; it only works through a pointer local. The existing
 * entry for that lever does not state this.
 *
 * wram.sym has no second symbol at 0x03001810, so the two-distinct-symbols
 * trick is unavailable.
 *
 * SETTLED, and each was worth several instructions:
 *   * A `goto` loop for the outer scan. The ROM rebuilds both 0x200 and -1
 *     every iteration, which is the documented selection signature; a
 *     `while (1)` leaves the -1 outside.
 *   * A `goto` loop for the fill loop too -- a `for` gets strength-reduced into
 *     a pointer induction, and the ROM recomputes `start + j` each iteration.
 *   * ONE variable serving as both the not-found return value and the run
 *     start, assigned `r = -1` INSIDE the loop body. gcc-2.96 always merges two
 *     textually separate `return -1;` statements into one shared block (probed
 *     three ways, including with distinct constants), so a ROM that
 *     materialises the same constant at two exits is telling you the source
 *     used one variable, not two returns.
 *   * Guard-then-do/while for the inner scan, which places `add r2,r0,r7` after
 *     the guard.
 */
struct SpriteSlot { unsigned short size; unsigned short vramOffset; };
extern struct SpriteSlot gSpriteSlots[];
extern unsigned char gSpriteAllocTable[];

int Func_8003e58(unsigned int slot, unsigned int size)
{
    unsigned int n;
    int i;
    int r;
    unsigned int end;
    unsigned int j;
    unsigned char *p;
    unsigned char *t;
    unsigned char *s;

    n = size >> 6;
    if (slot > 0x5f)
        return -1;
    t = gSpriteAllocTable;
    i = 0;
    s = (unsigned char *)gSpriteSlots;
loop:
    r = -1;
    if (i >= 0x200)
        goto done;
    if (*(gSpriteAllocTable + i) == 0xff) {
        r = i;
        end = n + r;
        if (r < end) {
            p = r + gSpriteAllocTable;
            do {
                if (*p++ != 0xff)
                    goto skip;
                i++;
            } while (i < end);
        }
        j = 0;
        if (j >= n)
            goto shift;
    fill:
        *(gSpriteAllocTable + (r + j)) = slot;
        j++;
        if (j < n)
            goto fill;
    shift:
        r = r << 6;
        goto done;
    }
skip:
    i += *(unsigned short *)(s + (*(t + i) << 2)) >> 6;
    goto loop;
done:
    return r;
}
