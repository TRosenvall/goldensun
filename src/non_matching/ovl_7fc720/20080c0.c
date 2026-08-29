/* OvlFunc_973_20080c0 (RaisePartyLevels) -- NON-MATCHING.
 * Blocker class: ADDRESS CSE across a call -- gcc computes `sp` once and
 * shares it, the ROM computes it twice. 7 of 22, same length.
 *
 *     rom    mov r7, r0 / mov r0, sp / bl Func_80796c4 / cmp r0, #0 / ble
 *            / mov r6, sp / mov r5, r0
 *     ours   mov r6, sp / mov r7, r0 / mov r0, r6 / bl / mov r5, r0
 *            / cmp r5, #0 / ble
 *
 * Two effects of one cause. gcc materialises the buffer address into r6 BEFORE
 * the call and passes a copy, because it needs sp again for the loop pointer
 * afterwards; the ROM passes sp directly and rebuilds it inside the guarded
 * block. And gcc copies the return into r5 before testing it, where the ROM
 * tests r0 and only saves it after the branch -- which follows from the same
 * decision, since r5 and r6 are both being set up ahead of the branch.
 *
 * SOLVED, and worth reusing: THE POST-INCREMENT BELONGS IN THE CALL. The ROM
 * advances the pointer BEFORE the call:
 *
 *     ldrh r0, [r6] / mov r1, r7 / sub r5, #1 / add r6, #2 / bl
 *
 * Written as `f(*p, lv); p++;` gcc emits the increment after the call and the
 * function stands at 9 differing. Written as `f(*p++, lv);` the increment
 * moves ahead of it and the loop body is exact -- 9 down to 7.
 *
 * Tried and rejected: hoisting the count out of the guard by calling
 * Func_80796c4 inside the `if` and again for the count. That is 25 lines
 * against 22 and 23 differing, and it calls the helper twice, which is wrong
 * regardless of the diff.
 *
 * Same family as rom_15000/801c954.c and ovl_7b2078/2008388.c: a value the ROM
 * rebuilds and gcc shares across a call boundary, where the documented remedy
 * does not apply.
 */
extern int __Func_80796c4(void *buf);
extern void OvlFunc_973_20080a0(int id, int levels);

void OvlFunc_973_20080c0(int levels)
{
    unsigned short buf[0x10];
    unsigned short *p;
    int lv;
    int n;

    lv = levels;
    n = __Func_80796c4(buf);
    if (n > 0) {
        p = buf;
        do {
            OvlFunc_973_20080a0(*p++, lv);
            n--;
        } while (n != 0);
    }
}
