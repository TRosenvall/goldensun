/* OvlFunc_891_2009b44 -- PARKED on pool-constant CSE (the certain, zero-label class).
 * ref: asm/overlays/rom_78c76c/ovl_30_c_c_c_c_a.s
 *
 * As written: 74 raw differing lines / 51 instructions in disagreeing regions,
 * of 66 (ours 76 lines), first diff at position 0 --
 * gcc hoists 0x3333 and 0x1999 into two extra callee-saved registers, which
 * changes the prologue (push {r5,r6,r7,lr} vs the ROM's push {r5,r6,lr}) and
 * cascades through the whole body.
 *
 * MEASUREMENT THAT ISOLATES IT: change the SECOND __MapActor_SetSpeed call to
 * 0x3334/0x199a so the constants are no longer CSE-able and the same source is
 * 2 differing of 66 -- and those 2 are exactly the two literals that were
 * changed.  Everything else is byte-exact.
 *
 * Flags measured, all identical at 74/51 of 66: -fno-rerun-cse-after-loop,
 * -fno-gcse, -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 * -fno-expensive-optimizations, -fno-force-mem; -O1 gives 75 raw.
 * The function has NO labels at all, so there is no boundary for the
 * basic-block lever.
 *
 * Two spellings that WERE load-bearing and should be kept if this is retried:
 *   - `c <<= 4;` and `c += 8;` as separate statements (the ROM's destructive
 *     `lsl r5,#4 / add r5,#8`).  Written `c = (c << 4) + 8;` gcc emits
 *     `lsl r3, r6, #4 / mov r6, r3` and the whole allocation shifts.
 *   - the `c += 8; d += 8;` pair placed AFTER `__MapActor_SetAnim(0, 8)`,
 *     which is what puts `mov r1,#8` before `mov r0,#0` at that call.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_891_2009b44(int slot, int a, int b, int c, int d)
{
    unsigned char *p;

    __CutsceneStart();
    __PlaySound(0xb9);
    __MapActor_SetSpeed(slot, 0x3333, 0x1999);
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    p = __MapActor_GetActor(slot) + 0x5a;
    *p = 0xfe & *p;
    c <<= 4;
    d <<= 4;
    __MapActor_SetAnim(0, 8);
    c += 8;
    d += 8;
    __MapActor_TravelTo(0, c, d);
    a <<= 4;
    b <<= 4;
    a += 8;
    b += 8;
    __MapActor_TravelTo(slot, a, b);
    __MapActor_WaitMovement(slot);
    __MapActor_SetAnim(0, 1);
    __CutsceneEnd();
}
