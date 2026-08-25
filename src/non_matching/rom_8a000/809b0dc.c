/* Func_809b0dc  --  0x0809b0dc, asm/rom_8a000/rom_9ad70_c_a.s
 *
 * BLOCKER CLASS: the width of a POOLED CONSTANT's load.
 * Status: 29 lines against 29. ONE differing instruction.
 *
 *      rom    ldr  r3, =0x0
 *      ours   ldrh r3, .L1
 *
 * WHAT IT DOES
 * One per-frame step for a falling entity: two position words at +0x18 and
 * +0x1c drop by 0x280, a halfword at +6 and a word at +0xc advance, and once
 * the +0x18 word falls below 0x3000 a flag byte at +0x54 is cleared.
 *
 * THE ZERO IS POOLED BY BOTH COMPILERS -- ours as a halfword, the ROM's as a
 * word. That is worth stating plainly because the obvious reading is wrong:
 * `ldr r3, =0x0` where `mov r3, #0` would do looks exactly like the pool tell,
 * and it is NOT one here. gcc pools it too. The operand is a plain zero and the
 * only disagreement is the load width, which is the halfword-pooling behaviour
 * in docs/elevation.md section 1b: gcc narrows a pooled constant to match the
 * width of the eventual store, and this one is a `strb`.
 *
 * WHAT WAS TRIED
 *   - the zero through a named int local: 24 lines, 19 differing. WORSE --
 *     gcc folds the local away and then re-optimises the whole tail.
 *   - the flag field declared `int` and stored through an `unsigned char *`
 *     cast, to keep the value int-typed at the store: byte-identical.
 *
 * Both fail the same way. The width follows the store instruction, and the
 * store instruction is fixed by the field being a byte -- which the ROM's
 * `strb` confirms. There is no room between those two facts.
 *
 * Everything else matches, including the `-0x280` shared between two adds and
 * the two shifted constants.
 */

struct S {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[4];
    int fc;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x34];
    unsigned char f54;
};

void Func_809b0dc(struct S *p)
{
    int d;
    int v;

    d = -0x280;
    p->f1c += d;
    v = p->f18 + d;
    p->f6 += 0x80 << 6;
    p->fc += 0x80 << 9;
    p->f18 = v;
    if (v < 0xc0 << 6)
        p->f54 = 0;
}
