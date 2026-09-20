/* Cluster ActorCmd_FollowTargetWait..ActorCmd_FollowTargetWait extracted from goldensun/asm/rom_9000/rom_d924_c.s.
 *
 * Total .text for this TU = 148 bytes (= 0x94).
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_d924_c_c_a.o and asm/rom_9000/rom_d924_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 273. No pins, no flags.
 *
 * A script opcode: copy the target's speed pair, then step toward it until within
 * 0x10, scaling the delta by (dist - 0x10) / dist.
 *
 * AN INLINE ARGUMENT EXPRESSION CAN BE A CALL-CROSSING LOCAL, and that was the whole
 * spill. The first transcription was 75 lines against 67 with a spilled `dist` and a
 * `sub sp, #4`, entirely because `*(int *)(a + 0xc)` sat INLINE in the
 * Actor_TravelTo argument list: gcc evaluated it before both __divsi3 calls, which
 * put six values live across them, and reload spilled `dist`. The ROM loads it last.
 *
 * Splitting the two divisions into their own statements -- and assigning the scaled
 * deltas BACK INTO `dx`/`dz` rather than into new locals -- puts the load where the
 * ROM has it and removes both the spill and the `sub sp, #4`. 63 differing to 4.
 *
 * The last four closed by naming `mag`, so the sum of squares completes before the
 * function pointer is materialised.
 *
 * MEASURED on the function pointer: assigned at the TOP of the function it lands in
 * r4 and costs 3; assigned just before `sx` it is 54. Where a value is BORN decides
 * this, which is the recorded "assign a base where the ROM loads it" rule applying to
 * a function pointer.
 */
extern int Func_8000948(int);
extern int Actor_TravelTo(void *actor, int x, int y, int z);
extern void Actor_SetAnim(void *actor, int anim);

int ActorCmd_FollowTargetWait(void *r0)
{
    unsigned char *a;
    unsigned char *t;
    int dx;
    int dz;
    int sx;
    int sz;
    int dist;
    int n;
    int mag;
    int (*fp)(int);

    a = r0;
    t = *(unsigned char **)(a + 0x68);
    *(int *)(a + 0x30) = *(int *)(t + 0x30);
    *(int *)(a + 0x34) = *(int *)(t + 0x34);
    dx = *(int *)(t + 8) - *(int *)(a + 8);
    dz = *(int *)(t + 0x10) - *(int *)(a + 0x10);
    sx = dx >> 16;
    sz = dz >> 16;
    mag = sx * sx + sz * sz;
    fp = Func_8000948;
    dist = fp(mag);
    if (dist > 0x10) {
        n = dist - 0x10;
        dx = dx * n / dist;
        dz = dz * n / dist;
        Actor_TravelTo(a, *(int *)(a + 8) + dx, *(int *)(a + 0xc),
                       *(int *)(a + 0x10) + dz);
        Actor_SetAnim(a, 2);
        *(unsigned short *)(a + 4) += 1;
        return 1;
    }
    Actor_SetAnim(a, 1);
    return 0;
}
