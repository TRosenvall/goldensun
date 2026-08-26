/* Func_80b2ed8 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b0000/rom_b0070_c_c_a_c.s
 * Best screen: 45 instructions against the ROM's 46, and the missing one is a
 * single `mov r0, r5`. Everything before it and everything after it is right;
 * the 19 differing positions are that one-instruction offset cascading.
 *
 * BLOCKER CLASS: gcc propagates a constant straight into the argument register
 * where the ROM stages it through a callee-saved one.
 *
 *     rom    ldr r5, =0xd2c / b .L2 / .L1: ldr r5, =0xd2d / .L2: mov r0, r5 / bl
 *     ours   ldr r0, =0xd2c / b .L2 / .L1: ldr r0, =0xd2d / .L2: bl
 *
 * A TWO-WAY CHOICE OF ADJACENT CONSTANTS -- AND HOW TO GET THE BRANCH BACK.
 * This is worth keeping even though the function did not close, because it is
 * the first time the branchless-nearby-constant blocker has been defeated.
 *
 * 0xd2c and 0xd2d differ by one, so the plain
 *
 *     if (cond) id = 0xd2c; else id = 0xd2d;
 *     s = Func_80b2884(id);
 *
 * gets if-converted: gcc emits the `neg / orr / lsr #31` boolean-normalise
 * idiom and subtracts, with no branch at all (26 differing of 46). Putting the
 * CALL INSIDE EACH ARM --
 *
 *     if (cond) s = Func_80b2884(0xd2c); else s = Func_80b2884(0xd2d);
 *
 * -- blocks the if-conversion, because gcc will not speculate a call. It then
 * cross-jumps the two identical tails back into one `bl`, which is exactly the
 * ROM's shape: two pool loads, a `b`, and one shared call. That took it from 26
 * differing to 19 and made the whole control flow match.
 *
 * So the rule to carry: when the ROM branches over a choice of two constants
 * that gcc insists on making branchless, look for a CALL that can be moved
 * inside the arms. Cross-jumping puts it back.
 *
 * WHAT WAS TRIED AGAINST THE REMAINING INSTRUCTION, all identical at 19:
 *   - the call in each arm, with the constant inline
 *   - the call in each arm, with a named `id` assigned in each arm first
 *     (an attempt to make the cross-jumped tail `mov r0, id / bl`)
 *   - both of those with Func_80b2884's prototype withheld
 *   - withholding Func_80b2778's prototype (this also fixes an r0/r1 rotation
 *     earlier in the function, but the rotation is already right without it)
 *
 * Nothing puts the constant in a callee-saved register first. gcc coalesces the
 * constant's pseudo with r0 because nothing else wants it; the ROM's build did
 * not. That is the same allocator disagreement as
 * src/non_matching/ovl_7ed0a0/2009458.c, seen on a constant rather than on a
 * loaded byte.
 */
extern unsigned char *iwram_3001f2c;
extern void *Func_80b2778(int a, int b);
extern int Func_80b27b0(int a, int b);
extern void *Func_80b2884(int id);
extern void _Func_8016478(int a);
extern void _Func_8019908(void *h, int n);
extern void _DrawSmallText(void *s, int a, int b, int c);

void Func_80b2ed8(int a, int b)
{
    int lang;
    void *h;
    void *s;

    lang = *(signed char *)(iwram_3001f2c + 0x3aa);
    h = Func_80b2778(b, lang);
    if (a != 0) {
        _Func_8016478(a);
        if (Func_80b27b0(b, lang))
            s = Func_80b2884(0xd2c);
        else
            s = Func_80b2884(0xd2d);
        _Func_8019908(h, 5);
        _DrawSmallText(s, a, 0, 0);
    }
}
