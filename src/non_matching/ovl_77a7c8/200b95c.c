/* OvlFunc_881_200b95c -- 0x0200b95c  [asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s]
 *
 * NOT MATCHING. 74 lines against 74, SEVEN differing -- of which ONE IS A
 * PHANTOM and six are real. The .s also holds OvlFunc_881_200b9fc; no split was
 * done, since it would be wasted until the body lands.
 *
 * Every third frame, spawn a sparkle at one of the four diagonal neighbours of
 * the party leader's tile.
 *
 * THE PHANTOM: `bl __umodsi3` against the ROM's `bl _umodsi3_RAM`.
 * overlays/rom_77a7c8/overlay.ld already carries `__umodsi3 = _umodsi3_RAM;`
 * (line 111), so that line resolves at link and is not a defect. Two existing
 * parks record the same phantom. It is counted in the 7 only because tryc.py
 * compares mnemonics, not the link.
 *
 * BLOCKER CLASS: argument-fill scheduling, three sites, six lines. The ROM
 * emits `lsl r2, r6, #0x10` (the third argument) BEFORE `mov r1, #0x1` (the
 * magnitude of the second) in cases 0, 2 and 3; we emit them the other way
 * round. Case 1 has OUR order in the ROM too, and the reason is visible: there
 * the x-offset constant lives in r2 (`mov r2, #0x80 / lsl r2, #9 / add r0, r2`)
 * so r2 is busy and the `mov r1` has to go first. In the other three arms the
 * constant is in r3, r2 is free, and the ROM fills it first. So the residue is
 * WHICH REGISTER HOLDS THE PER-ARM CONSTANT, not the argument order as such.
 *
 * SEVEN SPELLINGS MEASURED, and the failures are the informative part:
 *
 *     inline expressions (this file)                          7
 *     operand order swapped in the y expression               7
 *     operand order swapped in the x expression               7
 *     third argument named, PINNED to r2                     24
 *     third argument named and pinned, plus a pinned `-1`    26
 *     third argument named, unpinned                         47
 *     third argument named, unpinned, with a barrier         49
 *
 * NAMING THE ARGUMENT IS DECISIVELY WRONG HERE, and that is the reusable part:
 * every named form is three to seven times worse, because the name buys the
 * value a register of its own and the four arms stop sharing their tail. This
 * is batch 216's "do not name an intermediate that is consumed immediately"
 * confirmed from the failure side, on a function where the inline form is
 * already within six lines.
 *
 * The two operand-order swaps tying at exactly 7 says gcc canonicalises the
 * addition, so those two are one spelling, not two.
 *
 * WHAT IS ALREADY RIGHT: the gState array idiom, both `ldrsh` register-offset
 * reads, `iwram_3001e40 % 3` through the overlay's aliased helper, the
 * `__Random() * 4 >> 16` switch value as an UNSIGNED shift, the four diagonal
 * offsets in 16.16, and the cross-jumped tail that all four arms share. The
 * near-twin src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c_b.c matched with
 * the same idioms and LITERAL positions; this one computes them from the
 * actor's own tile, which is the only difference and the whole residue.
 *
 * NEXT: vary WHICH REGISTER the per-arm constant lands in, not how the
 * arguments are written. Nothing tried here touched that.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern void __Func_80933f8(int a, int b, int c, int d);

void OvlFunc_881_200b95c(void)
{
    unsigned char *g;
    unsigned char *a;
    int x;
    int y;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    x = *(short *)(a + 0xa);
    y = *(short *)(a + 0x12);
    if (iwram_3001e40 % 3 == 0) {
        switch (__Random() * 4 >> 16) {
        case 0:
            __Func_80933f8((x << 16) - 0x10000, -1, (y << 16) + (0x80 << 9), 1);
            break;
        case 1:
            __Func_80933f8((x << 16) + (0x80 << 9), -1, (y << 16) - 0x10000, 1);
            break;
        case 2:
            __Func_80933f8((x << 16) + (0x80 << 9), -1, (y << 16) + (0x80 << 9), 1);
            break;
        case 3:
            __Func_80933f8((x << 16) - 0x10000, -1, (y << 16) - 0x10000, 1);
            break;
        }
    }
}
