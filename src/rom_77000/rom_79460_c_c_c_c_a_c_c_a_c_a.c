/* CheckEquipmentCritBoost  --  0x08079cbc
 *
 * Cut out of goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_a.s.
 *
 * Walks a unit's fifteen equipment slots at +0xd8, and for every equipped item
 * whose halfword carries flag 0x200 reads the item record's four ability slots
 * at +0x18.  Slot type 0x17 is the crit bonus; its signed byte is accumulated.
 * A negative total is clamped to zero.
 *
 * WHICH OPERAND IS THE POINTER.  The load is `ldrh r3, [r5, r7]` with r5
 * walking 0xd8, 0xda, ... and r7 holding the argument, so the WALKING OFFSET is
 * the base and the argument is the index.  Taking `rec` as an `int` and walking
 * `unsigned char *p = (unsigned char *)0xd8` produces that; writing `rec` as the
 * pointer and indexing it produces `[r7, r5]` and shifts every later register.
 *
 * WHY THE MASK IS A NAMED `unsigned short` AND THE LOOP COUNTS UP.  Both of
 * those exist to order two preheader instructions, `mov r8, r2` (the mask) and
 * `mov r1, #0xe` (the counter), which sched2 breaks on INSN_LUID.  A bare
 * `0x200` literal is born inside the loop and hoisted by loop.c's
 * move_movables, and a hoist always lands last in the preheader -- after an
 * explicit `i = 0xe`.  Naming the mask `int` is worse still: the AND stops
 * being narrowed to HImode, the two `ldrh`s of the same slot fold into one and
 * the allocator takes r10 (47 differing).  `unsigned short` keeps the HImode
 * AND and both loads; what it changes is that the in-loop copy of the mask now
 * has a two-luid life in loop.c's FIRST pass instead of its second, so
 * move_movables hoists it in pass one -- and check_dbra_loop, which runs after
 * move_movables inside the same pass, then reverses `for (i = 0; i < 15; i++)`
 * and emits its own `i = 14` BEHIND the hoist.  That is the ROM's order.
 */
extern unsigned char *GetItemInfo(int id);

int CheckEquipmentCritBoost(int rec)
{
    unsigned char *p;
    unsigned char *q;
    int i;
    int j;
    int total;
    unsigned short mask;

    total = 0;
    p = (unsigned char *)0xd8;
    mask = 0x200;
    for (i = 0; i < 15; i++) {
        if (*(unsigned short *)(p + rec) & mask) {
            q = GetItemInfo(*(unsigned short *)(p + rec)) + 0x18;
            for (j = 3; j >= 0; j--) {
                if (*q == 0x17)
                    total += *(signed char *)(q + 1);
                q += 4;
            }
        }
        p += 2;
    }
    if (total < 0)
        total = 0;
    return total;
}
