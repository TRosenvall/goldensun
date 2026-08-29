/* Func_80c1054 -- NON-MATCHING.
 * Blocker class: THE COUNT IS SAVED BEFORE THE TEST, plus the addressing-base
 * choice that follows from it. 23 lines against the ROM's 24, 19 differing.
 *
 *     rom    bl Func_80b6c08 / cmp r0, #0 / ble L0 / mov r7, r5 / mov r6, #0
 *            / mov r5, r0
 *     ours   bl Func_80b6c08 / mov r5, r0 / cmp r5, #0 / ble L0 / mov r6, #0
 *
 * The ROM tests the return value in r0 and only commits it to a callee-saved
 * register INSIDE the guarded block, where it also copies the buffer pointer
 * from r5 to r7 because r5 is about to be reused for the count. gcc saves the
 * count first and tests the copy, so it never needs the buffer copy and comes
 * out a line short.
 *
 * That cascades into the addressing: the ROM ends up with the byte offset in
 * r6 and the buffer in r7 and emits `ldrsh r0, [r6, r7]` -- OFFSET as the
 * addressing base -- while gcc has them the other way and emits `[r7, r6]`.
 * Writing the address as `off + (char *)base` rather than `(char *)base + off`
 * does not flip it; gcc normalises the addition and the base choice follows
 * allocation, not source order.
 *
 * Func_808c2dc in asm/rom_8a000/rom_8ba38_a_a_a.s has the SAME shape -- a
 * count from a call, tested, then a walk -- and fails the same way at 19 of
 * 22, three lines short. One fix would take both.
 *
 * The `f(*p++, x)` lever from ovl_7fc720/20080c0.c is applied in that sibling
 * and is correct there; it is not what is failing.
 */
extern int Func_80b6c08(int kind, void *buf);
extern void Func_80c0f98(int id, int flag);

void Func_80c1054(void)
{
    short buf[0xe];
    short *base;
    int off;
    int n;

    n = Func_80b6c08(3, buf);
    if (n > 0) {
        base = buf;
        off = 0;
        do {
            Func_80c0f98(*(short *)((char *)base + off), 0);
            n--;
            off += 2;
        } while (n != 0);
    }
}
