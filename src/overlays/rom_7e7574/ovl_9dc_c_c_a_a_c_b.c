// fakematch
/* OvlFunc_959_200c704  --  0x0200c704
 *
 * Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c.s.
 *
 * A cutscene beat for actor slot 0x15: emote, turn to three angles with waits
 * between, speak message 0x2411, switch to animation 4, turn once more, then
 * speak the next message.
 *
 * FOUND BY tools/templated.py, which is the point of this one. It scored 1.00
 * -- every callee this function touches already appears together in one solved
 * file -- and it was picked BECAUSE of that rather than because it was small.
 * The two rounds before this chose by size and went 0-for-6.
 *
 * FAKEMATCH. The plain-C floor is 37 of 53, and the blocker is the recorded
 * straight-line one: `0xb0 << 8` is used at TWO call sites, the function has no
 * branch anywhere, so no branch dominates the uses, cse1 commons the constant
 * into r5 and gcse cprop -- being strictly cross-block -- can never undo it.
 * The rule says park or fakematch; this is the same callee family as the two
 * existing fakematches on __Func_8092adc, so it takes the idiom.
 *
 * The bare pin was tried before the barrier, per the batch-189 rule, and won:
 * pins alone give 30, pins plus a volatile barrier give 33.
 *
 * FOUR THINGS ARE LOAD-BEARING, and each was measured by REMOVING it from the
 * finished file rather than by adding it:
 *
 *   1. The pin block on the first `0xb0 << 8` call. Without it, 32 differing.
 *      This is the actual CSE defeat; the rest is scheduling.
 *   2. THE MESSAGE ID IS A NAMED LOCAL, INCREMENTED. The ROM loads 0x2411 into
 *      the callee-saved r5, uses it, then reaches the second message with
 *      `add r5, #1`. Two separate literals give two pool loads and no r5 in the
 *      prologue at all. Writing `msg = 0x2411; ... msg++;` was worth 30 -> 6 --
 *      by far the largest single step, and nothing to do with the fakematch.
 *   3. `do { msg = 0x2411; } while (0)` AS A SCHEDULING BARRIER. Without it the
 *      pool load is hoisted three slots, above the preceding call, because r5
 *      is callee-saved and survives it. 4 differing without, 2 with. This is
 *      the Func_80c0700 finding used deliberately for the first time: the
 *      construct emits no instruction and exists only to stop the hoist.
 *   4. The pin block on __MapActor_Emote. Without it, 2 differing -- the
 *      `ldr r1, =0x101` drifts one slot early.
 *
 * And the last instruction was the TWO-STEP CONSTANT. The ROM builds the third
 * angle as `mov r1, #0xa0 / mov r2, #0 / lsl r1, #7 / mov r0, #0x15`, with the
 * shift BETWEEN the other two argument setups. Written `0xa0 << 7` the shift
 * lands one slot early; written as `w = 0xa0; ... w <<= 7;` with the zero
 * argument pinned, it lands where the ROM has it. Same two-step the other
 * __Func_8092adc fakematches use.
 *
 * WHAT THIS DOES NOT ESTABLISH: the original plainly did not use inline asm.
 * The pins stand in for whatever its build did differently, exactly as in the
 * register-allocation class. Three of the four levers above are ordinary C and
 * would survive if the pins were ever made unnecessary.
 */

extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8092adc(int slot, int angle, int frames);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_959_200c704(void)
{
    int msg;
    int w;

    {
        register unsigned int e0 __asm__("r0") = 0x15;
        register unsigned int e1 __asm__("r1") = 0x101;
        register unsigned int e2 __asm__("r2") = 0x1e;
        __MapActor_Emote(e0, e1, e2);
    }
    __Func_8092adc(0x15, 0xd0 << 8, 0);
    __CutsceneWait(0x32);
    {
        register unsigned int s0 __asm__("r0") = 0x15;
        register unsigned int a0 __asm__("r1") = 0xb0 << 8;
        register unsigned int f0 __asm__("r2") = 0;
        __Func_8092adc(s0, a0, f0);
    }
    __CutsceneWait(0x32);
    w = 0xa0;
    {
        register unsigned int f2 __asm__("r2") = 0;
        w <<= 7;
        __Func_8092adc(0x15, w, f2);
    }
    __CutsceneWait(0x32);
    do { msg = 0x2411; } while (0);
    __MessageID(msg);
    __ActorMessage(0x15, 0);
    __MapActor_SetAnim(0x15, 4);
    __CutsceneWait(0x3c);
    __Func_8092adc(0x15, 0xb0 << 8, 0);
    msg++;
    __CutsceneWait(0x28);
    __MessageID(msg);
    __ActorMessage(0x15, 0);
}