/* OvlFunc_887_2008e34 (0x02008e34) -- NON-MATCHING.
 *
 * NON-MATCHING, 20 of 73 STREAM encodings, NO PINS, NO DEVICES
 *   (IMPROVED FROM 25 THIS BATCH -- 343, brief B, on the ANCHOR axis.)
 *   IN PHASE: 47 reference instructions against 47, size equal.  The 20 is a
 *   TRUE DISTANCE.
 *   RELOCATIONS: the SAME 18 symbols in the SAME order on both sides.  Five call
 *   offsets (0x32..0x56 on the reference) run +2 on our side and then come back
 *   into exact agreement from 0x5e onward.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/2008e34.c \
 *     asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s \
 *     --func OvlFunc_887_2008e34
 *
 * >> CORRECTION TO THE PREVIOUS HEADER, AND IT MATTERS FOR HOW THE FIGURE READS.
 *    It said "RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET --
 *    which this project treats as a CONSEQUENCE of the length difference".
 *    THERE IS NO LENGTH DIFFERENCE: 47/47 instructions, 73/73 stream encodings,
 *    sizes equal.  The shift is a LOCAL +1 / -1 pair INSIDE the body, which is a
 *    different thing and a better clue -- it localises the whole residue to the
 *    region between the `__Func_80925cc` call and the `__SetFlag` site. <<
 *
 * ALSO CORRECTED: line 1 of the previous header named
 * `.../ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a.s`, which a later split
 * consumed.  Its `Verify with:` recipe named `..._a_a_c.s`, which is the live
 * file; that is what is used above.  `tools/piece.py` reports ONE function in
 * that `.s`, so this park SHIPS ALONE -- no split, no stage1.ld edit.  (There is
 * exactly one `2008e34.c` in the tree, so the overlay shared-address-space
 * ambiguity `piece.py` warns about does not apply to this target.)
 *
 * ============ WHAT CHANGED: ONE ANCHOR, DEVICE-FREE ============
 *
 * `do { } while (0);` after `__Func_809280c(0xd, 0, 0xa);` -- i.e. immediately
 * before the `__GetFlag(0xc0 << 2)` test.  25 of 73 -> 20 of 73, sizes still
 * equal.  Found by `tools/autosweep.py`'s anchor axis (boundary 06 of 17).
 *
 * WHAT IT FIXES, read off the streams: WITHOUT the anchor, the two instructions
 * that build the flag id 0x300 are SPLIT ACROSS the `__Func_80925cc` call --
 * `movs r5,#0xc0` lands before it and `lsls r5,#2` after it.  With the anchor
 * they sit together at the same place the ROM builds the constant.  So the
 * anchor is doing exactly what the anchor does: it is a scheduling fence
 * (haifa-sched.c:3735-3736 -- the notes hang on the FOLLOWING insn), and the
 * defect it removes is a sched2 motion, not the CSE.
 *
 * ============ THE ROOT DEFECT SURVIVES, AND IT IS THE SAME CLASS AS
 * ============ src/non_matching/ovl_7e7574/200a69c.c ============
 *
 * The flag id 0x300 is used TWICE on one path -- `__GetFlag(0xc0 << 2)` in the
 * test and `__SetFlag(0xc0 << 2)` inside the arm -- and gcc commons it into a
 * callee-saved register:
 *
 *     ROM    movs r0,#0xc0 / lsls r0,#2   ... movs r0,#0xc0 / lsls r0,#2
 *     ours   movs r5,#0xc0 / lsls r5,#2   ... adds r0,r5,#0  ... adds r0,r5,#0
 *
 * so we `push {r5, r6, lr}` / `pop {r5, r6}` where the ROM pushes and pops r5
 * alone, and the arc bound `0x90 << 8` is displaced from r5 to r6.  The entry
 * condition is the one 200a69c's park derived from the vendored compiler and it
 * reproduces here: `precompute_register_parameters` (calls.c:849-856) forces a
 * fresh pseudo for any argument that is not already a REG and whose
 * `rtx_cost (value, SET)` exceeds two, whenever
 * `SMALL_REGISTER_CLASSES && *reg_parm_seen`; and `arm_rtx_costs`
 * (arm.c:2075-2082) charges a Thumb CONST_INT under 256 ZERO and a
 * `thumb_shiftable_const` COSTS_N_INSNS(2) = 6.  0x300 is 768, so it ALWAYS
 * gets a pseudo; cse pass one then unifies the two sites' pseudos by value.
 *
 * The exact 20, decomposed:
 *   6 encodings   0x300 holds r5, so `0x9000` is displaced to r6 -- indices
 *                 0 (push), 5, 7, 8, 45 and 62 (pop)
 *   3 encodings   the commoning itself: `adds r0,r5,#0` at our 28 and 37 where
 *                 the ROM rebuilds `movs r0,#0xc0 / lsls r0,#2`
 *   1 encoding    `bne` reach (d10a vs d109), a knock-on of the above
 *  10 encodings   the alignment shadow those cast
 *   2 encodings   argument ORDER at `__Func_8093054(0xd, 0)`: the ROM emits
 *                 `movs r1,#0 / movs r0,#13`, we emit `movs r0,#13 / movs r1,#0`
 * => close the commoning and 18 of the 20 go with it.
 *
 * ** NEW NEGATIVE, AND IT IS THE INTERESTING ONE.  cse.c:5741-5745's volatile
 * ** ASM_OPERANDS FLUSH -- the lever that took the sibling park 200a69c from 29
 * ** to 10 -- IS COUNTER-PRODUCTIVE HERE.  Measured:
 * **     `__asm__ volatile ("" :: "i"(0))` before BOTH 0x300 sites, no anchor
 * **                                                           27 of 73 (WORSE)
 * **     the same barriers PLUS the winning anchor              27 of 73 (WORSE)
 * **     the same barriers, anchor, and a second anchor         27 of 73 (WORSE)
 * ** The flush does separate the two pseudos, and the function still gets
 * ** longer-looking in the figure, so the class is NOT uniformly barrier-
 * ** reachable.  The difference from 200a69c is that THERE the commoned constants
 * ** were pure call arguments in a straight-line script, whereas HERE one of the
 * ** two uses is the controlling expression of an `if`, so the flush also drops
 * ** the compare's own equivalences.  Recorded so nobody reaches for the barrier
 * ** on this park on the strength of the sibling. **
 *
 * MEASURED INERT (stream terms, all 47/47 instructions and equal size):
 *   anchor before `__SetFlag` only, no other change ........ 25 (inert)
 *   anchor before `__Func_8093054` only .................... 25 (inert)
 *   the winning anchor + an anchor before `__SetFlag` ...... 20 (inert on top)
 *   the winning anchor + an anchor before `__MessageID(0x1c14)` 20 (inert)
 *   the winning anchor + an anchor before `__Func_8093054` . 20 (inert)
 *   `0x300` / `0x9000` written as plain hex rather than
 *     `0xc0 << 2` / `0x90 << 8`, on the winning anchor ..... 20 (inert)
 * The last row re-confirms the previous header's "no spelling separates two
 * occurrences of one value": gcc folds every arrangement to the same constant
 * and then shares it.
 *
 * FLAG AXIS, all eight per-file groups on the PARKED body (autosweep, batch 343):
 *   -fno-schedule-insns2 ... 23 (better than the 25 base, WORSE than 20)
 *   -fno-rerun-cse-after-loop 27 (worse);  -O1 43 (worse)
 *   -fno-gcse / -ffixed-r7 / -fno-strict-aliasing / -fno-strength-reduce /
 *     -fno-rerun-loop-opt ... 25, all exactly inert
 * The sched2 group is subsumed: the anchor reaches the same sched2 motion
 * device-free and goes further, so there is no reason to spend a per-file flag
 * rule here.  NOT YET CROSSED: the flag groups against the ANCHORED body.
 *
 * WHAT IS RIGHT and should be kept: every call and every argument, both arms of
 * the `__GetFlag(0x87a)` test, the pool CONTENTS and ORDER (0xffffe000, 0x87a,
 * 0x1c14, 0x1c15, 0x815, 0x11a9, 0xf58), the `(unsigned int)(e->f6 - 0x2000) >
 * (0x90 << 8)` range test and its `ldr r2 / adds r3,r3,r2 / cmp` expansion, and
 * the fact that `0x90 << 8` is the same value later passed to `__Func_8092adc`
 * in r1 -- gcc's CSE of THOSE two is what the ROM does too.
 *
 *   >> NEXT: this is a PRESSURE question, not a spelling one, and the pin-free
 *      route needs the 0x300 allocno to LOSE both callee-saved low registers.
 *      global.c:987-997 makes the candidate set LO_REGS minus call-used minus
 *      fixed when `calls_crossed` is non-zero, and only `0x9000` competes for it
 *      here, so with two registers free and two contenders both win.  Either a
 *      third simultaneously-live pseudo >= 256 is found (there is none
 *      semantically), or this belongs in pass 3 beside 200a69c. <<
 */
struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80b3284(int inn, int slot);
extern void __Func_80925cc(int slot, int n);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093054(int slot, int n);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_887_2008e34(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0x2000) > (0x90 << 8)) {
        __Func_80b3284(0, 0xd);
        return;
    }
    __CutsceneStart();
    if (__GetFlag(0x87a)) {
        __Func_80925cc(0xd, 2);
        __Func_809280c(0xd, 0, 0xa);
        do { } while (0);
        if (__GetFlag(0xc0 << 2) == 0) {
            __MessageID(0x1c14);
            __ActorMessage(0xd, 0);
            __SetFlag(0xc0 << 2);
        }
        __MessageID(0x1c15);
        __Func_8093054(0xd, 0);
        __Func_8092adc(0xd, 0x90 << 8, 0xa);
    } else {
        if (__GetFlag(0x815))
            __MessageID(0x11a9);
        else
            __MessageID(0xf58);
        __ActorMessage(0xd, 0);
    }
    __CutsceneEnd();
}
