/* OvlFunc_901_2008640 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s
 * Best screen: 17 differing of 47, streams the same length.
 *
 * BLOCKER CLASS: instruction scheduling inside one straight-line block.
 *
 * This is another member of the cutscene-bookend family already represented by
 * src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c (matched) and
 * src/non_matching/overlays/2008acc.c (parked): set the lock bit in the actor's
 * flag halfword at +0x64, save the facing short at +6, speak, restore the
 * facing, clear the halfword. The `ldr r3, =2` is the pooled two settled in
 * that matched file, and it behaves here exactly as it does there.
 *
 * EVERY REMAINING DIFFERENCE IS AN ORDERING ONE. The prologue below is the
 * best of six statement orders tried, and it gets the register ASSIGNMENT
 * right -- r8 holds the saved facing, r10 the named zero, matching the ROM --
 * but emits the two `mov r8` / `mov r10` moves BEFORE the OR block where the
 * ROM emits them after, and computes the +0x64 pointer after the `ldrsh`
 * instead of before:
 *
 *      rom    mov r6, r5 / add r6, #0x64 / mov r2, #6 / ldrsh r1, [r5, r2]
 *             ldr r3, =2 / ldrh / orr / strh / mov r8, r1 / mov r1, #0 / mov r10, r1
 *      ours   mov r2, #6 / ldrsh / mov r6, r5 / add r6, #0x64 / mov r8
 *             mov r2, #0 / mov r10 / ldr r3, =sym / ldrh / orr / strh
 *
 * SIX ORDERS MEASURED, and they trade one property against the other:
 *   p, saved, two, OR, zero          20 differ  (r8/r10 swapped)
 *   saved, p, ...                    20 differ  (identical output)
 *   p, saved, zero, two, OR          17 differ  <- kept below
 *   saved, zero, p, two, OR          17 differ
 *   p, zero, saved, two, OR          21 differ
 *   saved and zero after the OR      18 differ  (ldrsh lands too late)
 * Declaration order was also permuted three ways with no effect at all.
 *
 * So the source order decides WHICH high register holds which value, and that
 * part is now right; it does not decide where the moves land relative to the
 * OR, and nothing tried reaches that.
 *
 * ONE USEFUL NEGATIVE FOR ITS SIBLING. This function has the same pool-skip
 * shape as the parked OvlFunc_898_2008acc -- `b .L / <pool> / .L:` sitting in
 * the middle of the tail -- and HERE GCC PLACES IT CORRECTLY, between
 * `mov r0, #1` and `bl __WaitFrames`, exactly as the ROM does. So the pool
 * dump point is not systematically one instruction early in this family; 2008acc
 * is off for a reason specific to 2008acc, and that park should be read with
 * this in mind rather than as evidence of a general placement bug.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_901_2008640(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    unsigned short zero;
    short saved;

    a = __MapActor_GetActor(0xf);
    p = (unsigned short *)((char *)a + 0x64);
    saved = a->f6;
    zero = 0;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1cb4);
    __MapActor_SetAnim(0xf, 0);
    __Func_8092848(0xf, 0, 2);
    __Func_8093040(0xf, 0, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = zero;
}
