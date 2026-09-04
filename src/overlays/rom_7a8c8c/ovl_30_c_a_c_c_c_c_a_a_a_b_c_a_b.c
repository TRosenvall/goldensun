/* OvlFunc_922_2009154  --  0x02009154
 *
 * Cut out of goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_b_c_a.s.
 *
 * Queues a fixed set of dialogue lines, choosing between alternatives on four
 * save flags. Nine calls to the same six-argument helper and nothing else.
 *
 * TWO SCREENS, AND NO PINS AT ALL. The plain spelling was already 21 of 164
 * with the length exact, and every one of the 21 was the same thing:
 *
 *     rom    mov r3, #0xc / mov r2, #0x8 / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0xc / str r3, [sp] / mov r3, #0x8 / str r3, [sp, #4]
 *
 * A SIX-ARGUMENT CALL PASSES ITS LAST TWO ON THE STACK, and the ROM builds
 * BOTH before storing either, holding them in two registers at once. gcc
 * reuses one register for both -- same instruction count, different registers,
 * so the length never moves and the residue is purely allocation. Assigning
 * the two values to named locals immediately before the call restores it, at
 * seven sites here.
 *
 * All seven were load-bearing by construction: each appeared as its own hunk
 * in the diff of the plain form, and the two sites that ALREADY used named
 * locals -- because the ROM holds those values in r5/r6/r7 across several
 * calls -- were the only __Func_8010704 calls absent from that diff.
 *
 * NOTE WHICH VALUES GET LOCALS AND WHY. The three the ROM keeps in
 * callee-saved registers across calls (0x13, 0xd, 0xc) are ordinary locals
 * assigned once, because gcc holds them exactly as the ROM does. The
 * literal pairs are locals assigned FRESH before each call, because the ROM
 * rebuilds them each time. Same construct, opposite reasons, and the ROM's
 * register choice is what tells them apart -- callee-saved means one local,
 * scratch means rebuild.
 *
 * CHOSEN WITH tools/crossed.py. Three of the five best-ranked candidates
 * carried the mov-order-against-shift-order wall that stalled two functions in
 * the preceding rounds; this one does not, and it matched in two screens with
 * no register pinned anywhere.
 */

extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_922_2009004(int a, int b, int c);

void OvlFunc_922_2009154(void)
{
    int a;
    int b;
    int c;
    int e;
    int g;

    e = 0xc;
    g = 8;
    __Func_8010704(0, 0x1c, 0xa, 0x12, e, g);
    if (__GetFlag(0xc1 << 2) != 0) {
        OvlFunc_922_2009004(8, 0x15, 0x14);
        e = 0xd;
        g = 0x13;
        __Func_8010704(0x14, 0x13, 1, 3, e, g);
    } else {
        OvlFunc_922_2009004(8, 0xd, 0x14);
        e = 0x15;
        g = 0x13;
        __Func_8010704(0x14, 0x13, 1, 3, e, g);
    }
    if (__GetFlag(0x305) != 0) {
        OvlFunc_922_2009004(8, 0xc, 0x14);
        a = 0x13;
        c = 0xc;
        __Func_8010704(5, 0x13, 1, 3, c, a);
        b = 0xd;
        __Func_8010704(0x14, 0x13, 1, 3, b, a);
        if (__GetFlag(0xc1 << 2) != 0) {
            OvlFunc_922_2009004(8, 0x15, 0x14);
            __Func_8010704(0x14, 0x13, 1, 3, b, a);
            __Func_8010704(0x14, 0x13, 1, 3, c, a);
        }
    }
    if (__GetFlag(0x306) != 0) {
        OvlFunc_922_2009004(9, 0xf, 0x15);
        e = 0xe;
        g = 0x11;
        __Func_8010704(0xe, 0x12, 3, 1, e, g);
    } else {
        OvlFunc_922_2009004(9, 0xf, 0x11);
        e = 0xe;
        g = 0x15;
        __Func_8010704(0xe, 0x12, 3, 1, e, g);
    }
    if (__GetFlag(0x307) != 0) {
        OvlFunc_922_2009004(0xa, 0x13, 8);
        e = 0x12;
        g = 0x19;
        __Func_8010704(0xe, 0x12, 3, 1, e, g);
    } else {
        OvlFunc_922_2009004(0xa, 0x13, 0x19);
        e = 0x12;
        g = 8;
        __Func_8010704(0xe, 0x12, 3, 1, e, g);
    }
}
