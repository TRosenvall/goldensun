/* Func_8020b64 -- asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s
 *
 * BLOCKER: REGISTER ROTATION. 47 of 61, LENGTH EXACT.
 *
 * Builds a display string in a 0x14-byte stack buffer: copy a NUL-terminated
 * source, append control bytes 8 and 2, pad with 0x5f out to width 7, append
 * 8, 0xf and a terminator, then call Func_801e858(buf, a, 0, -2).
 *
 * TWO TELLS WERE READ CORRECTLY and together they took it from 56 differing at
 * 58 lines to 47 at 61 -- the length is now exact and the block structure,
 * including the `b L2` over the else-arm and all three labels, lines up:
 *
 *   1. THE CHARACTER IS TWO NAMES. The ROM shuffles it through two registers
 *      at both sites -- `ldrb r2, [r1] / mov r3, r2` before the loop and
 *      `ldrb r3, [r1] / mov r2, r3 / mov r3, r2` inside it. That double move
 *      is a QImode value round-tripping through an SImode pseudo, i.e. an
 *      `unsigned char` holding the byte and an `int` carrying the test.
 *      Writing both made the loop body the right length.
 *
 *   2. THE BUFFER BASE IS ITS OWN NAME. `mov r0, sp / mov r5, r0` keeps the
 *      base in one register and walks another, so the source has a base
 *      pointer and a separate cursor -- not one pointer used for both. The
 *      else-arm reloads the base (`.L20b8c: mov r0, sp`), which is why the C
 *      below assigns it in BOTH arms.
 *
 * WHAT REMAINS is the rotation, and every difference is an instance of it:
 *
 *     rom    c->r2  t->r3  n->r4  p->r5  a->r6  buf->r0
 *     ours   c->r0  t->r3  n->r2  p->r4  a->r6  buf->r5
 *
 * `t` and `a` agree; the other four are permuted. Same class as
 * src/non_matching/rom_77000/8079664.c and rom_b5000/80c0228.c. Nothing in
 * this batch has moved a rotation of more than two registers, and the
 * assignment-position lever that fixed an r0/r1 pair on Func_80d66cc is
 * recorded there as not reaching callee-saved pairs.
 *
 * NOTE ON THE SCREEN: this reference keeps its pool inside the function, so
 * per docs/elevation.md the count is advisory in both directions. The 47 is
 * reported here as what tryc said, not as a verified byte count -- the
 * function was never installed, because a whole-function rotation is not a
 * near miss worth a build cycle.
 */
extern void Func_801e858(char *buf, int a, int b, int c);

void Func_8020b64(int a, unsigned char *src)
{
    char buf[0x14];
    char *base;
    char *p;
    int n;
    int count;
    unsigned char c;
    int t;

    c = *src;
    t = c;
    n = 0;
    if (t != 0) {
        base = buf;
        p = base;
        do {
            *p = c;
            src++;
            c = *src;
            t = c;
            p++;
            n++;
        } while (t != 0);
    } else {
        base = buf;
    }
    base[n] = 8;
    n++;
    base[n] = 2;
    n++;
    if (n <= 6) {
        p = base + n;
        count = 7 - n;
        do {
            count--;
            *p = 0x5f;
            p++;
        } while (count != 0);
        n = 7;
    }
    base[n] = 8;
    n++;
    base[n] = 0xf;
    n++;
    base[n] = 0;
    Func_801e858(base, a, 0, -2);
}
