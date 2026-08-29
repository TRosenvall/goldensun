/* Func_80a40ac (DiscardFirstUnlockedItem) -- NON-MATCHING.
 * Blocker class: not yet isolated. 59 lines against the ROM's 55, 45
 * differing, diverging at line 3.
 *
 * The control flow is transcribed and believed right -- walk the inventory
 * from slot 0, skip any slot with bit 9 set, stop at the first empty one, and
 * for the first unlocked slot call _Func_80788c4 (quantity + 1) times. The
 * goto form below mirrors the ROM's five labels and its shared exits, where
 * one variable carries both the call result and the return value.
 *
 * WHERE IT GOES WRONG, and this is as far as it got:
 *
 *     rom    mov r3, #0xd8 / ldrh r3, [r0, r3] ... add r0, #0xd8
 *     ours   add r0, #0xd8 / ldrh r2, [r0, #0x0]
 *
 * The ROM reads the first slot with REGISTER-OFFSET addressing from the unit
 * pointer, holding 0xd8 in a register, and only afterwards advances the
 * pointer by the same 0xd8 as an IMMEDIATE. Two different forms of one
 * constant, which is unusual and is probably the thread to pull.
 *
 * Tried:
 *   - the pointer computed once and read through, as below: 45 differing
 *   - reading `*(unsigned short *)(u + 0xd8)` first and computing the pointer
 *     afterwards, to match the ROM's order: 52 differing, WORSE. gcc then
 *     keeps the unit pointer live and reloads, adding an instruction.
 *
 * Not attempted: naming 0xd8 as an offset variable, which is the lever the
 * register-offset read asks for (docs/elevation.md, "Finish the OFFSET before
 * the base"). It is the obvious next step and was left rather than started
 * badly at the end of a round.
 */
extern char *_GetUnit(int id);
extern int _Func_80788c4(int a, int b);

int Func_80a40ac(int who)
{
    unsigned short *p;
    unsigned short v;
    int i, r, q, n;

    p = (unsigned short *)(_GetUnit(who) + 0xd8);
    v = *p;
    r = 0;
    i = 0;
    goto test;
body:
    v = *p;
    if ((v & 0x200) != 0)
        goto next;
    q = v >> 11;
    n = q + 1;
    if (q == 0)
        n = 1;
    if (n == 0)
        goto done;
    do {
        r = _Func_80788c4(who, i);
        n--;
    } while (n != 0);
done:
    if (r != 2)
        return 0;
    goto one;
next:
    i++;
    p++;
    if (i > 0xe)
        goto ret;
    v = *p;
test:
    if (v != 0)
        goto body;
one:
    r = 1;
ret:
    return r;
}
