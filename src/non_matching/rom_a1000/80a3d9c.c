/* Func_80a3d9c  --  0x080a3d9c, asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a.s
 *
 * BLOCKER CLASS: a mask that gcc will either FOLD AWAY or HOIST, and the ROM
 * does neither.
 *
 * WHAT IT DOES
 * Scans the fifteen halfword slots at +0xd8 of a unit record for the first one
 * whose low nine bits equal `want`, and returns its top five bits plus one.
 * Zero if no slot matches -- so the +1 is what makes "slot 0" distinguishable
 * from "not found".
 *
 * THE TWO SPELLINGS AND WHY NEITHER WORKS
 *
 *   As a LITERAL, `(v & 0xf800) >> 11` is folded to a bare `lsr r3, r2, #0xb`.
 *   gcc knows v came from an `ldrh` and is therefore 16 bits, so the mask
 *   cannot change the result. 28 lines against the ROM's 32.
 *
 *   Held in a REGISTER, the AND survives -- but gcc gets it into a register by
 *   HOISTING it out of the loop, `mov r7, #0xf8 / lsl r7, #8` before the first
 *   iteration. 31 lines, and every line inside the loop is displaced.
 *
 * The ROM has it BOTH ways at once: built inline inside the if-body, on every
 * iteration, and the AND kept.
 *
 *     mov r3, #0xf8
 *     lsl r3, #8
 *     and r3, r2
 *     lsr r5, r3, #0xb
 *
 * That is one RTL insn `r3 = 0xf800` that the loop optimiser declined to move
 * and combine declined to fold, and nothing in the source selects between those
 * two behaviours -- picking one gives the other.
 *
 * FLAGS PROBED AND NEGATIVE: -fno-gcse, -fno-expensive-optimizations,
 * -fno-strength-reduce, -fno-thread-jumps, -fno-rerun-cse-after-loop. None
 * moves the hoist. -fno-strict-aliasing is not relevant here; there is no
 * store the load could be reordered against.
 *
 * WHAT DID WORK, and is kept in the source below because it is reusable:
 *
 *   1. `mask & v`, NOT `v & mask`. The ROM's `mov r3, r4 / and r3, r2` puts the
 *      MASK in the destination register. Written the other way round gcc emits
 *      `mov r3, r2 / and r3, r4` and the two registers are transposed for the
 *      rest of the loop. Same constant-as-destination lever as the ORR in
 *      Func_80ad5b4.
 *   2. Splitting the compound condition into nested `if`s. As
 *      `v != 0 && (mask & v) == want` the two tests share a register; nested,
 *      they match the ROM's two separate compares.
 *   3. `mask` as a named local. As a literal, 0x1ff is still a pool load, but
 *      naming it is what lets it be hoisted into r4 the way the ROM does.
 *
 * The one remaining thing the source cannot express is the ROM's `mov r3, r2`
 * immediately after the `ldrh` -- a copy of the loaded value that is then
 * compared and immediately overwritten. `t = v;` is coalesced away. It is the
 * same redundant-copy shape as GetUnit's `mov r3, r14`, and it is one
 * instruction of the four.
 */

extern void *_GetUnit(int id);

int Func_80a3d9c(int id, int want)
{
    unsigned short *p;
    unsigned int mask;
    int i;
    unsigned int v;
    unsigned int t;
    int r;

    p = (unsigned short *)((char *)_GetUnit(id) + 0xd8);
    mask = 0x1ff;
    r = 0;
    i = 0;
    do {
        v = *p;
        t = v;
        p++;
        if (t != 0) {
            t = mask & v;
            if (t == want) {
                t = 0xf800;
                t &= v;
                r = (t >> 11) + 1;
                break;
            }
        }
        i++;
    } while (i <= 0xe);
    return r;
}
