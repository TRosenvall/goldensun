/* OvlFunc_common2_380 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/common/common2_c_c_c_c_a.s
 * Best screen: 51 instructions against the ROM's 52, 30 differing.
 *
 * BLOCKER CLASS: gcc addresses two stack objects through `sp` where the ROM
 * copies `sp` into a register first.
 *
 *     rom    mov r3, sp / add r4, sp, #8 / str r0, [r3] / str r1, [r3, #4]
 *     ours   add r5, sp, #8 / str r0, [sp] / str r1, [sp, #4]
 *
 * One instruction, and it drags an r4-versus-r5 exchange behind it.
 *
 * NAMING THE POINTERS HELPS, HALFWAY. Writing `q = in;` and using `q[0]`,
 * `q[1]` takes it from 51 differing of 50 to 30 of 51 -- the length is right
 * and the second object's address is computed the same way. Naming a pointer to
 * the STRUCT as well changes nothing further, and naming only the struct
 * pointer is no better than neither.
 *
 * What is right and should not be re-derived: the two guard calls cross-jump
 * into one `return 0`; the `(r.f4 != 0) + 0x7fffffff` tail is the
 * neg/orr/lsr#31 boolean-normalise idiom plus a pooled constant, which is what
 * the source's `!= 0` gives directly; and `0x3c - t` is a plain subtraction
 * rather than gcc's arithmetic.
  *
 * STACK-BUFFER POINTER LEVER: TRIED, NO CHANGE. Naming a stack buffer's
 * address in a pointer local forces gcc to hold it in a callee-saved register
 * and closed OvlFunc_934_20090e0, which was three instructions short without
 * it. This function is ONE short and the ROM holds TWO stack addresses
 * (`mov r3, sp` and `add r4, sp, #8`), so it looked like the same shape. The
 * candidate already named the first; naming the struct as well, with the
 * assignment before and after the first, gives 30 and 31 differing against a
 * baseline of 30. gcc folds the second pointer back to sp-relative addressing
 * either way.
 *
 * Surveyed the whole park corpus for this lever at the same time: 16 parks
 * declare a local array, and only this one has the ROM holding a stack address
 * in a register at all. The lever has no other candidates here.
*/
struct R {
    unsigned char pad00[4];
    int f4;
    int f8;
    int fc;
    int f10;
};

extern void OvlFunc_common2_618(int *in, struct R *r);
extern int OvlFunc_common2_40c(struct R *r);
extern int OvlFunc_common2_3ec(struct R *r);
extern int OvlFunc_common2_3fc(struct R *r);
extern int OvlFunc_common2_41c(int a, int b, int c);

int OvlFunc_common2_380(int a, int b)
{
    int in[2];
    struct R r;
    int t;
    int v;
    int *q;

    q = in;
    q[0] = a;
    q[1] = b;
    OvlFunc_common2_618(q, &r);
    if (OvlFunc_common2_40c(&r))
        return 0;
    if (OvlFunc_common2_3ec(&r))
        return 0;
    if (OvlFunc_common2_3fc(&r) == 0) {
        t = r.f8;
        if (t < 0)
            return 0;
        if (t <= 0x1e) {
            v = OvlFunc_common2_41c(r.fc, r.f10, 0x3c - t);
            if (r.f4 != 0)
                v = -v;
            return v;
        }
    }
    return (r.f4 != 0) + 0x7fffffff;
}
