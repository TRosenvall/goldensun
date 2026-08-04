/* OvlFunc_909_200828c  [ovl_79c738]  --  0x0200828c
 *
 * Source asm: goldensun/asm/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.s
 * Same shape: OvlFunc_907_2008240 in rom_79b154/ovl_30_c_a_a_c_a_c_c_c.s.
 *
 * Blocker: CONSTANT CSE ACROSS A CALL. Eighteen instructions against twenty,
 * and the difference starts at the prologue:
 *
 *     rom    push {lr}      ... ldr r0, =0x303 / bl __GetFlag
 *                           ... ldr r0, =0x303 / bl __SetFlag
 *     ours   push {r5, lr}  ... ldr r5, =0x303 / mov r0, r5 / bl __GetFlag
 *                           ... mov r0, r5 / bl __SetFlag
 *
 * The flag id is used twice around a call. gcc hoists it into a callee-saved
 * register, which costs the push and the two moves; the ROM simply loads it
 * twice.
 *
 * **839 hand-written functions load the same pooled constant more than once**,
 * so if this has a fix it is worth a great deal. It is not automatically a
 * blocker for all of them -- gcc reloads rather than CSEs in 68 functions of
 * its own honest output -- but in every one of those the repeated value is a
 * GLOBAL'S ADDRESS THAT GETS DEREFERENCED, so the reload is forced by the call
 * possibly having changed the memory, not by any property of the source.
 *
 * A HYPOTHESIS THAT FAILED, recorded so it is not retried: that the operand
 * was a symbol rather than a literal, by the same argument as the pool tell in
 * area.sym -- gcc always reloads a symbol's address after a call. It does not.
 * Passing `(int)&_FLAG_303` produces exactly the same CSE, because the address
 * is only passed, never dereferenced. A `flag.sym` was written and then
 * deleted rather than left asserting a namespace on a wrong inference.
 *
 * So the open question is narrow: what makes gcc-2.96 decline to CSE a value
 * that is merely passed to two calls? Nothing tried so far.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);

void OvlFunc_909_200828c(void)
{
    __CutsceneStart();
    __MessageID(0x1756);
    if (__GetFlag(0x303)) {
        __MessageID(0x176c);
        __ActorMessage(0xf, 0);
        __SetFlag(0x303);
    }
    __CutsceneEnd();
}
