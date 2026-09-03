/* Func_80a8508  --  0x080a8508
 *
 * Cut from goldensun/asm/rom_a1000/rom_a7380_c.s (fourth of five); the trailing
 * .rodata stays with the tail piece, which needed one label exported first --
 * that export is a separate, byte-neutral commit. Split verified byte-neutral.
 *
 * Draws the label pair for the selected entry of a five-slot list, or a fallback
 * line when the list is empty.
 *
 * A CONSTANT BASE PLUS AN INDEX, THEN +1: PUT THE CONSTANT IN THE ACCUMULATOR
 * FIRST. New lever, and the mechanism was READ out of the compiler rather than
 * inferred. The symptom is that every natural spelling of `id = K + i*2;` then
 * `id + 1` emits a SECOND pool load of K+1 and a fresh add, where the ROM keeps
 * one pool load and a plain increment.
 *
 * cse's associative path, reached for a PLUS, looks up the register operand as a
 * PLUS expression, asks whether its second operand has a known constant value,
 * and if so rewrites `id + 1` as `shift + (K+1)`. Confirmed in the .03.cse dump:
 * the increment insn is rewritten from adding one to adding K+1.
 *
 * THE ESCAPE IS TO MAKE THAT REGISTER STOP BEING A KNOWN CONSTANT. Writing
 * `id = K; id += i*2;` reuses ONE pseudo, so after the accumulate its quantity
 * is no longer constant, the lookup returns nothing, and the plain increment
 * survives. Read in the winner's own .03.cse dump.
 *
 * A COROLLARY THAT EXPLAINS WHY A NEIGHBOURING LEVER DOES NOT TRANSFER: cse's
 * related-value path requires a CONST expression, so it never applies to plain
 * integer constants -- only to symbol-plus-offset. That is why the recorded
 * message-id lever, which produces an increment from a SYMBOL, does not carry
 * over to a numeric id. The numeric case needs this different lever, and the
 * sibling's note actively points the wrong way.
 *
 * A PARTIAL CREDIT THAT COULD HAVE BEEN MISTAKEN FOR THE ANSWER: passing the
 * post-increment as the call argument is worth 7 to 4 on its own, because it
 * fixes the line count and moves the increment above the call. It is NOT the
 * lever -- once the base lever is in place, the post-increment and a separate
 * increment statement are interchangeable and both exact. Seven flags reach
 * none of this.
 *
 * The callee grep won outright: two solved files gave the argument order, the
 * id's type, and the one-variable-incremented shape, producing a 7-differing
 * first screen with no structural probing. The stem-sibling shares no callee
 * with this function and was worth nothing.
 */
extern void _DrawSmallText(int id, void *w, int x, int y);

void Func_80a8508(void *w, int which, unsigned char *flags)
{
    int i;
    int n;
    int id;

    n = 0;
    for (i = 0; i <= 4; i++) {
        if (flags[i]) {
            if (which == n) {
                id = 0xbdc;
                id += i * 2;
                _DrawSmallText(id, w, 0, -1);
                id += 1;
                _DrawSmallText(id, w, 0, 0xf);
            }
            n++;
        }
    }
    if (n == 0)
        _DrawSmallText(0xbda, w, 0, 0);
}
