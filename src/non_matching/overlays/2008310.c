/* OvlFunc_899_2008310  --  0x02008310, asm/overlays/rom_794ac0/ovl_30_a_c_a_a_c_a_c_c_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_a_c_a_c_c_c.s
 *
 * BLOCKER CLASS: gcc rewrites a signed LOWER bound. THIRD member of that class,
 * after OvlFunc_899_2008048 and Func_80a3ce4, and it sits at the same 2-line
 * floor those do:
 *
 *     rom    cmp r3, #0xf / blt L0
 *     ours   cmp r3, #0xe / ble L0
 *
 * gcc-2.96 canonicalises every signed lower bound to `cmp #(K-1) / ble` and
 * leaves upper bounds alone; the ROM's compiler does not. 23 of 25 identical.
 *
 * TWO THINGS WERE SOLVED GETTING HERE and both are worth keeping.
 *
 *   THE COMPOUND CONDITION FUSES. `if (v <= 0x11 && v >= 0xf)` becomes one
 *   unsigned range check --
 *
 *       sub r3, #0xf / lsl r3, #16 / mov r2, #0x80 / lsl r2, #10 / cmp r3, r2
 *
 *   -- 20 of 27 differing. Nesting it as two `if`s gives the ROM's two separate
 *   compares. A `goto` form does the same; the nested `if` is preferred as the
 *   more likely source.
 *
 *   gState IS A STRUCT. Reached as `unsigned char gState[]` with `*(short *)
 *   (gState + 0x1c2)`, gcc folds the offset into the pool entry; the member
 *   access gives the ROM's `ldr r3, =gState / add r3, r2 / ldrsh r3, [r3, r2]`.
 *   Same lesson as Func_80b10cc in this batch.
 *
 * The three tables are reached with gcc's asm-label extension, which needs no
 * change anywhere else. Not to be retried without a new idea about the bound.
 */
typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    unsigned char pad1c4[0x2c0 - 0x1c4];
} GlobalState;

extern GlobalState gState;
extern int __GetFlag(int id);
extern short L61fc[] __asm__(".L61fc");
extern short L6250[] __asm__(".L6250");
extern short L5e30[] __asm__(".L5e30");

void *OvlFunc_899_2008310(void)
{
    short v;

    v = gState.f1c2;
    if (v <= 0x11) {
        if (v >= 0xf)
            return L61fc;
    }
    if (__GetFlag(0x855))
        return L6250;
    return L5e30;
}
