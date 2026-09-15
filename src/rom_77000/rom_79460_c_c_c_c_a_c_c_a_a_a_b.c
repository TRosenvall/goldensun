/* Func_8079c30 -- 0x08079c30, split out of
 * asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a.s.
 *
 * The twin of Func_8079c5c next door: identical tail (call, two multiplies,
 * signed divide by 0x10000), differing only in that this one passes its second
 * argument straight through where the other biases it.
 *
 * THIS RETIRES A PARK THAT CONCLUDED THE FUNCTION WAS UNREACHABLE. The park
 * (rom_77000/rom_79c30.c, batch 56) recorded the blocker as "MULTIPLY OPERAND
 * CANONICALISATION", measured four spellings at 4 of 19 each, and closed with
 * "NEXT: nothing at the expression level. This wants either a gcc flag that
 * disables commutative canonicalisation -- none is known -- or ... a compiler
 * difference rather than a source one."
 *
 * It is a source one. The four spellings tried were
 *
 *     c * (a * f(b, 0))
 *     t = f(b, 0);  c * (a * t)
 *     t = f(b, 0);  u = a * t;  c * u
 *     a *= t; c *= a;
 *
 * -- every one of which puts the call's result on the RIGHT of the inner
 * multiply. The spelling that works puts it on the LEFT, at the head of a flat
 * left-to-right product:
 *
 *     r = Func_8079b24(b, 0);
 *     return r * a * c / 0x10000;
 *
 * Exact on the first try. The park's own diagnosis was right -- the operand gcc
 * puts first becomes the destructive `mul`'s destination -- but its conclusion
 * that the source cannot control it was wrong; the source controls it by
 * OPERAND ORDER, and the four probes happened to share the order that fails.
 *
 * WHAT THE PARK'S SWEEP MISSED IS A GENERAL SHAPE: it varied PARENTHESISATION
 * and how much was NAMED, and never varied which operand came first. Three of
 * its four spellings are the same expression tree.
 *
 * Folding the product into statements is WORSE -- `r *= a;` or `r = a * r;`
 * both come out 40 bytes against 44, two instructions short, because gcc then
 * commutes the second multiply into the first's register and drops a copy.
 *
 * EXACT: 44 bytes, 20 encodings, 1 relocation, measured three times.
 */
extern int Func_8079b24(int a, int b);

int Func_8079c30(int a, int b, int c)
{
    int r;

    r = Func_8079b24(b, 0);
    return r * a * c / 0x10000;
}
