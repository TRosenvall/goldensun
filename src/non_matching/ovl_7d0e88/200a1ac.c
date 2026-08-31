/*
 * OvlFunc_947_200a1ac -- asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_a.s
 *
 * BLOCKER: register allocation for a rematerialised constant. 54 lines
 * against 54, 9 differing, and the first difference is placement:
 *
 *      rom   mov r4, #0x8 / and r3, r1 / orr r3, r4
 *      ours  and r3, r1  / mov r1, #0x8 / orr r3, r1
 *
 * The ROM materialises the OR constant into r4 BEFORE the AND and keeps it
 * live across both flag updates; we rematerialise it into a scratch register
 * at each use. The two updates then diverge on which register holds the actor
 * pointer.
 *
 * SETTLED, and this one is a NEW LEVER worth reusing:
 *
 *   A MASK APPLIED TO A BYTE GETS NARROWED UNLESS IT IS NAMED. The ROM builds
 *   -13 as `mov r2, #0xd / neg r2, r2` -- two instructions, the full 32-bit
 *   value. Written inline as `p[9] = (p[9] & -13) | 8;` gcc notices the result
 *   is stored back into a byte, truncates the mask to 0xf3, and emits a single
 *   `mov r2, #0xf3`. That is one instruction SHORT and it cascades.
 *
 *   Assigning the mask to an `int mask = -13;` first stops the narrowing and
 *   gives the ROM's mov/neg pair. 53 lines and 42 differing becomes 54 and 9.
 *
 *   An `int` intermediate for the LOADED BYTE (`t = p[9]; p[9] = (t & -13) | 8;`)
 *   does NOT work -- 15 differing. It is the mask that has to be named, not the
 *   value being masked.
 *
 * TRIED AND REJECTED, all measured, all identical at 9 differing:
 *
 *   * `m = 8;` assigned BEFORE `mask = -13;`
 *   * `mask` declared before `m`
 *   * the OR constant left as a bare literal 8 rather than a named local
 *   * `unsigned char m = 8;` -- the narrow type the ORR-destination lever
 *     prescribes. MUCH WORSE: 47 differing and 55 lines, first difference at 8
 *     instead of 20. Tried because that lever closed OvlFunc_946_20092b4 and
 *     OvlFunc_903_2008d68 on a residue that looks identical to this one. It
 *     does not transfer: there the constant is the ORR DESTINATION and here
 *     the ROM's problem is that the constant must stay LIVE across two flag
 *     updates, which a narrow local does not do. The doc's caution that the
 *     two operations must be tried separately is the right reading.
 *
 * Naming the OR constant changes nothing in either direction, which is itself
 * the answer to the obvious next idea.
 *
 * Also settled: the actor pointer must come from a local assigned from the
 * call (`p = __MapActor_GetActor(0xd);` then `*(int *)(p + 0x18) = v;`), not
 * from the call embedded in the store expression. The embedded form swaps the
 * r5/r6 roles of the two long-lived values and moves the first difference from
 * line 16 to line 4.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_947_200a1ac(void)
{
    unsigned char *a;
    unsigned char *p;
    int v;
    int m;
    int mask;
    int s1;
    int s2;

    a = __MapActor_GetActor(0xe);
    p = __MapActor_GetActor(0xd);
    v = 0x80 << 9;
    *(int *)(p + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xd) + 0x1c) = v;
    p = *(unsigned char **)(__MapActor_GetActor(0xd) + 0x50);
    mask = -13;
    m = 8;
    p[9] = (p[9] & mask) | m;
    p = *(unsigned char **)(a + 0x50);
    p[9] = (p[9] & mask) | m;
    *(int *)(a + 0x34) = 0x6666;
    *(int *)(a + 0x30) = 0xcccc;
    __Actor_TravelTo(a, *(int *)(a + 8), 0x80 << 14, *(int *)(a + 0x10));
    __MapActor_WaitMovement(0xe);
    s1 = 0x16;
    s2 = 0x10;
    __Func_8010704(0x14, 0xe, 1, 1, s1, s2);
}
