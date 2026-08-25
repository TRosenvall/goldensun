/* OvlFunc_880_2008384  --  0x02008384, asm/overlays/rom_7795e8/ovl_30_c_c.s
 *
 * BLOCKER CLASS: gcc will not use `negsi2` on a comparison result.
 * Status: 32 lines against the ROM's 31. ONE extra instruction, at the very
 * end; the first twenty-six are exact.
 *
 * WHAT IT DOES
 * Returns 0 unless save flag 0x144 is set and the entrance id at gState+0x23e
 * is not 2, in which case it returns -1 when the area is not _AREA_02 and 0
 * when it is. So: a three-way gate collapsed into a 0 / -1 answer.
 *
 * THE ONE INSTRUCTION
 *      rom    ... lsr r0, #0x1f / neg r0, r0
 *      ours   ... lsr r0, #0x1f / mov r3, #0x0 / sub r0, r3, r0
 *
 * Both compute the same thing. gcc has a `negsi2` pattern and uses it happily
 * elsewhere -- but not here, where the operand is the result of a comparison
 * that it has just materialised with the branchless `neg / orr / lsr #31`
 * sequence. It materialises a zero and subtracts instead.
 *
 * THE TWO REQUIREMENTS PULL OPPOSITE WAYS, which is what makes this a floor
 * rather than a spelling problem:
 *
 *   Consumed directly by arithmetic -- `return -(a != b);` -- the comparison
 *   comes out BRANCHLESS, matching the ROM's neg/orr/lsr, and the negate comes
 *   out as mov+sub. 32 lines.
 *
 *   Routed through a named local -- `t = (a != b); t = -t;` -- the negate comes
 *   out as `neg r0, r0`, matching the ROM, and the comparison becomes a BRANCH
 *   (`cmp / beq / mov #1`). Also 32 lines, differing in a different place.
 *
 * Seven spellings tried, all 31-or-32 and never both halves at once:
 *   -(a != b)                            branchless, mov+sub
 *   0 - (a != b)                         identical
 *   ~(a != b) + 1                        identical
 *   t = -(a != b); return t;             identical
 *   t = a ^ b; return -(t != 0);         identical
 *   t = a ^ b; t = (t != 0); t = -t;     neg, but branchy comparison
 *   if (a != b) return -1; return 0;     30 lines, diverges at instruction 5
 *
 * This is the same SHAPE of impasse as the mask in Func_80a3d9c and the
 * constant in the batch-70 bitfield case: two behaviours that the ROM has
 * together and that no single spelling produces together. In those two the
 * answer was a different construct entirely (a bitfield). If there is one here,
 * it is not any arithmetic rearrangement of the same expression.
 *
 * `_AREA_02` is a pool tell -- 2 fits `mov r2, #2`, so pooling it means the
 * operand was a symbol -- and it was already defined. It is compared against
 * the gState halfword at +0x1c0 by way of the `eor`, which is area.sym's own
 * criterion applied through an equality test rather than a `cmp`.
 */

typedef struct {
    unsigned char pad[0x1c0];
    short area;
    unsigned char pad1c2[0x7c];
    short f23e;
} GlobalState;

extern GlobalState gState;
extern int _AREA_02;
extern int __GetFlag(int id);

int OvlFunc_880_2008384(void)
{
    if (__GetFlag(0xa2 << 1) == 0)
        return 0;
    if (gState.f23e == 2)
        return 0;
    return -(gState.area != (int)(&_AREA_02));
}
