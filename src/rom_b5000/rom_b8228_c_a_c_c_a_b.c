/* Func_80b8f08  --  0x080b8f08
 *
 * Cut out of goldensun/asm/rom_b5000/rom_b8228_c_a_c_c_a.s.
 *
 * Picks a random target from a candidate list: fetch the unit named by the
 * caller's +0xa field, and unless it is already flagged, ask Func_80b6b40 to
 * fill a stack array with candidates and return one of them at random.
 *
 * TWO LEVERS, and both have now been used twice in the tree.
 *
 * THE CALL GOES INSIDE EACH ARM. `Func_80b6b40(id > 0x7f ? 2 : 1, buf)` gets
 * if-converted -- gcc computes the 1-or-2 branchlessly and makes one call.
 * The ROM branches and duplicates `mov r6, sp` into both arms, which is what
 * cross-jumping leaves after two separate calls are merged:
 *
 *     if (id > 0x7f) n = Func_80b6b40(2, buf);
 *     else           n = Func_80b6b40(1, buf);
 *
 * gcc will not speculate a call, so the if-conversion cannot happen; it then
 * merges the identical tails back into one `bl`. Same lever as
 * src/non_matching/rom_b0000/80b2ed8.c, where it was found. 29 differing of 40
 * down to 2.
 *
 * THE MULTIPLY'S OPERAND ORDER IS REACHABLE, unlike `and`/`orr`'s. The last two
 * instructions were `mov r3, r5 / mul r3, r0` against our `mov r3, r0 /
 * mul r3, r5`. Thumb's `mul rd, rs` is two-operand, so one input has to become
 * the destination, and here the source's operand order decides which:
 * `Random() * n` gives the ROM, `n * Random()` gives the reverse. Worth noting
 * because the same question on a commutative bitwise op is NOT reachable --
 * see src/non_matching/ovl_7ed0a0/2009458.c. Multiply and mask behave
 * differently.
 *
 * BATCH 95'S OPERAND-ORDER RULE DOES NOT DECIDE THE FINAL READ -- checked,
 * because the ROM's `ldrsh r0, [r6, r3]` has the base first and that is the
 * shape pointer arithmetic produces. Both `buf[i]` and
 * `*(short *)((char *)buf + (i << 1))` compile to the same forty instructions
 * here. The rule held on src/rom_15000/rom_20198_c_c_c_a_a_c_a_b.c, where the
 * base was an extern pointer; a stack array is not the same case, and the
 * subscript is kept below because it is clearer.
 */
struct X { unsigned char pad00[0xa]; short fa; };
struct U { unsigned char pad00[0x38]; short f38; };

extern struct U *_GetUnit(int id);
extern int Func_80b6b40(int kind, short *buf);
extern unsigned int Random(void);

int Func_80b8f08(struct X *x)
{
    short buf[14];
    int id;
    int n;

    id = x->fa;
    if (_GetUnit(id)->f38 != 0)
        return id;
    if (id > 0x7f)
        n = Func_80b6b40(2, buf);
    else
        n = Func_80b6b40(1, buf);
    if (n == 0)
        return 0x80 << 1;
    return buf[Random() * n >> 16];
}
