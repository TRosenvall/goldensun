/* OvlFunc_882_2008064  --  0x02008064, cut from goldensun/asm/overlays/rom_77dd1c/ovl_30_a_a.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/overlays/rom_77dd1c/ovl_30_a_a.o in goldensun/overlays/rom_77dd1c/overlay.ld.
 * A four-phase nudge on a pair of 20.12 offsets at +0x18 and +0x1c, driven by a
 * countdown halfword at +0x64. Phase 0 recentres both to 0x10000 and reloads
 * the counter with `Random() % 0x5a + 0x3c`; phases 2, 4 and 6 push the pair in
 * different directions. Every path falls through to the decrement and returns 1.
 *
 * One of TWO byte-identical copies -- OvlFunc_882_2008064 and
 * OvlFunc_943_2008030. Found with tools/find_twins.py.
 *
 * MATCHED ON THE FIRST SCREEN, with two rules doing the work:
 *
 *   THE SAME MEMBER IS READ SIGNED AND UNSIGNED, and `short` gives both.
 *   `switch (a->f64)` gets `ldrsh` -- the sign decides which case runs -- and
 *   `a->f64--` gets `ldrh`, because the result is stored straight back into a
 *   halfword and the sign cannot reach the answer. Declaring it
 *   `unsigned short` breaks the switch. Fourth function on this rule.
 *
 *   THE `switch` REPRODUCES THE ROM'S DECISION TREE. The ROM tests `== 2`,
 *   then `> 2`, then `== 0` -- a balanced tree over the case values {0,2,4,6},
 *   not a chain -- and a plain `switch` emits exactly that. Written as an
 *   if/else chain it does not.
 *
 * The shared tails are gcc's, not the source's: the ROM joins cases 2 and 4 at
 * the `f1c` load and cases 4 and 6 at the add, which cross-jumping produces
 * from four independent `break` arms.
 *
 * `_umodsi3_RAM` needs `__umodsi3 = _umodsi3_RAM;` in this overlay's linker
 * script, the same link-time alias overlay division has used since batch 68.
 * gcc emits `__umodsi3` for `%` and has no flag to rename it.
 */

struct A {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    short f64;
};

extern unsigned int __Random(void);

int OvlFunc_882_2008064(struct A *a)
{
    switch (a->f64) {
    case 6:
        a->f18 += -0x4000;
        a->f1c += 0x80 << 6;
        break;
    case 4:
        a->f18 += 0x80 << 6;
        a->f1c += -0x1000;
        break;
    case 2:
        a->f18 += 0x80 << 5;
        a->f1c += -0x800;
        break;
    case 0:
        a->f18 = 0x80 << 9;
        a->f1c = 0x80 << 9;
        a->f64 = __Random() % 0x5a + 0x3c;
        break;
    }
    a->f64--;
    return 1;
}
